
LOCAL_PATH := $(call my-dir)

LOCAL_MODULE    := ja
LOCAL_MODULE_FILENAME    := libj

# ndk r21 : OpenMP is now available as a dynamic library (and this is the new default behavior, so link with -static-openmp if you want to stick with the static runtime)
ifeq ($(TARGET_ARCH_ABI),arm64-v8a)
  LOCAL_CFLAGS := -DSLEEF=1 -DSLEEFQUAD=1 -DHAVE_NEON64=1 -fPIC -Os -fvisibility=hidden -fwrapv -Werror -Wno-implicit-const-int-float-conversion -Wno-unknown-warning-option -Wno-braced-scalar-init -Wno-string-plus-int -Wno-empty-body -Wno-parentheses -Wno-pointer-sign -Wno-pointer-to-int-cast -Wno-incompatible-function-pointer-types -Wno-logical-op-parentheses -Wno-unused-value -Wno-null-dereference -Wno-type-limits -Wno-pass-failed -D_FORTIFY_SOURCE=2 -Werror=fortify-source -fno-strict-aliasing -march=armv8-a+crc+crypto -Wno-sign-compare -Wno-deprecated-non-prototype -mno-outline-atomics
  LOCAL_ARM_NEON := true
  LOCAL_CFLAGS += -fopenmp
  LOCAL_LDFLAGS += -fopenmp -static-openmp
endif
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
  LOCAL_CFLAGS := -DSLEEF=0 -DSLEEFQUAD=1 -DHAVE_NEON32=1 -DARMEABI_V7A -fPIC -Os -fvisibility=hidden -fwrapv -Werror -Wno-implicit-const-int-float-conversion -Wno-unknown-warning-option -Wno-braced-scalar-init -Wno-string-plus-int -Wno-empty-body -Wno-parentheses -Wno-pointer-sign -Wno-pointer-to-int-cast -Wno-incompatible-function-pointer-types -Wno-logical-op-parentheses -Wno-unused-value -Wno-null-dereference -Wno-type-limits -Wno-pass-failed -D_FORTIFY_SOURCE=2 -Werror=fortify-source -fno-strict-aliasing -mfloat-abi=softfp -march=armv7-a -Wno-sign-compare -Wno-deprecated-non-prototype
  LOCAL_ARM_MODE := arm
  LOCAL_ARM_NEON := true
  LOCAL_CFLAGS += -fopenmp
  LOCAL_LDFLAGS += -fopenmp -static-openmp
endif
LOCAL_LDLIBS := -ldl -llog
LOCAL_LDFLAGS += -Wl,-z,noexecstack

LOCAL_SRC_FILES := a.c ab.c af.c ai.c am.c am1.c amn.c ao.c ap.c ar.c as.c au.c c.c ca.c cc.c cd.c cf.c cg.c ch.c cip.c cl.c cp.c cpdtsp.c cpuinfo.c cr.c crs.c \
	ct.c cu.c cv.c cx.c d.c dc.c dss.c dstop.c dsusp.c dtoa.c f.c f2.c i.c io.c j.c jdlllic.c k.c m.c mbx.c p.c pv.c px.c r.c rl.c rt.c s.c sc.c sl.c \
	sn.c t.c u.c v.c v0.c v1.c v2.c va1.c va2.c va2s.c vamultsp.c vb.c vbang.c vbit.c vcant.c vchar.c vcat.c vcatsp.c vcomp.c vcompsc.c vd.c vdx.c ve.c \
	vf.c vfft.c vfrom.c  vfromsp.c vg.c vgauss.c vgcomp.c vgranking.c vgsort.c vgsp.c vi.c viix.c visp.c vm.c vo.c vp.c vq.c vrand.c vrep.c vs.c vsb.c \
	vt.c vu.c vx.c vz.c w.c wc.c wn.c ws.c x.c x15.c xa.c xb.c xc.c xcrc.c xd.c xf.c xfmt.c xh.c xi.c xl.c xo.c xs.c xt.c xu.c \

include $(BUILD_STATIC_LIBRARY)
