// Created by Petr Karnakov on 31.01.2021
// Copyright 2021 ETH Zurich

int err_print(const char* fmt, ...);
void err_exit(int);

#define ERR(x)                                              \
  do {                                                      \
    fprintf(stderr, "%s: %s:%d: ", me, __FILE__, __LINE__); \
    err_print x;                                            \
    fputs("
", stderr);                                    \
    err_exit(2);                                            \
  } while (0)

#define MSG(x)                                      \
  do {                                              \
    fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
    err_print x;                                    \
    fputs("
", stderr);                            \
  } while (0)
