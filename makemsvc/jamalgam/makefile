ODIR = ../bin/$(jplatform)/$(j64x)

COPTION =
ifeq (j64arm,$(j64x))
 TARGET_CPU = arm64
 COPTION := $(COPTION) /DSLEEF=1 /DSLEEFQUAD=1
else ifeq (j64avx512,$(j64x))
 TARGET_CPU = x64
 COPTION := $(COPTION) /DC_AVX2=1 /DC_AVX512=1 -DHAVE_AVX2=1 -march=skylake-avx512 -mno-vzeroupper
 COPTION := $(COPTION) /DSLEEF=1 /DSLEEFQUAD=1
else ifeq (j64avx2,$(j64x))
 TARGET_CPU = x64
 COPTION := $(COPTION) /DC_AVX2=1 -DHAVE_AVX2=1 -march=skylake -mno-vzeroupper
 COPTION := $(COPTION) /DSLEEF=1 /DSLEEFQUAD=1
else ifeq (j64,$(j64x))
 TARGET_CPU = x64
 COPTION := $(COPTION) /DSLEEF=0 /DSLEEFQUAD=1
else
 TARGET_CPU = x86
 COPTION := $(COPTION) /DSLEEF=1 /DSLEEFQUAD=1
endif

ifeq (1,$(USE_LINENOISE))
 READLINE = 1
endif
ifeq ($(OPTL),)
 OPTL := -O2
endif
ifeq ($(OPTLD),)
 OPTLD := -O2
endif
ifeq (3,$(_DEBUG))
 ifeq (-O0,$(OPTLD))
  COPTION := $(COPTION) /Od /Z7 /DDEBUG=1 /DOPTMO0
 else
  COPTION := $(COPTION) $(OPTLD) /Z7 /DDEBUG=1
 endif
 LDEBUG = /DEBUG
else
 ifeq (-O0,$(OPTL))
  COPTION := $(COPTION) /Od /DOPTMO0
 else
  COPTION := $(COPTION) $(OPTL)
 endif
endif
ifeq (1,$(USE_PYXES))
 COPTION := $(COPTION) /DPYXES=1
else
 COPTION := $(COPTION) /DPYXES=0
endif
ifeq (0,$(USE_EMU_AVX))
 COPTION := $(COPTION) /DEMU_AVX2=0
else
 COPTION := $(COPTION) /DEMU_AVX2=1
endif

ifeq (1,$(USE_OPENMP))
 COPTION := $(COPTION) -Xclang -fopenmp
# LOPENMP = libomp.lib
 ifeq (arm64,$(TARGET_CPU))
  LOPENMP = ../openmp/obj/windows/arm64/libomp.lib
 else ifeq (x64,$(TARGET_CPU))
  LOPENMP = ../openmp/obj/windows/x64/libomp.lib
 else
  LOPENMP = ../openmp/obj/windows/x86/libomp.lib
 endif
endif
ifeq (1,$(USE_PYXES))
 ifeq (arm64,$(TARGET_CPU))
  ifneq ($(PTHREADS4WSRC),)
   OBJS_PTHREADS4W = ../pthreads4w/src/pthread.o
  else
    PTHREADLIB = ../pthreads4w/arm64/pthreadVC3.lib
  endif
 else ifeq (x64,$(TARGET_CPU))
  ifneq ($(PTHREADS4WSRC),)
   OBJS_PTHREADS4W = ../pthreads4w/src/pthread.o
  else
    PTHREADLIB = ../pthreads4w/x64/pthreadVC3.lib
  endif
 else
  ifneq ($(PTHREADS4WSRC),)
   OBJS_PTHREADS4W = ../pthreads4w/src/pthread.o
  else
   PTHREADLIB = ../pthreads4w/x86/pthreadVC3.lib
  endif
 endif
endif

ifeq (arm64,$(TARGET_CPU))
# MPIRLIB = ../mpir/dll/arm64/mpir.lib
else ifeq (x64,$(TARGET_CPU))
# MPIRLIB = ../mpir/dll/x64/mpir.lib
else
# MPIRLIB = ../mpir/dll/x86/mpir.lib
endif

ifeq (1,$(NOSYNCHLIB))
 COPTION := $(COPTION) /DNOSYNCHLIB=1
endif
ifneq ($(_MEMAUDIT),)
 COPTION := $(COPTION) /DMEMAUDIT=$(_MEMAUDIT)
