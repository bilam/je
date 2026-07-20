/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Xenos: Miscellaneous                                                    */

#define _GNU_SOURCE

#ifdef _WIN32
#define __iamcu__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "j.h"
#include "x.h"

#if defined(__APPLE__)
#include <sys/param.h>
#include <sys/sysctl.h>
#endif

#include "cpuinfo.h"
extern uint64_t g_cpuFeatures,g0_cpuFeatures;
extern uint64_t g_cpuFeatures2,g0_cpuFeatures2;
extern void*libcblas;
extern char hascblas;
extern C    cblasfile[];
extern char hasopenmp;

#ifdef BOXEDSPARSE
extern UC fboxedsparse;
#endif

#include <string.h>
#ifdef _WIN32
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

#include <signal.h>

static const char *hex_digits = "0123456789abcdef";

/* not null terminated */
static UC* tohex(UC* dest, UC* src, I len)
{
  I i;
  UC* d = src;
  UC* buffer = dest;
  for (i = 0; i < len; i++) {
    *buffer++ = hex_digits[(*d & 0xf0) >> 4];
    *buffer++ = hex_digits[*d & 0x0f];
    d++;
  }
  R dest;
}

F1(jtassertq){ASSERTMTV(w); R scb(jt->assert);}

F1(jtasserts){B b; RE(b=b0(w)); jt->assert=b; R mtm;}

F1(jtboxq){ASSERTMTV(w); R ca(jt->bxa);}

F1(jtboxs){A x;
 RZ(w=vs(w));
 ASSERT(11==*AS(w),EVLENGTH);
 x=jt->bxa; RZ(jt->bxa=ra(w)); jt->bx=CAV(jt->bxa); fa(x);
 R mtv;
}

F1(jtctq){ASSERTMTV(w); R scf(jt->ct);}

F1(jtcts){D d;
 ASSERT(!AR(w),EVRANK);
 RZ(w=cvt(FL,w)); d=*DAV(w);
 ASSERT(0<=d,EVDOMAIN); 
 ASSERT(d<=5.820766091e-11,EVDOMAIN);
 jt->ct=d;
 R mtv;
}

F1(jtdispq){A z; ASSERTMTV(w); GA(z,INT,*jt->disp,1,0); ICPY(AV(z),1+jt->disp,*jt->disp); R z;}

F1(jtdisps){I n;
 RZ(w=vi(w));
 n=AN(w);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(all1(nubsieve(w)),EVDOMAIN);
 ASSERT(all1(eps(w,eval("1 2 4 5 6"))),EVINDEX);
 *jt->disp=n; ICPY(1+jt->disp,AV(w),n);
 R mtv;
}

F1(jtdotnamesq){ASSERTMTV(w); R jt->dotnames?one:zero;}

F1(jtdotnamess){B b,c;
 RZ(w);
 ASSERT(!AR(w),EVRANK);
 if(!(B01&AT(w)))RZ(w=cvt(B01,w));
 c=jt->dotnames; jt->dotnames=b=*BAV(w);
 if(c&&!b)ds(CMDOT)=ds(CNDOT)=ds(CUDOT)=ds(CVDOT)=ds(CXDOT)=ds(CYDOT)=0;
 else if(!c&&b){
  ds(CMDOT)=mdot;
  ds(CNDOT)=ndot;
  ds(CUDOT)=udot;
  ds(CVDOT)=vdot;
  ds(CXDOT)=xdot;
  ds(CYDOT)=ydot;
 }
 R mtv;
}

F1(jtevmq){ASSERTMTV(w); R behead(jt->evm);}

F1(jtevms){A t,*tv,*wv;
 RZ(w);
 ASSERT(1==AR(w),EVRANK);
 ASSERT(NEVM==AN(w),EVLENGTH);
 ASSERT(BOX&AT(w),EVDOMAIN);
 GA(t,BOX,1+NEVM,1,0); tv=AAV(t); 
 *tv++=mtv;
 if(ARELATIVE(w))RZ(w=car(w));
 wv=AAV(w);
 DO(NEVM, RZ(*tv++=vs(*wv++)););
 ra(t); fa(jt->evm); jt->evm=t;
 R mtv;
}

F1(jtfxx){
 RZ(w);
 ASSERT(AT(w)&LIT+BOX,EVDOMAIN);
 ASSERT(1>=AR(w),EVRANK);
 R fx(ope(w)); 
}

F1(jtiepdoq){ASSERTMTV(w); R scb(jt->iepdo);}

F1(jtiepdos){B b; RE(b=b0(w)); jt->iepdo=b; R mtm;}

F1(jtiepq){
 ASSERTMTV(w); 
 ASSERT(1==AR(w),EVRANK);
 ASSERT(!AN(w),EVDOMAIN); 
 R jt->iep?jt->iep:mtv;
}

F1(jtieps){
 RZ(w);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(!AN(w)||AT(w)&LIT,EVDOMAIN);
 fa(jt->iep);
 RZ(jt->iep=ra(w)); 
 R mtm;
}

I prokey=1; /* enabled for 5.01 beta */

F1(jtoutparmq){A z;D*u,x;I*v;
 ASSERTMTV(w);
 if(IMAX==jt->outmaxlen||IMAX==jt->outmaxbefore||IMAX==jt->outmaxafter){
  GA(z,FL, 4,1,0); u=DAV(z);
  u[0]=(D)jt->outeol;
  x=(D)jt->outmaxlen;    u[1]=x==(D)IMAX?inf:x;
  x=(D)jt->outmaxbefore; u[2]=x==(D)IMAX?inf:x;
  x=(D)jt->outmaxafter;  u[3]=x==(D)IMAX?inf:x;
 }else{
  GA(z,INT,4,1,0); v= AV(z);
  v[0]=jt->outeol;
  v[1]=jt->outmaxlen;
  v[2]=jt->outmaxbefore;
  v[3]=jt->outmaxafter;
 }
 R z;
}

F1(jtoutparms){I*v;
 RZ(w=vib(w));
 ASSERT(1==AR(w),EVRANK);
 ASSERT(4==AN(w),EVLENGTH);
 v=AV(w);
 ASSERT(0==v[0]||2==v[0],EVINDEX);
 ASSERT(0<=v[1],EVDOMAIN);
 ASSERT(0<=v[2],EVDOMAIN);
 ASSERT(0<=v[3],EVDOMAIN);
 jt->outeol      =v[0];
 jt->outmaxlen   =v[1];
 jt->outmaxbefore=v[2];
 jt->outmaxafter =v[3];
 R mtv;
}

F1(jtposq){ASSERTMTV(w); R v2(jt->pos[0],jt->pos[1]);}

