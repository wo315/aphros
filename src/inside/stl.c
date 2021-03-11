// Created by Petr Karnakov on 31.01.2021
// Copyright 2021 ETH Zurich

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "err.h"
#include "memory.h"

#define USED(x) \
  if (x)        \
    ;           \
  else {        \
  }

enum { SIZE = 999 };
int stl_read(
    FILE* f, int* status, int* pnt, int** ptri, int* pnv, double** pver) {
  enum { Stl, FacetStart, OuterStart, FacetEnd, OuterEnd, Vertex, End };
  char line[SIZE];
  char* s;
  double* ver;
  double x;
  double y;
  double z;
  int i;
  int j;
  int nt;
  int nv;
  int state;
  int* tri;
  int v;
  int cap;
  int iver;

  state = Stl;
  v = 0;
  cap = 10;
  if ((ver = malloc(cap * sizeof *ver)) == NULL) {
    fprintf(stderr, "%s:%d: malloc failed
", __FILE__, __LINE__);
    goto err;
  }
  for (;;) {
    if ((s = fgets(line, SIZE, f)) == NULL) break;
    while (isspace(*s))
      s++; /* leading spaces */
    for (i = 0; s[i] != ' '; i++)
      if (s[i] == '
' || s[i] == '#') {
        s[i] = ' ';
        break;
      }
    for (i = j = 0; s[i] != ' '; i++) /* trailing spaces */
      if (!isspace(s[i])) j = i;
    s[j + 1] = ' ';
    if (s[0] == ' ') /* empty line */
      continue;
    switch (state) {
      case Stl:
        if (strncmp(s, "solid", 5)) goto not_stl;
        state = FacetStart;
        break;
      case FacetStart:
        if (!strncmp(s, "endsolid", 8))
          state = End;
        else if (!strncmp(s, "facet normal", 12))
          state = OuterStart;
        else {
          fprintf(
              stderr,
              "%s:%d: expecting 'facet normal' or 'endsolid', got '%s'
",
              __FILE__, __LINE__, s);
          goto err;
        }
        break;
      case OuterStart:
        if (strncmp(s, "outer loop", SIZE)) {
          fprintf(
              stderr, "%s:%d: expecting 'outer loop', got '%s'
", __FILE__,
              __LINE__, s);
          goto err;
        }
        iver = 0;
        state = Vertex;
        break;
      case Vertex:
        if (strncmp(s, "vertex", 6)) {
          fprintf(
              stderr, "%s:%d: expecting 'vertex', got '%s'
", __FILE__,
              __LINE__, s);
          goto err;
        }
        if (sscanf(s, "vertex %lf %lf %lf", &x, &y, &z) != 3) {
          fprintf(
              stderr, "%s:%d: expcting vertices got '%s'
", __FILE__, __LINE__,
              s);
          goto err;
        }
        if (v + 2 >= cap) {
          cap *= 2;
          if ((ver = realloc(ver, cap * sizeof *ver)) == NULL) {
            fprintf(stderr, "%s:%d: realloc failed
", __FILE__, __LINE__);
            goto err;
          }
        }
        ver[v++] = x;
        ver[v++] = y;
        ver[v++] = z;
        iver++;
        if (iver == 3) state = OuterEnd;
        break;
      case OuterEnd:
        if (strncmp(s, "endloop", SIZE)) {
          fprintf(
              stderr, "%s:%d: expecting 'endloop', got '%s'
", __FILE__,
              __LINE__, s);
          goto err;
        }
        state = FacetEnd;
        break;
      case FacetEnd:
        if (strncmp(s, "endfacet", SIZE)) {
          fprintf(
              stderr, "%s:%d: expecting 'endfacet', got '%s'
", __FILE__,
              __LINE__, s);
          goto err;
        }
        state = FacetStart;
        break;
      case End:
        fprintf(stderr, "%s:%d: extra line '%s'
", __FILE__, __LINE__, s);
        goto err;
        break;
    }
  }
  if (state != End) {
    fprintf(stderr, "%s:%d: stl file is not complite
", __FILE__, __LINE__);
    goto err;
  }

  nv = v / 3;
  nt = nv / 3;
  if ((tri = malloc(3 * nt * sizeof *tri)) == NULL) {
    fprintf(stderr, "%s:%d: malloc failed
", __FILE__, __LINE__);
    goto err;
  }
  for (i = 0; i < nv; i++)
    tri[i] = i;
  *pnv = nv;
  *pnt = nt;
  *ptri = tri;
  *pver = ver;
  *status = 0;
  return 0;
err:
  return 1;
not_stl:
  *status = 1;
  return 0;
}

int stl_write(int nt, const int* tri, int nv, const double* ver, FILE* f) {
  int i;
  int j;
  int k;
  int t;
  USED(nv);
  if (fprintf(f, "solid stl
") < 0) {
    fprintf(stderr, "%s:%d: fail to write
", __FILE__, __LINE__);
    goto err;
  }
  for (t = 0; t < nt; t++) {
    i = tri[3 * t];
    j = tri[3 * t + 1];
    k = tri[3 * t + 2];
    fprintf(f, "  facet normal 0 0 1
");
    fprintf(f, "    outer loop
");
    fprintf(
        f, "      vertex %.16e %.16e %.16e
", ver[3 * i], ver[3 * i + 1],
        ver[3 * i + 2]);
    fprintf(
        f, "      vertex %.16e %.16e %.16e
", ver[3 * j], ver[3 * j + 1],
        ver[3 * j + 2]);
    fprintf(
        f, "      vertex %.16e %.16e %.16e
", ver[3 * k], ver[3 * k + 1],
        ver[3 * k + 2]);
    fprintf(f, "    endloop
");
    fprintf(f, "  endfacet
");
  }
  fprintf(f, "endsolid stl
");
  return 0;
err:
  return 1;
}
