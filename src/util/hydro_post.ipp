// Created by Petr Karnakov on 20.03.2021
// Copyright 2021 ETH Zurich

#include <array>
#include <iomanip>
#include <iostream>
#include <string>

#include "dump/dumper.h"
#include "parse/util.h"

#include "hydro_post.h"

template <class M>
struct HydroPost<M>::Imp {
  template <class MEB>
  static FieldCell<Scal> GetVortScal(
      const FieldCell<Vect>& fcvel, const MapEmbed<BCond<Vect>>& me_vel,
      MEB& eb) {
    constexpr auto dim = M::dim;
    auto& m = eb.GetMesh();
    using UEB = UEmbed<M>;

    std::array<FieldCell<Vect>, dim> grad;
    for (size_t d = 0; d < dim; ++d) {
      grad[d].Reinit(m, Vect(0));
      const auto mebc = GetScalarCond(me_vel, d, m);
      const FieldCell<Scal> fcu = GetComponent(fcvel, d);
      const FieldFace<Scal> ffg = UEB::Gradient(fcu, mebc, m);
      grad[d] = UEB::AverageGradient(ffg, m);
    }

    FieldCell<Scal> res(m, 0);
    for (auto c : m.Cells()) {
      res[c] = grad[1][c][0] - grad[0][c][1];
    }
    return res;
  }