F1(jtposs){I n,p,q,*v;
 RZ(w=vi(w));
 n=AN(w); v=AV(w);
 ASSERT(1>=AR(w),EVRANK);
 ASSERT(1==n||2==n,EVLENGTH);
 if(1==n)p=q=*v; else{p=v[0]; q=v[1];} 
 ASSERT(0<=p&&p<=2&&0<=q&&q<=2,EVDOMAIN);
 jt->pos[0]=p; jt->pos[1]=q;   
 R mtv;
}

F1(jtppq){C*end;I k;
 ASSERTMTV(w);
 k = strtoI(3+jt->pp, (char**)&end, 10);
 R sc(k);
}

F1(jtpps){I k;
 RE(sc(k=i0(w))); ASSERT(0<k,EVDOMAIN); ASSERT(k<=NPP,EVLIMIT);
 sprintf(3+jt->pp,FMTI"g", k);
 R mtv;
}

F1(jtretcommq){ASSERTMTV(w); R scb(jt->retcomm);}

F1(jtretcomms){B b; RE(b=b0(w)); jt->retcomm=b; R mtm;}

F1(jtseclevq){ASSERTMTV(w); R sc(jt->seclev);}

F1(jtseclevs){I k; 
 RE(k=i0(w)); 
 ASSERT(0==k||1==k,EVDOMAIN); 
 if(!jt->seclev&&1==k)jt->seclev=k;
 R mtm;
}

F1(jtsysparmq){I k;
 RE(k=i0(w));
 switch(k){
  default: ASSERT(0,EVINDEX);
  case 0:  R sc(jt->fdepn);
  case 1:  R sc(jt->fdepi);
  case 2:  R sc(jt->fcalln);
  case 3:  R sc(jt->fcalli);
}}

F1(jtsysparms){A*wv;I k,m,wd;
 RZ(w);
 ASSERT(BOX&AT(w),EVDOMAIN);
 ASSERT(1==AR(w),EVRANK);
 ASSERT(2==AN(w),EVLENGTH);
 wv=AAV(w); wd=(I)w*ARELATIVE(w);
 RE(k=i0(WVR(0)));
 switch(k){
  default: ASSERT(0,EVINDEX);
  case 0:  RE(m=i0(WVR(1))); jt->fdepn =m; break;
  case 1:  ASSERT(0,EVDOMAIN);  /* jt->fdepi  can not be set */
  case 2:  RE(m=i0(WVR(1))); jt->fcalln=m; break;
  case 3:  ASSERT(0,EVDOMAIN);  /* jt->fcalli can not be set */
 }
 R mtm;
}

F1(jtsysq){I j;
 ASSERTMTV(w);
 switch(SYS){
  case SYS_PC:        j=0;                break;
  case SYS_PC386:     j=1;                break;
  case SYS_PCWIN:     j=SY_WIN32 ? (SY_WINCE ? 7 : 6) : 2; break;
  case SYS_MACINTOSH: j=3;                break;
  case SYS_OS2:       j=4;                break;
  default:            j=SYS&SYS_UNIX ? 5 : -1;
 }
 R sc(j);
}

// 9!:56
// query/override cpu feature
F1(jtcpufeature){
 ASSERT(AT(w)&LIT,EVDOMAIN);
 ASSERT(AN(w),EVLENGTH);
 ASSERT(1>=AR(w),EVRANK);
 w=str0(w);
 if (!strcasecmp(CAV(w),"CPU")) {
#if defined(__wasm32__)
  R cstr("wasm32");
#elif defined(__wasm__)
  R cstr("wasm64");
#elif defined(__aarch64__)||defined(_M_ARM64)
  R cstr("arm64");
#elif defined(__arm__)||defined(_M_ARM)
  R cstr("arm");
#elif defined(__x86_64__)||defined(_M_X64)
  R cstr("x86_64");
#elif defined(__i386__)||defined(_M_IX86)
  R cstr("x86");
#else
  R cstr("unknown");
#endif
 } else if (!strcasecmp(CAV(w),"CORES")) {
  R sc(numberOfCores);
 } else if (!strcasecmp(CAV(w),"MAXTHREADS")) {
  R sc(PYXES?MAXTHREADS:1);
 } else if (!strcasecmp(CAV(w),"CBLAS")) {
  R sc(hascblas&&libcblas);
 } else if (!strcasecmp(CAV(w),"CBLASFILE")) {
  R cstr(cblasfile);
 } else if (!strcasecmp(CAV(w),"OPENMP")) {
#if defined(_OPENMP)
  R sc(hasopenmp);
#else
  R sc(0);
#endif
 } else if (!strcasecmp(CAV(w),"SLEEF")) {
#if defined(SLEEF)
  R sc(SLEEF);
#else
  R sc(0);
#endif
 } else if (!strcasecmp(CAV(w),"SLEEFQUAD")) {
#if defined(SLEEFQUAD)
  R sc(SLEEFQUAD);
#else
  R sc(0);
#endif
 } else if (!strcasecmp(CAV(w),"MAXOP")) {  // MAXOP for 128!:14 - must match with the value in xlp.c
  R sc(100);
 } else if (!strcasecmp(CAV(w),"CACHELINESIZE")) {  // cacheline size used in compilation time
  R sc(CACHELINESIZE);
 } else if (!strcasecmp(CAV(w),"CACHELINESIZEHW")) {  // cacheline size by querrying OS during runtime
#if defined(__APPLE__)
  size_t line_size = 0;
  size_t size = sizeof(line_size);
  if (sysctlbyname("hw.cachelinesize", &line_size, &size, NULL, 0)) R sc(0);
  R sc(line_size);
#elif defined(_WIN32)
  size_t line_size = 0;
  DWORD buffer_size = 0;
  DWORD i = 0;
  SYSTEM_LOGICAL_PROCESSOR_INFORMATION * buffer = 0;

  if (!GetLogicalProcessorInformation(0, &buffer_size)) R sc(0);
  buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *)malloc(buffer_size);
  if (!GetLogicalProcessorInformation(&buffer[0], &buffer_size)) {free(buffer); R sc(0);};

  for (i = 0; i != buffer_size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) {
      if (buffer[i].Relationship == RelationCache && buffer[i].Cache.Level == 1) {
          line_size = buffer[i].Cache.LineSize;
          break;
      }
  }
  free(buffer);
  R sc(line_size);
#elif defined(__linux__)
  R sc(sysconf(_SC_LEVEL1_DCACHE_LINESIZE));
#else
  R sc(64);
#endif
 } else if(!strcasecmp(CAV(w),"CPUSETSIZE")) {
#if defined(CPU_SETSIZE)
  R sc(CPU_SETSIZE);
#else
  R sc(0);
#endif
 } else if(!strcasecmp(CAV(w),"SUPPORTAFFINITY")) {
  R sc(supportaffinity);
 } else if(!strcasecmp(CAV(w),"C_AVX2")) {
  R sc(C_AVX2);
 } else if(!strcasecmp(CAV(w),"C_AVX512")) {
  R sc(C_AVX512);
 } else if(!strcasecmp(CAV(w),"C_CRC32C")) {
  R sc(C_CRC32C);
 } else if(!strcasecmp(CAV(w),"C_VIAVX")) {
  R sc(C_VIAVX);
 } else if(!strcasecmp(CAV(w),"EMU_AVX2")) {
  R sc(EMU_AVX2);
 } else if(!strcasecmp(CAV(w),"GMP")) {
  R sc(0);
 } else if(!strcasecmp(CAV(w),"MEMAUDIT")) {
  R sc(MEMAUDIT);
 } else if(!strcasecmp(CAV(w),"PYXES")) {
  R sc(PYXES);
 } else if(!strcasecmp(CAV(w),"NAN.C")) {
#if defined(__has_builtin)
#if  __has_builtin(__builtin_nan)
  double a = __builtin_nan("");
  UC dh[2*8];
  R str(16, tohex(dh,(UC*)&a,8));
#else
  R cstr("");
#endif
#else
  R cstr("");
#endif
 } else if(!strcasecmp(CAV(w),"INFINITY.C")) {
#if defined(__has_builtin)
#if __has_builtin(__builtin_inf)
  double a = __builtin_inf();
  UC dh[2*8];
  R str(16, tohex(dh,(UC*)&a,8));
#else
  R cstr("");
#endif
#else
  R cstr("");
#endif
 } else if(!strcasecmp(CAV(w),"NAN")) {
  UC dh[2*8];
  R str(16, tohex(dh,(UC*)XNAN,8));
 } else if(!strcasecmp(CAV(w),"INFINITY")) {
  UC dh[2*8];
  R str(16, tohex(dh,(UC*)XINF,8));
 } else if(!strcasecmp(CAV(w),"NORMAH")) {
  R sc(NORMAH);
 }