endif
ifneq ($(_ASSERT),)
 COPTION := $(COPTION) /D_ASSERT
endif
ifneq ($(_ASSERT2),)
 COPTION := $(COPTION) /D_ASSERT2
endif
ifneq ($(C_CRC32C),)
 COPTION := $(COPTION) /DC_CRC32C=$(C_CRC32C)
endif
ifneq ($(NOSTACKCHK),)
 COPTION := $(COPTION) /DNOSTACKCHK
endif
ifneq ($(_NAN),)
 COPTION := $(COPTION) /D_NAN
endif
ifneq ($(_NAMETRACK),)
 COPTION := $(COPTION) /DNAMETRACK=$(_NAMETRACK)
endif
ifneq ($(__MSYS__),)
 COPTION := $(COPTION) /D__MSYS__
endif
ifeq (1,$(NO_UNDEFINED))
 COPTION := $(COPTION) /DNO_UNDEFINED=1
endif
ifeq (1,$(USE_BOXEDSPARSE))
 COPTION := $(COPTION) /DBOXEDSPARSE
endif
ifeq (x64,$(TARGET_CPU))
 ifneq ($(_SSE4_2),)
  COPTION := $(COPTION) -msse4.1 -msse4.2
 endif
endif

ifeq (arm64,$(TARGET_CPU))
 CF1 = -target arm64-pc-windows-msvc -march=armv8-a+crc+crypto -fno-strict-aliasing -Wno-unknown-warning-option -Wsign-compare -Wno-int-conversion -Wno-incompatible-function-pointer-types -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-missing-field-initializers -Wno-null-pointer-arithmetic -Wno-char-subscripts -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor -Wno-deprecated-non-prototype -Wno-unused-variable -Wno-single-bit-bitfield-constant-conversion
 CFLAGS = $(COPTION) $(CINCLUDE) /DHAVE_NEON64=1 /DJAMALGAM /DC_NA=0 /WX /W3 /Zc:strictStrings /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT $(CF1)
 FLAGS = $(LDEBUG) /NOLOGO /STACK:0xc00000 /MACHINE:ARM64 /SUBSYSTEM:CONSOLE,5.02
 ASMFLAGS = /Cx
else ifeq (x64,$(TARGET_CPU))
 CF1 = -maes -msse3 -fno-strict-aliasing -Wno-unknown-warning-option -Wsign-compare -Wno-int-conversion -Wno-incompatible-function-pointer-types -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-missing-field-initializers -Wno-null-pointer-arithmetic -Wno-char-subscripts -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor -Wno-deprecated-non-prototype -Wno-unused-variable -Wno-single-bit-bitfield-constant-conversion
 CFLAGS = $(COPTION) $(CINCLUDE) /DJAMALGAM /DC_NA=0 /WX /W3 /Zc:strictStrings /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT $(CF1)
 FLAGS = $(LDEBUG) /NOLOGO /STACK:0xc00000 /MACHINE:X64 /SUBSYSTEM:CONSOLE,5.02
 ASMFLAGS = /Cx
 NASM_FLAGS = -fwin64
else
 CF1 = -m32 -maes -msse2 -fno-strict-aliasing -Wno-unknown-warning-option -Wsign-compare -Wno-int-conversion -Wno-incompatible-function-pointer-types -Wno-parentheses -Wno-unused-variable -Wno-pointer-sign -Wno-missing-braces -Wno-missing-field-initializers -Wno-null-pointer-arithmetic -Wno-char-subscripts -Wno-pointer-to-int-cast -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-function -Wno-empty-body -Wno-sometimes-uninitialized -Wno-string-plus-int -Wno-unknown-pragmas -Wno-delete-non-abstract-non-virtual-dtor -Wno-deprecated-non-prototype -Wno-unused-variable -Wno-single-bit-bitfield-constant-conversion
 CFLAGS = $(COPTION) $(CINCLUDE) /DJAMALGAM /WX /W3 /Zc:strictStrings /D_USING_V110_SDK71_ /D_CRT_SECURE_NO_WARNINGS /MT $(CF1)
 FLAGS = $(LDEBUG) psapi.lib /NOLOGO /STACK:0xc00000 /MACHINE:X86 /SUBSYSTEM:CONSOLE,5.01
 ASMFLAGS = /Cx /coff
 NASM_FLAGS = -fwin32
endif

.SUFFIXES: .o

