$(WRK)/aphros_c/git.obj: $(SRC)/aphros_c/git.cpp; $(CXX_RULE) $(SRC)/aphros_c/git.cpp
$(WRK)/aphros_c/main.obj: $(SRC)/aphros_c/main.cpp; $(CXX_RULE) $(SRC)/aphros_c/main.cpp
$(WRK)/aphros_c/parser.obj: $(SRC)/aphros_c/parser.cpp; $(CXX_RULE) $(SRC)/aphros_c/parser.cpp
$(WRK)/color/color.obj: $(SRC)/color/color.c; $(CC_RULE) $(SRC)/color/color.c
$(WRK)/distr/comm_manager.obj: $(SRC)/distr/comm_manager.cpp; $(CXX_RULE) $(SRC)/distr/comm_manager.cpp
$(WRK)/distr/distrbasic.obj: $(SRC)/distr/distrbasic.cpp; $(CXX_RULE) $(SRC)/distr/distrbasic.cpp
$(WRK)/distr/distr.obj: $(SRC)/distr/distr.cpp; $(CXX_RULE) $(SRC)/distr/distr.cpp
$(WRK)/distr/distrsolver.obj: $(SRC)/distr/distrsolver.cpp; $(CXX_RULE) $(SRC)/distr/distrsolver.cpp
$(WRK)/distr/local.obj: $(SRC)/distr/local.cpp; $(CXX_RULE) $(SRC)/distr/local.cpp
$(WRK)/distr/native.obj: $(SRC)/distr/native.cpp; $(CXX_RULE) $(SRC)/distr/native.cpp
$(WRK)/distr/report.obj: $(SRC)/distr/report.cpp; $(CXX_RULE) $(SRC)/distr/report.cpp
$(WRK)/dump/dump.obj: $(SRC)/dump/dump.cpp; $(CXX_RULE) $(SRC)/dump/dump.cpp
$(WRK)/dump/dumper.obj: $(SRC)/dump/dumper.cpp; $(CXX_RULE) $(SRC)/dump/dumper.cpp
$(WRK)/dump/hdf.obj: $(SRC)/dump/hdf.cpp; $(CXX_RULE) $(SRC)/dump/hdf.cpp
$(WRK)/dump/raw.obj: $(SRC)/dump/raw.cpp; $(CXX_RULE) $(SRC)/dump/raw.cpp
$(WRK)/dump/xmf.obj: $(SRC)/dump/xmf.cpp; $(CXX_RULE) $(SRC)/dump/xmf.cpp
$(WRK)/func/init_contang.obj: $(SRC)/func/init_contang.cpp; $(CXX_RULE) $(SRC)/func/init_contang.cpp
$(WRK)/func/init.obj: $(SRC)/func/init.cpp; $(CXX_RULE) $(SRC)/func/init.cpp
$(WRK)/func/init_vel.obj: $(SRC)/func/init_vel.cpp; $(CXX_RULE) $(SRC)/func/init_vel.cpp
$(WRK)/func/primlist.obj: $(SRC)/func/primlist.cpp; $(CXX_RULE) $(SRC)/func/primlist.cpp
$(WRK)/geom/mesh.obj: $(SRC)/geom/mesh.cpp; $(CXX_RULE) $(SRC)/geom/mesh.cpp
$(WRK)/inside/bbox.obj: $(SRC)/inside/bbox.c; $(CC_RULE) $(SRC)/inside/bbox.c
$(WRK)/inside/err.obj: $(SRC)/inside/err.c; $(CC_RULE) $(SRC)/inside/err.c
$(WRK)/inside/main.obj: $(SRC)/inside/main.c; $(CC_RULE) $(SRC)/inside/main.c
$(WRK)/inside/memory.obj: $(SRC)/inside/memory.c; $(CC_RULE) $(SRC)/inside/memory.c
$(WRK)/inside/off.obj: $(SRC)/inside/off.c; $(CC_RULE) $(SRC)/inside/off.c
$(WRK)/inside/ply.obj: $(SRC)/inside/ply.c; $(CC_RULE) $(SRC)/inside/ply.c
$(WRK)/inside/predicate.obj: $(SRC)/inside/predicate.c; $(CC_RULE) $(SRC)/inside/predicate.c
$(WRK)/inside/stl.obj: $(SRC)/inside/stl.c; $(CC_RULE) $(SRC)/inside/stl.c
$(WRK)/kernel/hydro.obj: $(SRC)/kernel/hydro.cpp; $(CXX_RULE) $(SRC)/kernel/hydro.cpp
$(WRK)/linear/linear.obj: $(SRC)/linear/linear.cpp; $(CXX_RULE) $(SRC)/linear/linear.cpp
$(WRK)/march/main.obj: $(SRC)/march/main.c; $(CC_RULE) $(SRC)/march/main.c
$(WRK)/parse/argparse.obj: $(SRC)/parse/argparse.cpp; $(CXX_RULE) $(SRC)/parse/argparse.cpp
$(WRK)/parse/codeblocks.obj: $(SRC)/parse/codeblocks.cpp; $(CXX_RULE) $(SRC)/parse/codeblocks.cpp
$(WRK)/parse/conf2py.obj: $(SRC)/parse/conf2py.cpp; $(CXX_RULE) $(SRC)/parse/conf2py.cpp
$(WRK)/parse/parser.obj: $(SRC)/parse/parser.cpp; $(CXX_RULE) $(SRC)/parse/parser.cpp
$(WRK)/parse/template.obj: $(SRC)/parse/template.cpp; $(CXX_RULE) $(SRC)/parse/template.cpp
$(WRK)/parse/vars.obj: $(SRC)/parse/vars.cpp; $(CXX_RULE) $(SRC)/parse/vars.cpp
$(WRK)/solver/approx_eb.obj: $(SRC)/solver/approx_eb.cpp; $(CXX_RULE) $(SRC)/solver/approx_eb.cpp
$(WRK)/solver/approx.obj: $(SRC)/solver/approx.cpp; $(CXX_RULE) $(SRC)/solver/approx.cpp
$(WRK)/solver/convdiffe.obj: $(SRC)/solver/convdiffe.cpp; $(CXX_RULE) $(SRC)/solver/convdiffe.cpp
$(WRK)/solver/convdiffi.obj: $(SRC)/solver/convdiffi.cpp; $(CXX_RULE) $(SRC)/solver/convdiffi.cpp
$(WRK)/solver/convdiffvg.obj: $(SRC)/solver/convdiffvg.cpp; $(CXX_RULE) $(SRC)/solver/convdiffvg.cpp
$(WRK)/solver/curv.obj: $(SRC)/solver/curv.cpp; $(CXX_RULE) $(SRC)/solver/curv.cpp
$(WRK)/solver/electro.obj: $(SRC)/solver/electro.cpp; $(CXX_RULE) $(SRC)/solver/electro.cpp
$(WRK)/solver/embed.obj: $(SRC)/solver/embed.cpp; $(CXX_RULE) $(SRC)/solver/embed.cpp
$(WRK)/solver/fluid_dummy.obj: $(SRC)/solver/fluid_dummy.cpp; $(CXX_RULE) $(SRC)/solver/fluid_dummy.cpp
$(WRK)/solver/normal.obj: $(SRC)/solver/normal.cpp; $(CXX_RULE) $(SRC)/solver/normal.cpp
$(WRK)/solver/particles.obj: $(SRC)/solver/particles.cpp; $(CXX_RULE) $(SRC)/solver/particles.cpp
$(WRK)/solver/partstrmeshm.obj: $(SRC)/solver/partstrmeshm.cpp; $(CXX_RULE) $(SRC)/solver/partstrmeshm.cpp
$(WRK)/solver/proj_eb.obj: $(SRC)/solver/proj_eb.cpp; $(CXX_RULE) $(SRC)/solver/proj_eb.cpp
$(WRK)/solver/proj.obj: $(SRC)/solver/proj.cpp; $(CXX_RULE) $(SRC)/solver/proj.cpp
$(WRK)/solver/simple.obj: $(SRC)/solver/simple.cpp; $(CXX_RULE) $(SRC)/solver/simple.cpp
$(WRK)/solver/solver.obj: $(SRC)/solver/solver.cpp; $(CXX_RULE) $(SRC)/solver/solver.cpp
$(WRK)/solver/tracer.obj: $(SRC)/solver/tracer.cpp; $(CXX_RULE) $(SRC)/solver/tracer.cpp
$(WRK)/solver/vofm.obj: $(SRC)/solver/vofm.cpp; $(CXX_RULE) $(SRC)/solver/vofm.cpp
$(WRK)/solver/vof.obj: $(SRC)/solver/vof.cpp; $(CXX_RULE) $(SRC)/solver/vof.cpp
$(WRK)/util/convdiff.obj: $(SRC)/util/convdiff.cpp; $(CXX_RULE) $(SRC)/util/convdiff.cpp
$(WRK)/util/distr.obj: $(SRC)/util/distr.cpp; $(CXX_RULE) $(SRC)/util/distr.cpp
$(WRK)/util/events.obj: $(SRC)/util/events.cpp; $(CXX_RULE) $(SRC)/util/events.cpp
$(WRK)/util/filesystem.obj: $(SRC)/util/filesystem.cpp; $(CXX_RULE) $(SRC)/util/filesystem.cpp
$(WRK)/util/fixed_allocator.obj: $(SRC)/util/fixed_allocator.cpp; $(CXX_RULE) $(SRC)/util/fixed_allocator.cpp
$(WRK)/util/fluid.obj: $(SRC)/util/fluid.cpp; $(CXX_RULE) $(SRC)/util/fluid.cpp
$(WRK)/util/format.obj: $(SRC)/util/format.cpp; $(CXX_RULE) $(SRC)/util/format.cpp
$(WRK)/util/gitgen.obj: $(SRC)/util/gitgen.cpp; $(CXX_RULE) $(SRC)/util/gitgen.cpp
$(WRK)/util/git.obj: $(SRC)/util/git.cpp; $(CXX_RULE) $(SRC)/util/git.cpp
$(WRK)/util/histogram.obj: $(SRC)/util/histogram.cpp; $(CXX_RULE) $(SRC)/util/histogram.cpp
$(WRK)/util/hydro.obj: $(SRC)/util/hydro.cpp; $(CXX_RULE) $(SRC)/util/hydro.cpp
$(WRK)/util/linear.obj: $(SRC)/util/linear.cpp; $(CXX_RULE) $(SRC)/util/linear.cpp
$(WRK)/util/logger.obj: $(SRC)/util/logger.cpp; $(CXX_RULE) $(SRC)/util/logger.cpp
$(WRK)/util/mpi.obj: $(SRC)/util/mpi.cpp; $(CXX_RULE) $(SRC)/util/mpi.cpp
$(WRK)/util/posthook_default.obj: $(SRC)/util/posthook_default.cpp; $(CXX_RULE) $(SRC)/util/posthook_default.cpp
$(WRK)/util/subcomm_dummy.obj: $(SRC)/util/subcomm_dummy.cpp; $(CXX_RULE) $(SRC)/util/subcomm_dummy.cpp
$(WRK)/util/suspender.obj: $(SRC)/util/suspender.cpp; $(CXX_RULE) $(SRC)/util/suspender.cpp
$(WRK)/util/sysinfo.obj: $(SRC)/util/sysinfo.cpp; $(CXX_RULE) $(SRC)/util/sysinfo.cpp
$(WRK)/util/system.obj: $(SRC)/util/system.c; $(CC_RULE) $(SRC)/util/system.c
$(WRK)/util/timer.obj: $(SRC)/util/timer.cpp; $(CXX_RULE) $(SRC)/util/timer.cpp
$(WRK)/util/vof.obj: $(SRC)/util/vof.cpp; $(CXX_RULE) $(SRC)/util/vof.cpp
$(WRK)/young/main.obj: $(SRC)/young/main.cpp; $(CXX_RULE) $(SRC)/young/main.cpp