#if defined(__aarch64__)
 if     (!strcasecmp(CAV(w),"FP"      )) R sc(!!(getCpuFeatures()&ARM_HWCAP_FP ));
 else if(!strcasecmp(CAV(w),"ASIMD"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_ASIMD ));
 else if(!strcasecmp(CAV(w),"EVTSTRM" )) R sc(!!(getCpuFeatures()&ARM_HWCAP_EVTSTRM ));
 else if(!strcasecmp(CAV(w),"AES"     )) R sc(!!(getCpuFeatures()&ARM_HWCAP_AES ));
 else if(!strcasecmp(CAV(w),"PMULL"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_PMULL ));
 else if(!strcasecmp(CAV(w),"SHA1"    )) R sc(!!(getCpuFeatures()&ARM_HWCAP_SHA1 ));
 else if(!strcasecmp(CAV(w),"SHA2"    )) R sc(!!(getCpuFeatures()&ARM_HWCAP_SHA2 ));
 else if(!strcasecmp(CAV(w),"CRC32"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_CRC32 ));
 else if(!strcasecmp(CAV(w),"ATOMICS" )) R sc(!!(getCpuFeatures()&ARM_HWCAP_ATOMICS ));
 else if(!strcasecmp(CAV(w),"FPHP"    )) R sc(!!(getCpuFeatures()&ARM_HWCAP_FPHP ));
 else if(!strcasecmp(CAV(w),"ASIMDHP" )) R sc(!!(getCpuFeatures()&ARM_HWCAP_ASIMDHP ));
 else if(!strcasecmp(CAV(w),"CPUID"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_CPUID ));
 else if(!strcasecmp(CAV(w),"ASIMDRDM")) R sc(!!(getCpuFeatures()&ARM_HWCAP_ASIMDRDM ));
 else if(!strcasecmp(CAV(w),"JSCVT"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_JSCVT ));
 else if(!strcasecmp(CAV(w),"FCMA"    )) R sc(!!(getCpuFeatures()&ARM_HWCAP_FCMA ));
 else if(!strcasecmp(CAV(w),"LRCPC"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_LRCPC ));
 else if(!strcasecmp(CAV(w),"DCPOP"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_DCPOP ));
 else if(!strcasecmp(CAV(w),"SHA3"    )) R sc(!!(getCpuFeatures()&ARM_HWCAP_SHA3 ));
 else if(!strcasecmp(CAV(w),"SM3"     )) R sc(!!(getCpuFeatures()&ARM_HWCAP_SM3 ));
 else if(!strcasecmp(CAV(w),"SM4"     )) R sc(!!(getCpuFeatures()&ARM_HWCAP_SM4 ));
 else if(!strcasecmp(CAV(w),"ASIMDDP" )) R sc(!!(getCpuFeatures()&ARM_HWCAP_ASIMDDP ));
 else if(!strcasecmp(CAV(w),"SHA512"  )) R sc(!!(getCpuFeatures()&ARM_HWCAP_SHA512 ));
 else if(!strcasecmp(CAV(w),"SVE"     )) R sc(!!(getCpuFeatures()&ARM_HWCAP_SVE ));
 else if(!strcasecmp(CAV(w),"ASIMDFHM")) R sc(!!(getCpuFeatures()&ARM_HWCAP_ASIMDFHM ));
 else if(!strcasecmp(CAV(w),"DIT"     )) R sc(!!(getCpuFeatures()&ARM_HWCAP_DIT ));
 else if(!strcasecmp(CAV(w),"USCAT"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_USCAT ));
 else if(!strcasecmp(CAV(w),"ILRCPC"  )) R sc(!!(getCpuFeatures()&ARM_HWCAP_ILRCPC ));
 else if(!strcasecmp(CAV(w),"FLAGM"   )) R sc(!!(getCpuFeatures()&ARM_HWCAP_FLAGM ));
 else if(!strcasecmp(CAV(w),"SSBS"    )) R sc(!!(getCpuFeatures()&ARM_HWCAP_SSBS ));
 else if(!strcasecmp(CAV(w),"SB"      )) R sc(!!(getCpuFeatures()&ARM_HWCAP_SB ));
 else if(!strcasecmp(CAV(w),"PACA"    )) R sc(!!(getCpuFeatures()&ARM_HWCAP_PACA ));
 else if(!strcasecmp(CAV(w),"PACG"    )) R sc(!!(getCpuFeatures()&ARM_HWCAP_PACG ));
 else if(!strcasecmp(CAV(w),"DCPODP"  )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_DCPODP ));
 else if(!strcasecmp(CAV(w),"SVE2"    )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SVE2 ));
 else if(!strcasecmp(CAV(w),"SVEAES"  )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SVEAES ));
 else if(!strcasecmp(CAV(w),"SVEPMULL" )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SVEPMULL ));
 else if(!strcasecmp(CAV(w),"SVEBITPERM" )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SVEBITPERM ));
 else if(!strcasecmp(CAV(w),"SVESHA3" )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SVESHA3 ));
 else if(!strcasecmp(CAV(w),"SVESM4"  )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SVESM4 ));
 else if(!strcasecmp(CAV(w),"FLAGM2"  )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_FLAGM2 ));
 else if(!strcasecmp(CAV(w),"FRINT"   )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_FRINT ));
 else if(!strcasecmp(CAV(w),"SVEI8MM" )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SVEI8MM ));
 else if(!strcasecmp(CAV(w),"SVEF32MM" )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SVEF32MM ));
 else if(!strcasecmp(CAV(w),"SVEF64MM" )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SVEF64MM ));
 else if(!strcasecmp(CAV(w),"SVEBF16" )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SVEBF16 ));
 else if(!strcasecmp(CAV(w),"I8MM"    )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_I8MM ));
 else if(!strcasecmp(CAV(w),"BF16"    )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_BF16 ));
 else if(!strcasecmp(CAV(w),"DGH"     )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_DGH ));
 else if(!strcasecmp(CAV(w),"RNG"     )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_RNG ));
 else if(!strcasecmp(CAV(w),"BTI"     )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_BTI ));
 else if(!strcasecmp(CAV(w),"MTE"     )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_MTE ));
 else if(!strcasecmp(CAV(w),"ECV"     )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_ECV ));
 else if(!strcasecmp(CAV(w),"AFP"     )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_AFP ));
 else if(!strcasecmp(CAV(w),"RPRES"   )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_RPRES ));
 else if(!strcasecmp(CAV(w),"MTE3"    )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_MTE3 ));
 else if(!strcasecmp(CAV(w),"SME"     )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SME ));
 else if(!strcasecmp(CAV(w),"SME_I16I64" )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SME_I16I64 ));
 else if(!strcasecmp(CAV(w),"SME_F64F64" )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SME_F64F64 ));
 else if(!strcasecmp(CAV(w),"SME_I8I32" )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SME_I8I32 ));
 else if(!strcasecmp(CAV(w),"SME_F16F32" )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SME_F16F32 ));
 else if(!strcasecmp(CAV(w),"SME_B16F32" )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SME_B16F32 ));
 else if(!strcasecmp(CAV(w),"SME_F32F32" )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SME_F32F32 ));
 else if(!strcasecmp(CAV(w),"SME_FA64" )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SME_FA64 ));
 else if(!strcasecmp(CAV(w),"WFXT"    )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_WFXT ));