OBJS = \
 jconsole.o \
 jeload.o \
 vi.o \
 vcompsc.o \
 a.o \
 ab.o \
 af.o \
 ai.o \
 am.o \
 am1.o \
 amn.o \
 ao.o \
 ap.o \
 ar.o \
 as.o \
 au.o \
 c.o \
 ca.o \
 cc.o \
 cd.o \
 cf.o \
 cg.o \
 ch.o \
 cip.o \
 cl.o \
 cp.o \
 cpdtsp.o \
 cpuinfo.o \
 cr.o \
 crs.o \
 ct.o \
 cu.o \
 cv.o \
 cx.o \
 d.o \
 dc.o \
 dss.o \
 dstop.o \
 dsusp.o \
 dtoa.o \
 f.o \
 f2.o \
 i.o \
 io.o \
 j.o \
 jdlllic.o \
 k.o \
 m.o \
 mbx.o \
 p.o \
 pv.o \
 px.o \
 r.o \
 rl.o \
 rt.o \
 s.o \
 sc.o \
 sl.o \
 sn.o \
 t.o \
 u.o \
 v.o \
 v0.o \
 v1.o \
 v2.o \
 va1.o \
 va2.o \
 va2s.o \
 vamultsp.o \
 vb.o \
 vbang.o \
 vbit.o \
 vcant.o \
 vcat.o \
 vcatsp.o \
 vchar.o \
 vcomp.o \
 vd.o \
 vdx.o \
 ve.o \
 vf.o \
 vfft.o \
 vfrom.o \
 vfromsp.o \
 vg.o \
 vgauss.o \
 vgcomp.o \
 vgranking.o \
 vgsort.o \
 vgsp.o \
 viix.o \
 visp.o \
 vm.o \
 vo.o \
 vp.o \
 vq.o \
 vrand.o \
 vrep.o \
 vs.o \
 vsb.o \
 vt.o \
 vu.o \
 vx.o \
 vz.o \
 w.o \
 wc.o \
 wn.o \
 ws.o \
 x.o \
 x15.o \
 xa.o \
 xb.o \
 xc.o \
 xcrc.o \
 xd.o \
 xf.o \
 xfmt.o \
 xh.o \
 xi.o \
 xl.o \
 xo.o \
 xs.o \
 xt.o \
 xu.o \
 ../dllsrc/jdll.o \
 ../dllsrc/jdllcomx.o


ifdef READLINE
 OBJSLN = linenoise.o
 CFLAGSLN = -DREADLINE -DUSE_LINENOISE
endif

DEF = ../dllsrc/jdll3.def
RCS = ../makevs/jconsole/jconsole.res

ifndef LD
 LD = lld-link
endif

ifndef NASM
 NASM = nasm
endif

%.o : %.c
	$(CC) $(CFLAGS) $(CFLAGS1) /Fo$@ /c $<

%.o : %.cpp
	$(CC) $(CFLAGS) /Fo$@ /c $<

%.o : %.asm
	$(NASM) $(NASM_FLAGS) -o $@ $<

%.res : %.rc
	$(RC) /L 0x409 /D_USING_V110_SDK71_ -R -FO$@ $<

TARGET = jamalgam.exe

ifeq (1,$(NOSYNCHLIB))
 SYNCLIB =
else
 SYNCLIB = synchronization.lib
endif

$(ODIR)/$(TARGET) : $(OBJS) $(OBJSLN) $(SRC_ASM) $(OBJS_ASM) $(OBJS_SIMDUTF8) $(OBJS_PTHREADS4W) $(ASM) $(PTHREADLIB) $(RCS)
	$(LD) /NOLOGO /DEF:$(DEF) $(FLAGS) /out:$@ $(OBJS) $(OBJSLN) $(SRC_ASM) $(OBJS_ASM) $(OBJS_SIMDUTF8) $(OBJS_PTHREADS4W) $(ASM) $(PTHREADLIB) $(RCS) $(MPIRLIB) ole32.lib advapi32.lib oleaut32.lib onecore.lib $(SYNCLIB) $(LOPENMP)

all : $(ODIR)/$(TARGET)

noclean :
	#

clean :
	rm -f $(ODIR)/*.lib $(ODIR)/*.exp $(ODIR)/*.ilk $(ODIR)/*.pdb $(OBJS) $(OBJSLN) $(OBJS_SIMDUTF8) $(OBJS_PTHREADS4W) $(SRC_ASM) $(RCS)

.PHONY: all clean nonclean