  static FieldCell<Scal> GetField(
      const Hydro<M>* hydro, std::string name, const M& m) {
    if (name == "p" || name == "pressure") {
      return hydro->fs_->GetPressure();
    }
    if (name == "omz" || name == "vorticity") {
      if (hydro->eb_) {
        return GetVortScal(
            hydro->fs_->GetVelocity(), hydro->fs_->GetVelocityCond(),
            *hydro->eb_);
      }
      return GetVortScal(
          hydro->fs_->GetVelocity(), hydro->fs_->GetVelocityCond(), m);
    }
    if (name == "ebvf" || name == "embed fraction") {
      FieldCell<Scal> fc(m, 1);
      if (hydro->eb_) {
        auto& eb = *hydro->eb_;
        for (auto c : m.SuCells()) {
          fc[c] = eb.GetVolumeFraction(c);
        }
      }
      return fc;
    }
    if (name == "vf" || name == "volume fraction") {
      return hydro->as_->GetField();
    }
    if (name == "vmagn" || name == "velocity magnitude") {
      FieldCell<Scal> fc(m, 0);
      for (auto c : m.SuCells()) {
        fc[c] = hydro->fs_->GetVelocity()[c].norm();
      }
      return fc;
    }
    if (name == "vx" || name == "velocity x") {
      FieldCell<Scal> fc(m, 0);
      for (auto c : m.SuCells()) {
        fc[c] = hydro->fs_->GetVelocity()[c][0];
      }
      return fc;
    }
    if (name == "vy" || name == "velocity y") {
      FieldCell<Scal> fc(m, 0);
      for (auto c : m.SuCells()) {
        fc[c] = hydro->fs_->GetVelocity()[c][1];
      }
      return fc;
    }
    if (m.IsRoot()) {
      std::cerr << "Unknown field '" + name + "'\n";
    }
    return FieldCell<Scal>(m, 0);
  };
  static void DumpFields(Hydro<M>* hydro, M& m) {
    auto sem = m.GetSem("dumpfields");
    struct {
      Multi<FieldCell<Vect>> fcim;
      FieldCell<Scal> fc_cellcond;
      FieldCell<Scal> fcdiv; // divergence of velocity
      FieldCell<Scal> fcdis; // energy dissipation
      FieldCell<Scal> fc_ebvf; // embedded boundaries volume fraction
      FieldCell<Scal> fc_tracer_sum; // sum of tracer_ fields starting from 1
      FieldCell<Vect> fc_flux; // mixture flux
    } * ctx(sem);
    auto& t = *ctx;
    const auto& var = hydro->var;
    if (sem("dump")) {
      if (m.IsRoot()) {
        hydro->dumper_.Report(std::cerr);
      }

      auto dl = GetWords(var.String["dumplist"]);

      auto dump = [&dl, &m](const FieldCell<Scal>& fc, std::string name) {
        if (dl.count(name)) {
          m.Dump(&fc, name);
        }
      };
      auto dumpv = [&dl, &m](
                       const FieldCell<Vect>& fc, size_t i, std::string name) {
        if (dl.count(name)) {
          m.Dump(&fc, i, name);
        }
      };

      auto& fcv = hydro->fs_->GetVelocity();
      dumpv(fcv, 0, "vx");
      dumpv(fcv, 1, "vy");
      dumpv(fcv, 2, "vz");
      dump(hydro->fs_->GetPressure(), "p");
      dump(hydro->as_->GetField(), "vf");
      dump(hydro->fc_rho_, "rho");
      dump(hydro->fc_mu_, "mu");
      dump(hydro->fc_sig_, "sig");
      dump(hydro->fc_contang_, "contang");
      if (dl.count("cellcond")) {
        auto& fc = t.fc_cellcond;
        fc.Reinit(m, 0);
        for (auto& it : hydro->mc_velcond_) {
          fc[it.first] = 1;
        }
        m.Dump(&fc, "cellcond");
      }
      if (dl.count("omx") || dl.count("omy") || dl.count("omz") ||
          dl.count("omm") || dl.count("omcalc")) {
        hydro->CalcVort();
        dumpv(hydro->fcom_, 0, "omx");
        dumpv(hydro->fcom_, 1, "omy");
        dumpv(hydro->fcom_, 2, "omz");
        dump(hydro->fcomm_, "omm");
      }
      if (dl.count("fluxx") || dl.count("fluxy") || dl.count("fluxz")) {
        t.fc_flux.Reinit(m, Vect(0));
        auto& ffv = hydro->fs_->GetVolumeFlux();
        for (auto c : m.Cells()) {
          for (auto d : m.dirs) {
            t.fc_flux[c][d] = ffv[m.GetFace(c, IdxNci(d * 2))];
          }
        }
        dumpv(t.fc_flux, 0, "fluxx");
        dumpv(t.fc_flux, 1, "fluxy");
        dumpv(t.fc_flux, 2, "fluxz");
      }
      if (dl.count("dis") || dl.count("strain")) {
        hydro->fc_strain_ = hydro->CalcStrain(hydro->fs_->GetVelocity());
        if (dl.count("strain")) m.Dump(&hydro->fc_strain_, "strain");
        if (dl.count("dis")) {
          t.fcdis = hydro->fc_strain_;
          for (auto c : m.Cells()) {
            t.fcdis[c] *= 2. * hydro->fc_mu_[c];
          }
          m.Dump(&t.fcdis, "dis");
        }
      }
      if (dl.count("div")) {
        t.fcdiv = hydro->GetDiv();
        m.Dump(&t.fcdiv, "div");
      }
      using ASV = typename Hydro<M>::ASV;
      using ASVEB = typename Hydro<M>::ASVEB;
      using ASVM = typename Hydro<M>::ASVM;
      if (auto as = dynamic_cast<ASV*>(hydro->as_.get())) {
        dumpv(as->GetNormal(), 0, "nx");
        dumpv(as->GetNormal(), 1, "ny");
        dumpv(as->GetNormal(), 2, "nz");
        dump(as->GetColor(), "cls");
        dump(hydro->fck_[0], "k");
      }
      // TODO reuse ASV code
      if (auto as = dynamic_cast<ASVEB*>(hydro->as_.get())) {
        dumpv(as->GetNormal(), 0, "nx");
        dumpv(as->GetNormal(), 1, "ny");
        dumpv(as->GetNormal(), 2, "nz");
        dump(as->GetColor(), "cls");
        dump(hydro->fck_[0], "k");
      }
      if (auto as = dynamic_cast<ASVM*>(hydro->as_.get())) {
        for (auto l : hydro->layers) {
          auto sl = std::to_string(l);
          dump(*as->GetFieldM()[l], "vf" + sl);
          dump(*as->GetColor()[l], "cl" + sl);
          dumpv(*as->GetNormal()[l], 0, "nx" + sl);
          dumpv(*as->GetNormal()[l], 1, "ny" + sl);
          dumpv(*as->GetNormal()[l], 2, "nz" + sl);
          dump(hydro->fck_[l], "k" + sl);
        }

        // combined colors
        dump(as->GetColorSum(), "cls");

        // image vector
        t.fcim.resize(hydro->layers);
        t.fcim.Reinit(m);
        for (auto l : hydro->layers) {
          auto& fcim = *as->GetImage()[l];
          for (auto c : m.Cells()) {
            t.fcim[l][c] = Vect(fcim[c]);
          }
          auto sl = std::to_string(l);
          dumpv(t.fcim[l], 0, "imx" + sl);
          dumpv(t.fcim[l], 1, "imy" + sl);
          dumpv(t.fcim[l], 2, "imz" + sl);
        }
      }
      // TODO add ASVMEB

      if (hydro->eb_) {
        auto& eb = *hydro->eb_;
        if (dl.count("ebvf")) {
          auto& fc = t.fc_ebvf;
          fc.Reinit(m, 0);
          for (auto c : eb.Cells()) {
            fc[c] = eb.GetVolumeFraction(c);
          }
          m.Dump(&fc, "ebvf");
        }
      }

      if (auto& tracer = hydro->tracer_) {
        for (auto l : tracer->GetView().layers) {
          dump(tracer->GetVolumeFraction()[l], "tu" + std::to_string(l));
        }
        if (dl.count("tusum")) {
          t.fc_tracer_sum.Reinit(m, 0);
          for (auto l : tracer->GetView().layers) {
            if (l > 0) {
              const auto& fc = tracer->GetVolumeFraction()[l];
              for (auto c : m.Cells()) {
                t.fc_tracer_sum[c] += fc[c];
              }
            }
          }
          dump(t.fc_tracer_sum, "tusum");
        }
      }
      if (auto& electro = hydro->electro_) {
        dump(electro->GetPotential(), "elpot");
        dumpv(electro->GetCurrent(), 0, "elcurx");
        dumpv(electro->GetCurrent(), 1, "elcury");
        dumpv(electro->GetCurrent(), 2, "elcurz");
      }
    }
    if (var.Int["enable_advection"]) {
      if (var.Int["dumppoly"] && sem.Nested()) {
        hydro->as_->DumpInterface(
            GetDumpName("s", ".vtk", hydro->dumper_.GetN()));
      }
      if (var.Int["dumppolymarch"] && sem.Nested()) {
        hydro->as_->DumpInterfaceMarch(
            GetDumpName("sm", ".vtk", hydro->dumper_.GetN()));
      }
    }
    if (hydro->particles_ && var.Int["dump_particles"]) {
      const std::string path =
          GetDumpName("part", ".csv", hydro->dumper_.GetN());
      if (sem()) {
        if (m.IsRoot()) {
          std::cerr << std::fixed << std::setprecision(8) << "dump"
                    << " t=" << hydro->particles_->GetTime() << " to " << path
                    << std::endl;
        }
      }
      if (sem.Nested()) {
        hydro->particles_->DumpCsv(path);
      }
    }
    if (sem()) {
      if (m.IsRoot() && var.Int("create_dumpdone", false)) {
        std::ofstream(GetDumpName(".dumpdone", "", hydro->dumper_.GetN()));
      }
      // XXX: empty stage, otherwise ctx is destroyed before dump
    }
  }
};

template <class M>
auto HydroPost<M>::GetField(const Hydro<M>* hydro, std::string name, const M& m)
    -> FieldCell<Scal> {
  return Imp::GetField(hydro, name, m);
}

template <class M>
void HydroPost<M>::DumpFields(Hydro<M>* hydro, M& m) {
  Imp::DumpFields(hydro, m);
}