#if defined(_WIN64)||defined(__LP64__)
 else if(!strcasecmp(CAV(w),"EBF16"   )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_EBF16 ));
 else if(!strcasecmp(CAV(w),"SVE_EBF16" )) R sc(!!(getCpuFeatures2()&ARM_HWCAP2_SVE_EBF16 ));
#endif
 else if(!strcasecmp(CAV(w),"FMA"     )) R sc(1);
 else R sc(0);
#elif defined(__x86_64__)||defined(__i386__)||defined(_M_X64)||defined(_M_IX86)
 if     (!strcasecmp(CAV(w),"SSSE3"   )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_SSSE3 ));
 else if(!strcasecmp(CAV(w),"POPCNT"  )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_POPCNT ));
 else if(!strcasecmp(CAV(w),"MOVBE"   )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_MOVBE ));
 else if(!strcasecmp(CAV(w),"SSE4_1"  )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_SSE4_1 ));
 else if(!strcasecmp(CAV(w),"SSE4_2"  )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_SSE4_2 ));
 else if(!strcasecmp(CAV(w),"AES_NI"  )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_AES_NI ));
 else if(!strcasecmp(CAV(w),"AVX"     )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_AVX ));
 else if(!strcasecmp(CAV(w),"RDRAND"  )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_RDRAND ));
 else if(!strcasecmp(CAV(w),"AVX2"    )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_AVX2 ));
 else if(!strcasecmp(CAV(w),"SHA_NI"  )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_SHA_NI ));
 else if(!strcasecmp(CAV(w),"FMA"     )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_FMA ));
 else if(!strcasecmp(CAV(w),"RDSEED"  )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_RDSEED ));
 else if(!strcasecmp(CAV(w),"BMI1"    )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_BMI1 ));
 else if(!strcasecmp(CAV(w),"BMI2"    )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_BMI2 ));
 else if(!strcasecmp(CAV(w),"AVX512F" )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_AVX512F));
 else if(!strcasecmp(CAV(w),"AVX512VL" )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_AVX512VL));
 else if(!strcasecmp(CAV(w),"AVX512DQ" )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_AVX512DQ));
 else if(!strcasecmp(CAV(w),"AVX512BW" )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_AVX512BW));
 else if(!strcasecmp(CAV(w),"AVX512IFMA" )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_AVX512IFMA));
 else if(!strcasecmp(CAV(w),"AVX512VBMI" )) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_AVX512VBMI));
 else if(!strcasecmp(CAV(w),"AVX512VBMI2")) R sc(!!(getCpuFeatures()&CPU_X86_FEATURE_AVX512VBMI2));
 else if(!strcasecmp(CAV(w),"RING3MWAIT" )) R sc(!!(getCpuFeatures2()&CPU_X86_FEATURE2_RING3MWAIT));
 else if(!strcasecmp(CAV(w),"FSGSBASE" )) R sc(!!(getCpuFeatures2()&CPU_X86_FEATURE2_FSGSBASE));
 else R sc(0);
#else
 R sc(0);
#endif
}

