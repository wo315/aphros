#include <memory>
#include <cmath>

#include "geom/mesh.h"
#include "solver.h"
#include "util/height.h"
#include "debug/isnan.h"


namespace solver {

template <class M_>
struct UNormal<M_>::Imp {
  static constexpr size_t dim = M::dim;

  static auto Maxmod(Scal a, Scal b) -> Scal {
    return std::abs(b) < std::abs(a) ? a : b;
  }

  // Computes normal by gradient.
  // uc: volume fraction
  // mfc: boundary conditions for volume fraction
  // Output: modified in cells with msk=1
  // fcn: normal [s] 
  static void CalcNormalGrad(M& m, const FieldCell<Scal>& uc,
                             const MapFace<std::shared_ptr<CondFace>>& mfc,
                              FieldCell<Vect>& fcn) {
    auto uf = Interpolate(uc, mfc, m);
    auto gc = Gradient(uf, m);
    for (auto c : m.AllCells()) {
      Vect g = gc[c];
      fcn[c] = g;
    }
  }
  // Computes normal by Young's scheme (interpolation of gradient from nodes).
  // fcu: volume fraction
  // fci: interface mask (1: contains interface)
  // Output: modified in cells with fci=1, resized to m
  // fcn: normal with norm1()=1, antigradient of fcu [s] 
  // XXX: uses static variables, not suspendable
  // TODO: check non-uniform mesh
  static void CalcNormalYoung(M& m, const FieldCell<Scal>& fcu, 
                              const FieldCell<bool>& fci,
                              FieldCell<Vect>& fcn) {
    static FieldNode<Vect> g;  // gradient
    static FieldNode<Vect> l;  // step from cell to node
    g.Reinit(m, Vect(0));
    l.Reinit(m, Vect(0));
    // values from cells to neighbour nodes
    for (auto c : m.AllCells()) {
      Vect xc = m.GetCenter(c);
      for (size_t q = 0; q < m.GetNumNeighbourNodes(c); ++q) {
        IdxNode n = m.GetNeighbourNode(c, q);
        Vect xn = m.GetNode(n);
        for (size_t d = 0; d < dim; ++d) {
          g[n][d] += (xc[d] - xn[d] > 0. ? 1. : -1.) * fcu[c];
          l[n][d] += std::abs(xc[d] - xn[d]);
        }
      } 
    } 
    // gradient on nodes
    for (auto n : m.SuNodes()) {
      g[n] /= l[n];
    }

    // gradient on cells
    fcn.Reinit(m);
    for (auto c : m.SuCells()) {
      if (fci[c]) {
        // sum over neighbour nodes
        auto& v = fcn[c];
        v = Vect(0);
        for (size_t q = 0; q < m.GetNumNeighbourNodes(c); ++q) {
          IdxNode n = m.GetNeighbourNode(c, q);
          v += g[n];
        }
        // normalize
        v /= -v.norm1();
      }
    }
  }
  // CalcNormalYoung: optimized implementation
  static void CalcNormalYoung1(M& m, const FieldCell<Scal>& fcu, 
                               const FieldCell<bool>& fci,
                               FieldCell<Vect>& fcn) {
    using MIdx = typename M::MIdx;
    auto ic = m.GetIndexCells();
    auto bc = m.GetSuBlockCells();
    MIdx s = ic.GetSize();
    const size_t nx = s[0];
    const size_t ny = s[1];
    // offset
    const size_t fx = 1;
    const size_t fy = nx;
    const size_t fz = ny * nx;

    // index range
    const MIdx wb = bc.GetBegin() - ic.GetBegin();
    const MIdx we = bc.GetEnd() - ic.GetBegin();
    const size_t xb = wb[0], yb = wb[1], zb = wb[2];
    const size_t xe = we[0], ye = we[1], ze = we[2];

    fcn.Reinit(m);

    const Vect h = m.GetCellSize();

    const Scal* pu = fcu.data();
    Vect* pn = fcn.data();
    const bool* pi = fci.data();
    for (size_t z = zb; z < ze; ++z) {
      for (size_t y = yb; y < ye; ++y) {
        for (size_t x = xb; x < xe; ++x) {
          size_t i = (z * ny + y) * nx + x;
          if (!pi[i]) {
            continue;
          }
          auto q = [i,fy,fz,pu](int dx, int dy, int dz) {
            size_t ii = i + dx * fx + dy * fy + dz * fz;
            return pu[ii];
          };
          // generated by gen/normal.py
          pn[i][0] = (1.0/32.0)*(-q(-1,-1,-1) - 2*q(-1,-1,0) -
          q(-1,-1,1) - 2*q(-1,0,-1) - 4*q(-1,0,0) - 2*q(-1,0,1) -
          q(-1,1,-1) - 2*q(-1,1,0) - q(-1,1,1) + q(1,-1,-1) +
          2*q(1,-1,0) + q(1,-1,1) + 2*q(1,0,-1) + 4*q(1,0,0) +
          2*q(1,0,1) + q(1,1,-1) + 2*q(1,1,0) + q(1,1,1))/h[0];
          pn[i][1] = (1.0/32.0)*(-q(-1,-1,-1) - 2*q(-1,-1,0) -
          q(-1,-1,1) + q(-1,1,-1) + 2*q(-1,1,0) + q(-1,1,1) -
          2*q(0,-1,-1) - 4*q(0,-1,0) - 2*q(0,-1,1) + 2*q(0,1,-1) +
          4*q(0,1,0) + 2*q(0,1,1) - q(1,-1,-1) - 2*q(1,-1,0) -
          q(1,-1,1) + q(1,1,-1) + 2*q(1,1,0) + q(1,1,1))/h[1];
          pn[i][2] = (1.0/32.0)*(-q(-1,-1,-1) + q(-1,-1,1) -
          2*q(-1,0,-1) + 2*q(-1,0,1) - q(-1,1,-1) + q(-1,1,1) -
          2*q(0,-1,-1) + 2*q(0,-1,1) - 4*q(0,0,-1) + 4*q(0,0,1) -
          2*q(0,1,-1) + 2*q(0,1,1) - q(1,-1,-1) + q(1,-1,1) -
          2*q(1,0,-1) + 2*q(1,0,1) - q(1,1,-1) + q(1,1,1))/h[2];

          pn[i] /= -pn[i].norm1();
        }
      }
    }
  }
  // Computes normal and curvature from height functions.
  // fcu: volume fraction
  // fci: interface mask (1: contains interface)
  // edim: effective dimension
  // ow: 1: force overwrite, 0: update only if gives steeper profile
  // Output: modified in cells with fci=1, resized to m
  // fcn: normal, antigradient of fcu, if gives steeper profile or ow=1 [s] 
  static void CalcNormalHeight(
      M& m, const FieldCell<Scal>& fcu, const FieldCell<bool>& fci,
      size_t edim, bool ow, FieldCell<Vect>& fcn) {
    using MIdx = typename M::MIdx;
    using Dir = typename M::Dir;
    auto& bc = m.GetIndexCells();

    fcn.Reinit(m); // XXX

    for (auto c : m.SuCells()) {
      if (!fci[c]) {
        continue;
      }
      Vect bn; // best normal
      Scal bhx = 0., bhy = 0.; // best first derivative
      Dir bd = Dir::i;  // best direction
      bool fst = true; // first
      std::vector<Dir> dd; // direction of plane normal
      if (edim == 2) {
        dd = {Dir::i, Dir::j};
      } else {
        dd = {Dir::i, Dir::j, Dir::k};
      }
      for (Dir dn : dd) {
        // directions of plane tangents ([d]irection [t]angents)
        Dir dtx((size_t(dn) + 1) % dim); 
        Dir dty((size_t(dn) + 2) % dim); 

        MIdx w = bc.GetMIdx(c);

        // offset in normal direction
        MIdx on = MIdx(dn);
        // offset in dtx,dty
        MIdx otx = MIdx(dtx);
        MIdx oty = MIdx(dty);
        // mesh step
        const Scal lx = m.GetCellSize()[size_t(dtx)];
        const Scal ly = m.GetCellSize()[size_t(dty)];
        const Scal ln = m.GetCellSize()[size_t(dn)];

        // Evaluates height function
        // o: offset from w
        auto hh = [&](MIdx o) -> Scal {
          return 
            (fcu[bc.GetIdx(w + o - on)] + 
            fcu[bc.GetIdx(w + o)] + 
            fcu[bc.GetIdx(w + o + on)]) * ln;
        };

        // height function
        const Scal hxm = hh(-otx);
        const Scal hxp = hh(otx);
        const Scal hym = hh(-oty);
        const Scal hyp = hh(oty);

        // first derivative (slope)
        Scal hx = (hxp - hxm) / (2. * lx);  // centered
        Scal hy = (hyp - hym) / (2. * ly); 
        // sign: +1 if u increases in dn
        Scal sg = 
            (fcu[bc.GetIdx(w + on)] - fcu[bc.GetIdx(w - on)] > 0. ? 1. : -1.);
        // outer normal
        Vect n;
        n[size_t(dtx)] = -hx;
        n[size_t(dty)] = -hy;
        n[size_t(dn)] = -sg;
        // select best with minimal slope
        using std::abs;
        if (fst || abs(hx) + abs(hy) < abs(bhx) + abs(bhy)) {
          bn = n;
          bhx = hx;
          bhy = hy;
          bd = dn;
          fst = false;
        } 
      }
      bn /= bn.norm1(); // normalize

      // update if ow=1 or gives steeper profile in plane dn
      if (ow || std::abs(bn[size_t(bd)]) < std::abs(fcn[c][size_t(bd)])) {
        fcn[c] = bn;
      }
    }
  }
  // Computes heights.
  // S: the number of stages for stencils, column size is [-S*2,S*2]
  // fcu: volume fraction [a]
  // fcud2: volume fraction difference double (xpp-xmm, ypp-ymm, zpp-zmm) [a]
  // fcud4: volume fraction difference triple (xp4-xm4, ...) [a]
  // Output:
  // fch: fch[c][d] is absolute position of the interface
  // from a column in direction d starting from an interfacial cell c
  // otherwise, NaN
  template <size_t S>
  static void CalcHeight(
      M& m, const FieldCell<Scal>& fcu,
      const std::array<const FieldCell<Vect>*, S> vfcud,
      size_t edim, FieldCell<Vect>& fch) {
    auto I = [](Scal a) { return a > 0 && a < 1; }; // interface

    fch.Reinit(m, GetNan<Vect>());

    for (auto c : m.Cells()) {
      if (!I(fcu[c])) {
        continue;
      }
      for (size_t d = 0; d < edim; ++d) {
        IdxCell cm = m.GetNeighbourCell(c, 2 * d);
        IdxCell cmm = m.GetNeighbourCell(cm, 2 * d);
        IdxCell cp = m.GetNeighbourCell(c, 2 * d + 1);
        IdxCell cpp = m.GetNeighbourCell(cp, 2 * d + 1);

        const size_t si = (S + 1) * 4 + 1;
        const size_t sih = (S + 1) * 2;

        std::array<Scal, si> uu;

        uu[sih] = fcu[c];
        uu[sih - 1] = fcu[cm];
        uu[sih + 1] = fcu[cp];
        uu[sih - 2] = fcu[cmm];
        uu[sih + 2] = fcu[cpp];

        for (size_t s = 0; s < S; ++s) {
          const size_t q = (s + 1) * 2; // difference field half-interval
          const size_t ia = q + 1;
          const size_t ib = q + 2;
          const FieldCell<Vect>& fcud = *vfcud[s];

          uu[sih - ia] = uu[sih - ia + 2*q] - fcud[cm][d];
          uu[sih + ia] = uu[sih + ia - 2*q] + fcud[cp][d];
          uu[sih - ib] = uu[sih - ib + 2*q] - fcud[cmm][d];
          uu[sih + ib] = uu[sih + ib - 2*q] + fcud[cpp][d];
        }

        // |cm6|cm5|cm4|cm3|cmm| cm| c |cp |cpp|cp3|cp4|cp5|cp6|
        // |   |   |   |   | * |   | c |   | * |   |   |   |   |
        // |   |   | * |   |   |   | c |   |   |   | * |   |   |

        Scal s = UHeight<Scal>::Good(uu);
        fch[c][d] = m.GetCenter(c)[d] + s * m.GetCellSize()[d];
      }
    }
  }
  // Computes curvature from height functions.
  // fcu: volume fraction
  // fcn: normal, antigradient of fcu
  // edim: effective dimension
  // Output: modified in cells with fci=1, resized to m
  // fck: curvature [i] 
  static void CalcCurvHeight(
      M& m, const FieldCell<Scal>& fcu, const FieldCell<Vect>& fch,
      const FieldCell<Vect>& fcn, size_t edim, FieldCell<Scal>& fck) {
    using MIdx = typename M::MIdx;
    using Dir = typename M::Dir;
    auto& bc = m.GetIndexCells();

    auto I = [](Scal a) { return a > 0 && a < 1; }; // interface

    (void) edim;

    fck.Reinit(m, GetNan<Scal>());

    for (auto c : m.Cells()) {
      if (!I(fcu[c])) {
        continue;
      }

      size_t di = fcn[c].abs().argmax(); // best direction index
      Dir dn(di); // best direction
      // directions of plane tangents ([d]irection [t]angents)
      Dir dtx((size_t(dn) + 1) % dim); 
      Dir dty((size_t(dn) + 2) % dim); 

      MIdx w = bc.GetMIdx(c);

      // offset in normal direction
      MIdx on = MIdx(dn);
      // offset in dtx,dty
      MIdx otx = MIdx(dtx);
      MIdx oty = MIdx(dty);
      // mesh step
      const Scal lx = m.GetCellSize()[size_t(dtx)];
      const Scal ly = m.GetCellSize()[size_t(dty)];
      const Scal ln = m.GetCellSize()[size_t(dn)];

      // Evaluates height function from nearest interface
      // o: offset from w
      auto hh = [&w,&fch,&di,&bc,&on,&fcu](MIdx o) -> Scal {
        auto I = [](Scal a) { return a > 0 && a < 1; }; // true if interface
        const int si = 5; // stencil size
        const int sih = si / 2;
        int i = sih; // closest interface to center
        while (i < si) {
          auto cn = bc.GetIdx(w + o + on * (i - sih));
          if (I(fcu[cn])) {
            return fch[cn][di];
          }
          if (i > sih) {
            i = si - i - 1;
          } else {
            i = si - i;
          }
        }
        return GetNan<Scal>();
      };

      // height function
      const Scal hcc = hh(MIdx(0));
      const Scal hmc = hh(-otx);
      const Scal hpc = hh(otx);
      const Scal hcm = hh(-oty);
      const Scal hcp = hh(oty);
      // corners: hxy
      const Scal hmm = hh(-otx - oty);
      const Scal hmp = hh(-otx + oty);
      const Scal hpm = hh(otx - oty);
      const Scal hpp = hh(otx + oty);

      // first derivative (slope)
      Scal hx = (hpc - hmc) / (2. * lx);  // centered
      Scal hy = (hcp - hcm) / (2. * ly);
      // second derivative
      const Scal fl = 0.2; // filter factor (Basilisk: fl=0.2)
      Scal hxx = (
          (hpm - 2. * hcm + hmm) * fl +
          (hpc - 2. * hcc + hmc) +
          (hpp - 2. * hcp + hmp) * fl) / ((1 + 2 * fl) * lx * lx);
      Scal hyy = (
          (hmp - 2. * hmc + hmm) * fl +
          (hcp - 2. * hcc + hcm) +
          (hpp - 2. * hpc + hpm) * fl) / ((1 + 2 * fl) * ly * ly);
      Scal hxy = ((hpp - hmp) - (hpm - hmm)) / (4. * lx * ly);
      // curvature
      Scal k = (2. * hx * hy * hxy 
          -(sqr(hy) + 1.) * hxx -(sqr(hx) + 1.) * hyy) / 
          std::pow(sqr(hx) + sqr(hy) + 1., 3. / 2.);

      if (fcn[c][di] < 0) {
        k *= -1;
      }

      // curvature
      fck[c] = k;
    }
  }
  template <class Q>
  static void F(Q& qn, Scal& gxm, Scal& gxp, Scal& gym, Scal& gyp, Scal& sg,
                Scal& gc, Scal& gmm, Scal& gmp, Scal& gpm, Scal& gpp) {
    auto g = [&qn](int dx, int dy) {
      return qn(dx,dy,1) + qn(dx,dy,0) + qn(dx,dy,-1);
    };
    gxm = g(-1,0);
    gxp = g(1,0);
    gym = g(0,-1);
    gyp = g(0,1);
    sg = (qn(0,0,1) - qn(0,0,-1) > 0. ? 1. : -1);
    gc = g(0,0);
    gmm = g(-1,-1);
    gmp = g(-1,1);
    gpm = g(1,-1);
    gpp = g(1,1);
  }
  // CalcNormalHeight: optimized implementation
  static void CalcNormalHeight1(
      M& m, const FieldCell<Scal>& fcu, const FieldCell<bool>& fci,
      size_t edim, bool ow, FieldCell<Vect>& fcn) {
    using MIdx = typename M::MIdx;
    auto ic = m.GetIndexCells();
    auto bc = m.GetSuBlockCells();
    MIdx s = ic.GetSize();
    const size_t nx = s[0];
    const size_t ny = s[1];
    // offset
    const size_t fx = 1;
    const size_t fy = nx;
    const size_t fz = ny * nx;

    // index range
    const MIdx wb = bc.GetBegin() - ic.GetBegin();
    const MIdx we = bc.GetEnd() - ic.GetBegin();
    const size_t xb = wb[0], yb = wb[1], zb = wb[2];
    const size_t xe = we[0], ye = we[1], ze = we[2];

    fcn.Reinit(m);

    const Vect h = m.GetCellSize();

    const Scal* pu = fcu.data();
    Vect* pn = fcn.data();
    const bool* pi = fci.data();
    for (size_t z = zb; z < ze; ++z) {
      for (size_t y = yb; y < ye; ++y) {
        for (size_t x = xb; x < xe; ++x) {
          size_t i = (z * ny + y) * nx + x;
          if (!pi[i]) {
            continue;
          }
          auto q = [i,fy,fz,pu](int dx, int dy, int dz) {
            size_t ii = i + dx * fx + dy * fy + dz * fz;
            return pu[ii];
          };
          auto qx = [&q](int dx, int dy, int dz) {
            return q(dz, dx, dy);
          };
          auto qy = [&q](int dx, int dy, int dz) {
            return q(dy, dz, dx);
          };
          auto qz = [&q](int dx, int dy, int dz) {
            return q(dx, dy, dz);
          };

          Vect bn(0); // best normal
          size_t bd = 0;  // best direction
          std::vector<size_t> dd; // direction of plane normal
          if (edim == 2) {
            dd = {0, 1};
          } else {
            dd = {0, 1, 2};
          }
          for (size_t dn : dd) {
            size_t dx = (dn + 1) % dim;
            size_t dy = (dn + 2) % dim;

            // height function
            Scal gxm, gxp, gym, gyp;
            Scal gc, gmm, gmp, gpm, gpp;
            // sign: +1 if u increases in dn
            Scal sg;
            switch (dn) {
              case 0: { // dx:1 , dy:2
                F(qx, gxm, gxp, gym, gyp, sg, gc, gmm, gmp, gpm, gpp); break;
              }
              case 1: { // dx:2 , dy:0
                F(qy, gxm, gxp, gym, gyp, sg, gc, gmm, gmp, gpm, gpp); break;
              }
              default: { // dx:0 , dy:1
                F(qz, gxm, gxp, gym, gyp, sg, gc, gmm, gmp, gpm, gpp); break;
              }
            }

            Scal ln = h[dn];
            Scal lx = h[dx];
            Scal ly = h[dy];
            // first derivative (slope)
            Scal gx = (gxp - gxm) * ln / (2. * lx);  // centered
            Scal gy = (gyp - gym) * ln / (2. * ly);
            // outer normal
            Vect n;
            n[dx] = -gx;
            n[dy] = -gy;
            n[dn] = -sg;
            n /= n.norm1();
            // select best with minimal slope
            if (std::abs(n[dn]) > std::abs(bn[bd])) {
              bn = n;
              bd = dn;
            }
          }

          // update if ow=1 or gives steeper profile in plane dn
          if (ow || std::abs(bn[bd]) < std::abs(pn[i][bd])) {
            pn[i] = bn;
          }
        }
      }
    }
  }
  // Computes normal by combined Young's scheme and height-functions
  // fcu: volume fraction
  // fci: interface mask (1: contains interface)
  // edim: effective dimension
  // Output: set to NaN if fci=0
  // fcn: normal with norm1()=1, antigradient of fcu [s] 
  static void CalcNormal(
      M& m, const FieldCell<Scal>& fcu, const FieldCell<bool>& fci,
      size_t edim, FieldCell<Vect>& fcn) {
    fcn.Reinit(m, Vect(GetNan<Scal>()));
    FieldCell<char> fcd(m);
    CalcNormalYoung1(m,fcu, fci, fcn);
    CalcNormalHeight1(m, fcu, fci, edim, false, fcn);
  }
};

template <class M_>
void UNormal<M_>::CalcNormal(
    M& m, const FieldCell<Scal>& fcu, const FieldCell<bool>& fci,
    size_t edim, FieldCell<Vect>& fcn) {
  Imp::CalcNormal(m, fcu, fci, edim, fcn);
}

template <class M_>
void UNormal<M_>::CalcHeight(
      M& m, const FieldCell<Scal>& fcu,
      const FieldCell<Vect>& fcdu2,
      size_t edim, FieldCell<Vect>& fch) {
  std::array<const FieldCell<Vect>*, 1> vfcdu = {&fcdu2};
  Imp::CalcHeight(m, fcu, vfcdu, edim, fch);
}

template <class M_>
void UNormal<M_>::CalcHeight(
      M& m, const FieldCell<Scal>& fcu,
      const FieldCell<Vect>& fcdu2,
      const FieldCell<Vect>& fcdu4,
      size_t edim, FieldCell<Vect>& fch) {
  std::array<const FieldCell<Vect>*, 2> vfcdu = {&fcdu2, &fcdu4};
  Imp::CalcHeight(m, fcu, vfcdu, edim, fch);
}

template <class M_>
void UNormal<M_>::CalcHeight(
      M& m, const FieldCell<Scal>& fcu,
      const FieldCell<Vect>& fcdu2,
      const FieldCell<Vect>& fcdu4,
      const FieldCell<Vect>& fcdu6,
      size_t edim, FieldCell<Vect>& fch) {
  std::array<const FieldCell<Vect>*, 3> vfcdu = {&fcdu2, &fcdu4, &fcdu6};
  Imp::CalcHeight(m, fcu, vfcdu, edim, fch);
}

template <class M_>
void UNormal<M_>::CalcCurvHeight(
      M& m, const FieldCell<Scal>& fcu, const FieldCell<Vect>& fch,
      const FieldCell<Vect>& fcn, size_t edim, FieldCell<Scal>& fck) {
  Imp::CalcCurvHeight(m, fcu, fch, fcn, edim, fck);
}


template <class M_>
void UNormal<M_>::CalcNormalYoung(
    M& m, const FieldCell<Scal>& fcu, const FieldCell<bool>& fci,
    FieldCell<Vect>& fcn) {
  Imp::CalcNormalYoung1(m, fcu, fci, fcn);
}

} // namespace solver