// thread unsafe
F2(jtcpufeature2){I k;
 ASSERT(AT(w)&LIT,EVDOMAIN);
 ASSERT(AN(w),EVLENGTH);
 ASSERT(1>=AR(w),EVRANK);
 k=i0(a);  // get arg
 ASSERT(k==0||k==1,EVDOMAIN);
 w=str0(w);
 if(k){
#if defined(__aarch64__)
 if     (!strcasecmp(CAV(w),"FP"      )) g_cpuFeatures |= ARM_HWCAP_FP ;
 else if(!strcasecmp(CAV(w),"ASIMD"   )) g_cpuFeatures |= ARM_HWCAP_ASIMD ;
 else if(!strcasecmp(CAV(w),"EVTSTRM" )) g_cpuFeatures |= ARM_HWCAP_EVTSTRM ;
 else if(!strcasecmp(CAV(w),"AES"     )) g_cpuFeatures |= ARM_HWCAP_AES ;
 else if(!strcasecmp(CAV(w),"PMULL"   )) g_cpuFeatures |= ARM_HWCAP_PMULL ;
 else if(!strcasecmp(CAV(w),"SHA1"    )) g_cpuFeatures |= ARM_HWCAP_SHA1 ;
 else if(!strcasecmp(CAV(w),"SHA2"    )) g_cpuFeatures |= ARM_HWCAP_SHA2 ;
 else if(!strcasecmp(CAV(w),"CRC32"   )) g_cpuFeatures |= ARM_HWCAP_CRC32 ;
 else if(!strcasecmp(CAV(w),"ATOMICS" )) g_cpuFeatures |= ARM_HWCAP_ATOMICS ;
 else if(!strcasecmp(CAV(w),"FPHP"    )) g_cpuFeatures |= ARM_HWCAP_FPHP ;
 else if(!strcasecmp(CAV(w),"ASIMDHP" )) g_cpuFeatures |= ARM_HWCAP_ASIMDHP ;
 else if(!strcasecmp(CAV(w),"CPUID"   )) g_cpuFeatures |= ARM_HWCAP_CPUID ;
 else if(!strcasecmp(CAV(w),"ASIMDRDM")) g_cpuFeatures |= ARM_HWCAP_ASIMDRDM ;
 else if(!strcasecmp(CAV(w),"JSCVT"   )) g_cpuFeatures |= ARM_HWCAP_JSCVT ;
 else if(!strcasecmp(CAV(w),"FCMA"    )) g_cpuFeatures |= ARM_HWCAP_FCMA ;
 else if(!strcasecmp(CAV(w),"LRCPC"   )) g_cpuFeatures |= ARM_HWCAP_LRCPC ;
 else if(!strcasecmp(CAV(w),"DCPOP"   )) g_cpuFeatures |= ARM_HWCAP_DCPOP ;
 else if(!strcasecmp(CAV(w),"SHA3"    )) g_cpuFeatures |= ARM_HWCAP_SHA3 ;
 else if(!strcasecmp(CAV(w),"SM3"     )) g_cpuFeatures |= ARM_HWCAP_SM3 ;
 else if(!strcasecmp(CAV(w),"SM4"     )) g_cpuFeatures |= ARM_HWCAP_SM4 ;
 else if(!strcasecmp(CAV(w),"ASIMDDP" )) g_cpuFeatures |= ARM_HWCAP_ASIMDDP ;
 else if(!strcasecmp(CAV(w),"SHA512"  )) g_cpuFeatures |= ARM_HWCAP_SHA512 ;
 else if(!strcasecmp(CAV(w),"SVE"     )) g_cpuFeatures |= ARM_HWCAP_SVE ;
 else if(!strcasecmp(CAV(w),"ASIMDFHM")) g_cpuFeatures |= ARM_HWCAP_ASIMDFHM ;
 else if(!strcasecmp(CAV(w),"DIT"     )) g_cpuFeatures |= ARM_HWCAP_DIT ;
 else if(!strcasecmp(CAV(w),"USCAT"   )) g_cpuFeatures |= ARM_HWCAP_USCAT ;
 else if(!strcasecmp(CAV(w),"ILRCPC"  )) g_cpuFeatures |= ARM_HWCAP_ILRCPC ;
 else if(!strcasecmp(CAV(w),"FLAGM"   )) g_cpuFeatures |= ARM_HWCAP_FLAGM ;
 else if(!strcasecmp(CAV(w),"SSBS"    )) g_cpuFeatures |= ARM_HWCAP_SSBS ;
 else if(!strcasecmp(CAV(w),"SB"      )) g_cpuFeatures |= ARM_HWCAP_SB ;
 else if(!strcasecmp(CAV(w),"PACA"    )) g_cpuFeatures |= ARM_HWCAP_PACA ;
 else if(!strcasecmp(CAV(w),"PACG"    )) g_cpuFeatures |= ARM_HWCAP_PACG ;
 else if(!strcasecmp(CAV(w),"DCPODP"  )) g_cpuFeatures2 |= ARM_HWCAP2_DCPODP ;
 else if(!strcasecmp(CAV(w),"SVE2"    )) g_cpuFeatures2 |= ARM_HWCAP2_SVE2 ;
 else if(!strcasecmp(CAV(w),"SVEAES"  )) g_cpuFeatures2 |= ARM_HWCAP2_SVEAES ;
 else if(!strcasecmp(CAV(w),"SVEPMULL" )) g_cpuFeatures2 |= ARM_HWCAP2_SVEPMULL ;
 else if(!strcasecmp(CAV(w),"SVEBITPERM" )) g_cpuFeatures2 |= ARM_HWCAP2_SVEBITPERM ;
 else if(!strcasecmp(CAV(w),"SVESHA3" )) g_cpuFeatures2 |= ARM_HWCAP2_SVESHA3 ;
 else if(!strcasecmp(CAV(w),"SVESM4"  )) g_cpuFeatures2 |= ARM_HWCAP2_SVESM4 ;
 else if(!strcasecmp(CAV(w),"FLAGM2"  )) g_cpuFeatures2 |= ARM_HWCAP2_FLAGM2 ;
 else if(!strcasecmp(CAV(w),"FRINT"   )) g_cpuFeatures2 |= ARM_HWCAP2_FRINT ;
 else if(!strcasecmp(CAV(w),"SVEI8MM" )) g_cpuFeatures2 |= ARM_HWCAP2_SVEI8MM ;
 else if(!strcasecmp(CAV(w),"SVEF32MM" )) g_cpuFeatures2 |= ARM_HWCAP2_SVEF32MM ;
 else if(!strcasecmp(CAV(w),"SVEF64MM" )) g_cpuFeatures2 |= ARM_HWCAP2_SVEF64MM ;
 else if(!strcasecmp(CAV(w),"SVEBF16" )) g_cpuFeatures2 |= ARM_HWCAP2_SVEBF16 ;
 else if(!strcasecmp(CAV(w),"I8MM"    )) g_cpuFeatures2 |= ARM_HWCAP2_I8MM ;
 else if(!strcasecmp(CAV(w),"BF16"    )) g_cpuFeatures2 |= ARM_HWCAP2_BF16 ;
 else if(!strcasecmp(CAV(w),"DGH"     )) g_cpuFeatures2 |= ARM_HWCAP2_DGH ;
 else if(!strcasecmp(CAV(w),"RNG"     )) g_cpuFeatures2 |= ARM_HWCAP2_RNG ;
 else if(!strcasecmp(CAV(w),"BTI"     )) g_cpuFeatures2 |= ARM_HWCAP2_BTI ;
 else if(!strcasecmp(CAV(w),"MTE"     )) g_cpuFeatures2 |= ARM_HWCAP2_MTE ;
 else if(!strcasecmp(CAV(w),"ECV"     )) g_cpuFeatures2 |= ARM_HWCAP2_ECV ;
 else if(!strcasecmp(CAV(w),"AFP"     )) g_cpuFeatures2 |= ARM_HWCAP2_AFP ;
 else if(!strcasecmp(CAV(w),"RPRES"   )) g_cpuFeatures2 |= ARM_HWCAP2_RPRES ;
 else if(!strcasecmp(CAV(w),"MTE3"    )) g_cpuFeatures2 |= ARM_HWCAP2_MTE3 ;
 else if(!strcasecmp(CAV(w),"SME"     )) g_cpuFeatures2 |= ARM_HWCAP2_SME ;
 else if(!strcasecmp(CAV(w),"SME_I16I64" )) g_cpuFeatures2 |= ARM_HWCAP2_SME_I16I64 ;
 else if(!strcasecmp(CAV(w),"SME_F64F64" )) g_cpuFeatures2 |= ARM_HWCAP2_SME_F64F64 ;
 else if(!strcasecmp(CAV(w),"SME_I8I32" )) g_cpuFeatures2 |= ARM_HWCAP2_SME_I8I32 ;
 else if(!strcasecmp(CAV(w),"SME_F16F32" )) g_cpuFeatures2 |= ARM_HWCAP2_SME_F16F32 ;
 else if(!strcasecmp(CAV(w),"SME_B16F32" )) g_cpuFeatures2 |= ARM_HWCAP2_SME_B16F32 ;
 else if(!strcasecmp(CAV(w),"SME_F32F32" )) g_cpuFeatures2 |= ARM_HWCAP2_SME_F32F32 ;
 else if(!strcasecmp(CAV(w),"SME_FA64" )) g_cpuFeatures2 |= ARM_HWCAP2_SME_FA64 ;
 else if(!strcasecmp(CAV(w),"WFXT"    )) g_cpuFeatures2 |= ARM_HWCAP2_WFXT ;
#if defined(_WIN64)||defined(__LP64__)
 else if(!strcasecmp(CAV(w),"EBF16"   )) g_cpuFeatures2 |= ARM_HWCAP2_EBF16 ;
 else if(!strcasecmp(CAV(w),"SVE_EBF16" )) g_cpuFeatures2 |= ARM_HWCAP2_SVE_EBF16 ;
#endif

#elif defined(__x86_64__)||defined(__i386__)||defined(_M_X64)||defined(_M_IX86)
 if     (!strcasecmp(CAV(w),"SSSE3"   )) g_cpuFeatures |= CPU_X86_FEATURE_SSSE3 ;
 else if(!strcasecmp(CAV(w),"POPCNT"  )) g_cpuFeatures |= CPU_X86_FEATURE_POPCNT ;
 else if(!strcasecmp(CAV(w),"MOVBE"   )) g_cpuFeatures |= CPU_X86_FEATURE_MOVBE ;
 else if(!strcasecmp(CAV(w),"SSE4_1"  )) g_cpuFeatures |= CPU_X86_FEATURE_SSE4_1 ;
 else if(!strcasecmp(CAV(w),"SSE4_2"  )) g_cpuFeatures |= CPU_X86_FEATURE_SSE4_2 ;
 else if(!strcasecmp(CAV(w),"AES_NI"  )) g_cpuFeatures |= CPU_X86_FEATURE_AES_NI ;
 else if(!strcasecmp(CAV(w),"AVX"     )) g_cpuFeatures |= CPU_X86_FEATURE_AVX ;
 else if(!strcasecmp(CAV(w),"RDRAND"  )) g_cpuFeatures |= CPU_X86_FEATURE_RDRAND ;
 else if(!strcasecmp(CAV(w),"AVX2"    )) g_cpuFeatures |= CPU_X86_FEATURE_AVX2 ;
 else if(!strcasecmp(CAV(w),"SHA_NI"  )) g_cpuFeatures |= CPU_X86_FEATURE_SHA_NI ;
 else if(!strcasecmp(CAV(w),"FMA"     )) g_cpuFeatures |= CPU_X86_FEATURE_FMA ;
 else if(!strcasecmp(CAV(w),"RDSEED"  )) g_cpuFeatures |= CPU_X86_FEATURE_RDSEED ;
 else if(!strcasecmp(CAV(w),"BMI1"    )) g_cpuFeatures |= CPU_X86_FEATURE_BMI1 ;
 else if(!strcasecmp(CAV(w),"BMI2"    )) g_cpuFeatures |= CPU_X86_FEATURE_BMI2 ;
 else if(!strcasecmp(CAV(w),"AVX512F" )) g_cpuFeatures |= CPU_X86_FEATURE_AVX512F ;
 else if(!strcasecmp(CAV(w),"AVX512VL" )) g_cpuFeatures |= CPU_X86_FEATURE_AVX512VL ;
 else if(!strcasecmp(CAV(w),"AVX512DQ" )) g_cpuFeatures |= CPU_X86_FEATURE_AVX512DQ ;
 else if(!strcasecmp(CAV(w),"AVX512BW" )) g_cpuFeatures |= CPU_X86_FEATURE_AVX512BW ;
 else if(!strcasecmp(CAV(w),"AVX512IFMA" )) g_cpuFeatures |= CPU_X86_FEATURE_AVX512IFMA ;
 else if(!strcasecmp(CAV(w),"AVX512VBMI" )) g_cpuFeatures |= CPU_X86_FEATURE_AVX512VBMI ;
 else if(!strcasecmp(CAV(w),"AVX512VBMI2" )) g_cpuFeatures |= CPU_X86_FEATURE_AVX512VBMI2 ;
 else if(!strcasecmp(CAV(w),"RING3MWAIT" )) g_cpuFeatures2 |= CPU_X86_FEATURE2_RING3MWAIT ;
 else if(!strcasecmp(CAV(w),"FSGSBASE" )) g_cpuFeatures2 |= CPU_X86_FEATURE2_FSGSBASE ;
#endif
 } else {
#if defined(__aarch64__)
 if     (!strcasecmp(CAV(w),"FP"      )) g_cpuFeatures &= ~ARM_HWCAP_FP ;
 else if(!strcasecmp(CAV(w),"ASIMD"   )) g_cpuFeatures &= ~ARM_HWCAP_ASIMD ;
 else if(!strcasecmp(CAV(w),"EVTSTRM" )) g_cpuFeatures &= ~ARM_HWCAP_EVTSTRM ;
 else if(!strcasecmp(CAV(w),"AES"     )) g_cpuFeatures &= ~ARM_HWCAP_AES ;
 else if(!strcasecmp(CAV(w),"PMULL"   )) g_cpuFeatures &= ~ARM_HWCAP_PMULL ;
 else if(!strcasecmp(CAV(w),"SHA1"    )) g_cpuFeatures &= ~ARM_HWCAP_SHA1 ;
 else if(!strcasecmp(CAV(w),"SHA2"    )) g_cpuFeatures &= ~ARM_HWCAP_SHA2 ;
 else if(!strcasecmp(CAV(w),"CRC32"   )) g_cpuFeatures &= ~ARM_HWCAP_CRC32 ;
 else if(!strcasecmp(CAV(w),"ATOMICS" )) g_cpuFeatures &= ~ARM_HWCAP_ATOMICS ;
 else if(!strcasecmp(CAV(w),"FPHP"    )) g_cpuFeatures &= ~ARM_HWCAP_FPHP ;
 else if(!strcasecmp(CAV(w),"ASIMDHP" )) g_cpuFeatures &= ~ARM_HWCAP_ASIMDHP ;
 else if(!strcasecmp(CAV(w),"CPUID"   )) g_cpuFeatures &= ~ARM_HWCAP_CPUID ;
 else if(!strcasecmp(CAV(w),"ASIMDRDM")) g_cpuFeatures &= ~ARM_HWCAP_ASIMDRDM ;
 else if(!strcasecmp(CAV(w),"JSCVT"   )) g_cpuFeatures &= ~ARM_HWCAP_JSCVT ;
 else if(!strcasecmp(CAV(w),"FCMA"    )) g_cpuFeatures &= ~ARM_HWCAP_FCMA ;
 else if(!strcasecmp(CAV(w),"LRCPC"   )) g_cpuFeatures &= ~ARM_HWCAP_LRCPC ;
 else if(!strcasecmp(CAV(w),"DCPOP"   )) g_cpuFeatures &= ~ARM_HWCAP_DCPOP ;
 else if(!strcasecmp(CAV(w),"SHA3"    )) g_cpuFeatures &= ~ARM_HWCAP_SHA3 ;
 else if(!strcasecmp(CAV(w),"SM3"     )) g_cpuFeatures &= ~ARM_HWCAP_SM3 ;
 else if(!strcasecmp(CAV(w),"SM4"     )) g_cpuFeatures &= ~ARM_HWCAP_SM4 ;
 else if(!strcasecmp(CAV(w),"ASIMDDP" )) g_cpuFeatures &= ~ARM_HWCAP_ASIMDDP ;
 else if(!strcasecmp(CAV(w),"SHA512"  )) g_cpuFeatures &= ~ARM_HWCAP_SHA512 ;
 else if(!strcasecmp(CAV(w),"SVE"     )) g_cpuFeatures &= ~ARM_HWCAP_SVE ;
 else if(!strcasecmp(CAV(w),"ASIMDFHM")) g_cpuFeatures &= ~ARM_HWCAP_ASIMDFHM ;
 else if(!strcasecmp(CAV(w),"DIT"     )) g_cpuFeatures &= ~ARM_HWCAP_DIT ;
 else if(!strcasecmp(CAV(w),"USCAT"   )) g_cpuFeatures &= ~ARM_HWCAP_USCAT ;
 else if(!strcasecmp(CAV(w),"ILRCPC"  )) g_cpuFeatures &= ~ARM_HWCAP_ILRCPC ;
 else if(!strcasecmp(CAV(w),"FLAGM"   )) g_cpuFeatures &= ~ARM_HWCAP_FLAGM ;
 else if(!strcasecmp(CAV(w),"SSBS"    )) g_cpuFeatures &= ~ARM_HWCAP_SSBS ;
 else if(!strcasecmp(CAV(w),"SB"      )) g_cpuFeatures &= ~ARM_HWCAP_SB ;
 else if(!strcasecmp(CAV(w),"PACA"    )) g_cpuFeatures &= ~ARM_HWCAP_PACA ;
 else if(!strcasecmp(CAV(w),"PACG"    )) g_cpuFeatures &= ~ARM_HWCAP_PACG ;
 else if(!strcasecmp(CAV(w),"DCPODP"  )) g_cpuFeatures2 &= ~ARM_HWCAP2_DCPODP ;
 else if(!strcasecmp(CAV(w),"SVE2"    )) g_cpuFeatures2 &= ~ARM_HWCAP2_SVE2 ;
 else if(!strcasecmp(CAV(w),"SVEAES"  )) g_cpuFeatures2 &= ~ARM_HWCAP2_SVEAES ;
 else if(!strcasecmp(CAV(w),"SVEPMULL" )) g_cpuFeatures2 &= ~ARM_HWCAP2_SVEPMULL ;
 else if(!strcasecmp(CAV(w),"SVEBITPERM" )) g_cpuFeatures2 &= ~ARM_HWCAP2_SVEBITPERM ;
 else if(!strcasecmp(CAV(w),"SVESHA3" )) g_cpuFeatures2 &= ~ARM_HWCAP2_SVESHA3 ;
 else if(!strcasecmp(CAV(w),"SVESM4"  )) g_cpuFeatures2 &= ~ARM_HWCAP2_SVESM4 ;
 else if(!strcasecmp(CAV(w),"FLAGM2"  )) g_cpuFeatures2 &= ~ARM_HWCAP2_FLAGM2 ;
 else if(!strcasecmp(CAV(w),"FRINT"   )) g_cpuFeatures2 &= ~ARM_HWCAP2_FRINT ;
 else if(!strcasecmp(CAV(w),"SVEI8MM" )) g_cpuFeatures2 &= ~ARM_HWCAP2_SVEI8MM ;
 else if(!strcasecmp(CAV(w),"SVEF32MM" )) g_cpuFeatures2 &= ~ARM_HWCAP2_SVEF32MM ;
 else if(!strcasecmp(CAV(w),"SVEF64MM" )) g_cpuFeatures2 &= ~ARM_HWCAP2_SVEF64MM ;
 else if(!strcasecmp(CAV(w),"SVEBF16" )) g_cpuFeatures2 &= ~ARM_HWCAP2_SVEBF16 ;
 else if(!strcasecmp(CAV(w),"I8MM"    )) g_cpuFeatures2 &= ~ARM_HWCAP2_I8MM ;
 else if(!strcasecmp(CAV(w),"BF16"    )) g_cpuFeatures2 &= ~ARM_HWCAP2_BF16 ;
 else if(!strcasecmp(CAV(w),"DGH"     )) g_cpuFeatures2 &= ~ARM_HWCAP2_DGH ;
 else if(!strcasecmp(CAV(w),"RNG"     )) g_cpuFeatures2 &= ~ARM_HWCAP2_RNG ;
 else if(!strcasecmp(CAV(w),"BTI"     )) g_cpuFeatures2 &= ~ARM_HWCAP2_BTI ;
 else if(!strcasecmp(CAV(w),"MTE"     )) g_cpuFeatures2 &= ~ARM_HWCAP2_MTE ;
 else if(!strcasecmp(CAV(w),"ECV"     )) g_cpuFeatures2 &= ~ARM_HWCAP2_ECV ;
 else if(!strcasecmp(CAV(w),"AFP"     )) g_cpuFeatures2 &= ~ARM_HWCAP2_AFP ;
 else if(!strcasecmp(CAV(w),"RPRES"   )) g_cpuFeatures2 &= ~ARM_HWCAP2_RPRES ;
 else if(!strcasecmp(CAV(w),"MTE3"    )) g_cpuFeatures2 &= ~ARM_HWCAP2_MTE3 ;
 else if(!strcasecmp(CAV(w),"SME"     )) g_cpuFeatures2 &= ~ARM_HWCAP2_SME ;
 else if(!strcasecmp(CAV(w),"SME_I16I64" )) g_cpuFeatures2 &= ~ARM_HWCAP2_SME_I16I64 ;
 else if(!strcasecmp(CAV(w),"SME_F64F64" )) g_cpuFeatures2 &= ~ARM_HWCAP2_SME_F64F64 ;
 else if(!strcasecmp(CAV(w),"SME_I8I32" )) g_cpuFeatures2 &= ~ARM_HWCAP2_SME_I8I32 ;
 else if(!strcasecmp(CAV(w),"SME_F16F32" )) g_cpuFeatures2 &= ~ARM_HWCAP2_SME_F16F32 ;
 else if(!strcasecmp(CAV(w),"SME_B16F32" )) g_cpuFeatures2 &= ~ARM_HWCAP2_SME_B16F32 ;
 else if(!strcasecmp(CAV(w),"SME_F32F32" )) g_cpuFeatures2 &= ~ARM_HWCAP2_SME_F32F32 ;
 else if(!strcasecmp(CAV(w),"SME_FA64" )) g_cpuFeatures2 &= ~ARM_HWCAP2_SME_FA64 ;
 else if(!strcasecmp(CAV(w),"WFXT"    )) g_cpuFeatures2 &= ~ARM_HWCAP2_WFXT ;
#if defined(_WIN64)||defined(__LP64__)
 else if(!strcasecmp(CAV(w),"EBF16"   )) g_cpuFeatures2 &= ~ARM_HWCAP2_EBF16 ;
 else if(!strcasecmp(CAV(w),"SVE_EBF16" )) g_cpuFeatures2 &= ~ARM_HWCAP2_SVE_EBF16 ;
#endif
#elif defined(__x86_64__)||defined(__i386__)||defined(_M_X64)||defined(_M_IX86)
 if     (!strcasecmp(CAV(w),"SSSE3"   )) g_cpuFeatures &= ~CPU_X86_FEATURE_SSSE3 ;
 else if(!strcasecmp(CAV(w),"POPCNT"  )) g_cpuFeatures &= ~CPU_X86_FEATURE_POPCNT ;
 else if(!strcasecmp(CAV(w),"MOVBE"   )) g_cpuFeatures &= ~CPU_X86_FEATURE_MOVBE ;
 else if(!strcasecmp(CAV(w),"SSE4_1"  )) g_cpuFeatures &= ~CPU_X86_FEATURE_SSE4_1 ;
 else if(!strcasecmp(CAV(w),"SSE4_2"  )) g_cpuFeatures &= ~CPU_X86_FEATURE_SSE4_2 ;
 else if(!strcasecmp(CAV(w),"AES_NI"  )) g_cpuFeatures &= ~CPU_X86_FEATURE_AES_NI ;
 else if(!strcasecmp(CAV(w),"AVX"     )) g_cpuFeatures &= ~CPU_X86_FEATURE_AVX ;
 else if(!strcasecmp(CAV(w),"RDRAND"  )) g_cpuFeatures &= ~CPU_X86_FEATURE_RDRAND ;
 else if(!strcasecmp(CAV(w),"AVX2"    )) g_cpuFeatures &= ~CPU_X86_FEATURE_AVX2 ;
 else if(!strcasecmp(CAV(w),"SHA_NI"  )) g_cpuFeatures &= ~CPU_X86_FEATURE_SHA_NI ;
 else if(!strcasecmp(CAV(w),"FMA"     )) g_cpuFeatures &= ~CPU_X86_FEATURE_FMA ;
 else if(!strcasecmp(CAV(w),"RDSEED"  )) g_cpuFeatures &= ~CPU_X86_FEATURE_RDSEED ;
 else if(!strcasecmp(CAV(w),"BMI1"    )) g_cpuFeatures &= ~CPU_X86_FEATURE_BMI1 ;
 else if(!strcasecmp(CAV(w),"BMI2"    )) g_cpuFeatures &= ~CPU_X86_FEATURE_BMI2 ;
 else if(!strcasecmp(CAV(w),"AVX512F" )) g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512F ;
 else if(!strcasecmp(CAV(w),"AVX512VL" )) g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512VL ;
 else if(!strcasecmp(CAV(w),"AVX512DQ" )) g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512DQ ;
 else if(!strcasecmp(CAV(w),"AVX512BW" )) g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512BW ;
 else if(!strcasecmp(CAV(w),"AVX512IFMA" )) g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512IFMA ;
 else if(!strcasecmp(CAV(w),"AVX512VBMI" )) g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512VBMI ;
 else if(!strcasecmp(CAV(w),"AVX512VBMI2" )) g_cpuFeatures &= ~CPU_X86_FEATURE_AVX512VBMI2 ;
 else if(!strcasecmp(CAV(w),"RING3MWAIT" )) g_cpuFeatures2 &= ~CPU_X86_FEATURE2_RING3MWAIT ;
 else if(!strcasecmp(CAV(w),"FSGSBASE" )) g_cpuFeatures2 &= ~CPU_X86_FEATURE2_FSGSBASE ;
#endif
}
OPENSSL_setcap(getCpuFeatures());
#if defined(__x86_64__)
 hwfma=(getCpuFeatures()&CPU_X86_FEATURE_FMA)?1:0;
#endif
 if(!strcasecmp(CAV(w),"CBLAS"   ))  hascblas= k&&libcblas;
#if defined(_OPENMP)
 if(!strcasecmp(CAV(w),"OPENMP"  ))  hasopenmp= !!k;
#endif
R mtm;
}

