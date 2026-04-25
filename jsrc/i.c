/* Copyright (c) 1990-2026, Jsoftware Inc.  All rights reserved.           */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Initializations                                                         */

#ifdef _WIN32
#define __iamcu__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#define __cdecl
#include <dlfcn.h>
#endif
#include "j.h"
#include "w.h"
#include "cpuinfo.h"
extern char hascblas;

#if SYS & SYS_FREEBSD
#include <floatingpoint.h>
#endif


// create name block for xyuvmn.  flags is type flags to use, either 0 or NAMEBYVALUE
static A jtmakename(J jt,C*s,I flags){A z;I m;NM*zv;
 GATV0(z,NAME,1,1); AT(z)=NAME|flags; zv=NAV(z); z->mback.lookaside=0;  // Use GATV because GA doesn't support NAME type; but we must have NAMEBYVALUE set
 zv->s[0]=s[0];  // copy name character
 zv->n=zv->m=(UC)1;
 zv->bucket=0;
 zv->bucketx=0;
 zv->flag=NMMNUVXY;   // NMMNUVXY indicates mnuvxy
 zv->hash=(UI4)nmhash(1,(UC*)&zv->s);  // use copy because hic may overread
 ACX(z);  // set PERMANENT
 RETF (z);
}

/* 
JE can be used by multiple tasks and threads
tasks are easier than threads
global memory is distinct between tasks but is shared between threads

JE support for multiple tasks is good

 global storage that changes after globinit is a bad bug waiting to happen

storage belonging to a task or thread MUST be be in the J structure
there are only a few globals that have storage not in J
*/

// globals 
JS gjt=0; // JPF debug - convenience debug single process - points to shared area bacause sto() uses it

// thread-safe/one-time initialization of all global constants
// Use GA for all these initializations, to save space since they're done only once
// The call gives us jt, but we initialize in the master thread
// The jt we are given is a throwaway, needed ONLY so we can allocate some A blocks here.  Anything stored
// into jt will never be used.  jinit3 will later be called with the real jt, to initialize it
B jtglobinit(JS jjt){A x,y;J jt=MTHREAD(jjt);  // initialize in master thread
/* JCON0 */
/*
 printf("ACINPLACE              "FMT0X" \n",(I)ACINPLACE);
 printf("ACINPLACEX             %#010x \n",(UI4)ACINPLACEX);
 printf("ACPERMANENT            "FMT0X" \n",(I)ACPERMANENT);
 printf("ACPERMANENTX           %#010x \n",(UI4)ACPERMANENTX);
 printf("ACUC1                  %#010x \n",(UI4)ACUC1);
 printf("ACUC2                  %#010x \n",(UI4)ACUC2);
 printf("ACUSECOUNT             %#010x \n",(UI4)ACUSECOUNT);
 printf("ADV                    %#010x \n",(UI4)ADV);
 printf("ADVSIZE                %#010x \n",(UI4)ADVSIZE);
 printf("ADVX                   %#010x \n",(UI4)ADVX);
 printf("AFANCHORED             %#010x \n",(UI4)AFANCHORED);
 printf("AFANCHOREDX            %#010x \n",(UI4)AFANCHOREDX);
 printf("AFAUDITUC              "FMT0X" \n",(I)AFAUDITUC);
 printf("AFAUDITUCX             %#010x \n",(UI4)AFAUDITUCX);
 printf("AFDEBUGRESULT          %#010x \n",(UI4)AFDEBUGRESULT);
 printf("AFDEBUGRESULTX         %#010x \n",(UI4)AFDEBUGRESULTX);
 printf("AFDPAREN               %#010x \n",(UI4)AFDPAREN);
 printf("AFDPARENX              %#010x \n",(UI4)AFDPARENX);
 printf("AFGERUNDOK             %#010x \n",(UI4)AFGERUNDOK);
 printf("AFGERUNDOKX            %#010x \n",(UI4)AFGERUNDOKX);
 printf("AFKNOWNNAMED           %#010x \n",(UI4)AFKNOWNNAMED);
 printf("AFKNOWNNAMEDX          %#010x \n",(UI4)AFKNOWNNAMEDX);
 printf("AFNJA                  %#010x \n",(UI4)AFNJA);
 printf("AFNJAX                 %#010x \n",(UI4)AFNJAX);
 printf("AFPRISTINE             %#010x \n",(UI4)AFPRISTINE);
 printf("AFPRISTINEX            %#010x \n",(UI4)AFPRISTINEX);
 printf("AFRO                   %#010x \n",(UI4)AFRO);
 printf("AFROX                  %#010x \n",(UI4)AFROX);
 printf("AFSENTENCEWORD         %#010x \n",(UI4)AFSENTENCEWORD);
 printf("AFSENTENCEWORDX        %#010x \n",(UI4)AFSENTENCEWORDX);
 printf("AFUNAUDITABLE          %#010x \n",(UI4)AFUNAUDITABLE);
 printf("AFUNAUDITABLEX         %#010x \n",(UI4)AFUNAUDITABLEX);
 printf("AFUNIFORMITEMS         %#010x \n",(UI4)AFUNIFORMITEMS);
 printf("AFUNIFORMITEMSX        %#010x \n",(UI4)AFUNIFORMITEMSX);
 printf("AFUNINCORPABLE         %#010x \n",(UI4)AFUNINCORPABLE);
 printf("AFUNINCORPABLEX        %#010x \n",(UI4)AFUNINCORPABLEX);
 printf("AFUPPERTRI             %#010x \n",(UI4)AFUPPERTRI);
 printf("AFUPPERTRIX            %#010x \n",(UI4)AFUPPERTRIX);
 printf("AFVIRTUAL              %#010x \n",(UI4)AFVIRTUAL);
 printf("AFVIRTUALBOXED         %#010x \n",(UI4)AFVIRTUALBOXED);
 printf("AFVIRTUALBOXEDX        %#010x \n",(UI4)AFVIRTUALBOXEDX);
 printf("AFVIRTUALX             %#010x \n",(UI4)AFVIRTUALX);
 printf("AMOFFSET               %#010x \n",(UI4)AMOFFSET);
 printf("ANY                    %#010x \n",(UI4)ANY);
 printf("ARHASACV               %#010x \n",(UI4)ARHASACV);
 printf("ARHASACVX              %#010x \n",(UI4)ARHASACVX);
 printf("ARINVALID              %#010x \n",(UI4)ARINVALID);
 printf("ARLCLONED              %#010x \n",(UI4)ARLCLONED);
 printf("ARLCLONEDX             %#010x \n",(UI4)ARLCLONEDX);
 printf("ARLOCALTABLE           %#010x \n",(UI4)ARLOCALTABLE);
 printf("ARLOCALTABLEX          %#010x \n",(UI4)ARLOCALTABLEX);
 printf("ARLSYMINUSE            %#010x \n",(UI4)ARLSYMINUSE);
 printf("ARNAMEADDED            %#010x \n",(UI4)ARNAMEADDED);
 printf("ARNAMEADDEDX           %#010x \n",(UI4)ARNAMEADDEDX);
 printf("ARNAMED                %#010x \n",(UI4)ARNAMED);
 printf("ARNAMEDX               %#010x \n",(UI4)ARNAMEDX);
 printf("AROFFSET               %#010x \n",(UI4)AROFFSET);
 printf("ASGN                   %#010x \n",(UI4)ASGN);
 printf("ASGNLOCAL              %#010x \n",(UI4)ASGNLOCAL);
 printf("ASGNLOCALX             %#010x \n",(UI4)ASGNLOCALX);
 printf("ASGNSIZE               %#010x \n",(UI4)ASGNSIZE);
 printf("ASGNTONAME             %#010x \n",(UI4)ASGNTONAME);
 printf("ASGNX                  %#010x \n",(UI4)ASGNX);
 printf("B01                    %#010x \n",(UI4)B01);
 printf("B01SIZE                %#010x \n",(UI4)B01SIZE);
 printf("B01X                   %#010x \n",(UI4)B01X);
 printf("BOX                    %#010x \n",(UI4)BOX);
 printf("BOXMULTIASSIGN         %#010x \n",(UI4)BOXMULTIASSIGN);
 printf("BOXSIZE                %#010x \n",(UI4)BOXSIZE);
 printf("BOXX                   %#010x \n",(UI4)BOXX);
 printf("C2T                    %#010x \n",(UI4)C2T);
 printf("C2TEXTTYPEX            %#010x \n",(UI4)C2TEXTTYPEX);
 printf("C2TSIZE                %#010x \n",(UI4)C2TSIZE);
 printf("C2TX                   %#010x \n",(UI4)C2TX);
 printf("C4T                    %#010x \n",(UI4)C4T);
 printf("C4TEXTTYPEX            %#010x \n",(UI4)C4TEXTTYPEX);
 printf("C4TSIZE                %#010x \n",(UI4)C4TSIZE);
 printf("C4TX                   %#010x \n",(UI4)C4TX);
 printf("CMPX                   %#010x \n",(UI4)CMPX);
 printf("CMPXSIZE               %#010x \n",(UI4)CMPXSIZE);
 printf("CMPXX                  %#010x \n",(UI4)CMPXX);
 printf("CONJ                   %#010x \n",(UI4)CONJ);
 printf("CONJSIZE               %#010x \n",(UI4)CONJSIZE);
 printf("CONJX                  %#010x \n",(UI4)CONJX);
 printf("CONW                   %#010x \n",(UI4)CONW);
 printf("CONWSIZE               %#010x \n",(UI4)CONWSIZE);
 printf("CONWX                  %#010x \n",(UI4)CONWX);
 printf("CVTNOFUZZ              %#010x \n",(UI4)CVTNOFUZZ);
 printf("CVTNOFUZZX             %#010x \n",(UI4)CVTNOFUZZX);
 printf("DCCALL                 %#010x \n",(UI4)DCCALL);
 printf("DCJUNK                 %#010x \n",(UI4)DCJUNK);
 printf("DCPARSE                %#010x \n",(UI4)DCPARSE);
 printf("DCPM                   %#010x \n",(UI4)DCPM);
 printf("DCSCRIPT               %#010x \n",(UI4)DCSCRIPT);
 printf("DIRECT                 %#010x \n",(UI4)DIRECT);
 printf("EXACTNUMERIC           %#010x \n",(UI4)EXACTNUMERIC);
 printf("EXECCTNOTDELD          %#010x \n",(UI4)EXECCTNOTDELD);
 printf("EXECCTPERM             %#010x \n",(UI4)EXECCTPERM);
 printf("EXECCTPERMX            %#010x \n",(UI4)EXECCTPERMX);
 printf("FIXAFCOLOCATIVE        %#010x \n",(UI4)FIXAFCOLOCATIVE);
 printf("FIXAFCOPUBLIC          %#010x \n",(UI4)FIXAFCOPUBLIC);
 printf("FIXAFCORECUR           %#010x \n",(UI4)FIXAFCORECUR);
 printf("FIXAFCOX               %#010x \n",(UI4)FIXAFCOX);
 printf("FIXAFINTX              %#010x \n",(UI4)FIXAFINTX);
 printf("FIXAFIRSTIMPLOCONLY    %#010x \n",(UI4)FIXAFIRSTIMPLOCONLY);
 printf("FIXASTOPATINV          %#010x \n",(UI4)FIXASTOPATINV);
 printf("FL                     %#010x \n",(UI4)FL);
 printf("FLSIZE                 %#010x \n",(UI4)FLSIZE);
 printf("FLX                    %#010x \n",(UI4)FLX);
 printf("FUNC                   %#010x \n",(UI4)FUNC);
 printf("HP                     %#010x \n",(UI4)HP);
 printf("HPEXTTYPE              %#010x \n",(UI4)HPEXTTYPE);
 printf("HPSIZE                 %#010x \n",(UI4)HPSIZE);
 printf("HPX                    %#010x \n",(UI4)HPX);
 printf("INT                    %#010x \n",(UI4)INT);
 printf("INT1                   %#010x \n",(UI4)INT1);
 printf("INT1EXTTYPE            %#010x \n",(UI4)INT1EXTTYPE);
 printf("INT1SIZE               %#010x \n",(UI4)INT1SIZE);
 printf("INT1X                  %#010x \n",(UI4)INT1X);
 printf("INT2                   %#010x \n",(UI4)INT2);
 printf("INT2EXTTYPE            %#010x \n",(UI4)INT2EXTTYPE);
 printf("INT2SIZE               %#010x \n",(UI4)INT2SIZE);
 printf("INT2X                  %#010x \n",(UI4)INT2X);
 printf("INT4                   %#010x \n",(UI4)INT4);
 printf("INT4EXTTYPE            %#010x \n",(UI4)INT4EXTTYPE);
 printf("INT4SIZE               %#010x \n",(UI4)INT4SIZE);
 printf("INT4X                  %#010x \n",(UI4)INT4X);
 printf("INTSIZE                %#010x \n",(UI4)INTSIZE);
 printf("INTX                   %#010x \n",(UI4)INTX);
 printf("IS1BYTE                %#010x \n",(UI4)IS1BYTE);
 printf("JCHAR                  %#010x \n",(UI4)JCHAR);
 printf("JTALIGNBDY             %#010x \n",(UI4)JTALIGNBDY);
 printf("JTALLOWRETARG          %#010x \n",(UI4)JTALLOWRETARG);
 printf("JTALLOWRETARGX         %#010x \n",(UI4)JTALLOWRETARGX);
 printf("JTASGNWASLOCAL         %#010x \n",(UI4)JTASGNWASLOCAL);
 printf("JTASGNWASLOCALX        %#010x \n",(UI4)JTASGNWASLOCALX);
 printf("JTCOUNTITEMS           %#010x \n",(UI4)JTCOUNTITEMS);
 printf("JTCOUNTITEMSX          %#010x \n",(UI4)JTCOUNTITEMSX);
 printf("JTDESCEND              %#010x \n",(UI4)JTDESCEND);
 printf("JTDESCENDX             %#010x \n",(UI4)JTDESCENDX);
 printf("JTDOWHILE              %#010x \n",(UI4)JTDOWHILE);
 printf("JTDOWHILEX             %#010x \n",(UI4)JTDOWHILEX);
 printf("JTEXPVALENCEOFF        %#010x \n",(UI4)JTEXPVALENCEOFF);
 printf("JTEXPVALENCEOFFD       %#010x \n",(UI4)JTEXPVALENCEOFFD);
 printf("JTEXPVALENCEOFFM       %#010x \n",(UI4)JTEXPVALENCEOFFM);
 printf("JTEXPVALENCEOFFX       %#010x \n",(UI4)JTEXPVALENCEOFFX);
 printf("JTFINALASGN            %#010x \n",(UI4)JTFINALASGN);
 printf("JTFINALASGNX           %#010x \n",(UI4)JTFINALASGNX);
 printf("JTFLAGMSK              %#010x \n",(UI4)JTFLAGMSK);
 printf("JTFOLKNOHFN            %#010x \n",(UI4)JTFOLKNOHFN);
 printf("JTFOLKNOHFNX           %#010x \n",(UI4)JTFOLKNOHFNX);
 printf("JTFROMEXEC             %#010x \n",(UI4)JTFROMEXEC);
 printf("JTFROMEXECX            %#010x \n",(UI4)JTFROMEXECX);
 printf("JTINPLACEA             %#010x \n",(UI4)JTINPLACEA);
 printf("JTINPLACEAX            %#010x \n",(UI4)JTINPLACEAX);
 printf("JTINPLACEW             %#010x \n",(UI4)JTINPLACEW);
 printf("JTINPLACEWX            %#010x \n",(UI4)JTINPLACEWX);
 printf("JTNOFILL               %#010x \n",(UI4)JTNOFILL);
 printf("JTNOFILLX              %#010x \n",(UI4)JTNOFILLX);
 printf("JTNORESETERR           %#010x \n",(UI4)JTNORESETERR);
 printf("JTNORESETERRX          %#010x \n",(UI4)JTNORESETERRX);
 printf("JTNOTMATCH             %#010x \n",(UI4)JTNOTMATCH);
 printf("JTNOTMATCHX            %#010x \n",(UI4)JTNOTMATCHX);
 printf("JTOUTERPARENS          %#010x \n",(UI4)JTOUTERPARENS);
 printf("JTOUTERPARENSX         %#010x \n",(UI4)JTOUTERPARENSX);
 printf("JTPARENS               %#010x \n",(UI4)JTPARENS);
 printf("JTPARENSX              %#010x \n",(UI4)JTPARENSX);
 printf("JTPRFORSCREEN          %#010x \n",(UI4)JTPRFORSCREEN);
 printf("JTPRFORSCREENX         %#010x \n",(UI4)JTPRFORSCREENX);
 printf("JTPRNOSTDOUT           %#010x \n",(UI4)JTPRNOSTDOUT);
 printf("JTPRNOSTDOUTX          %#010x \n",(UI4)JTPRNOSTDOUTX);
 printf("JTPRTYO                %#010x \n",(UI4)JTPRTYO);
 printf("JTRETRY                %#010x \n",(UI4)JTRETRY);
 printf("JTRETRYX               %#010x \n",(UI4)JTRETRYX);
 printf("JTSPARSEARG            %#010x \n",(UI4)JTSPARSEARG);
 printf("JTSPARSEARGX           %#010x \n",(UI4)JTSPARSEARGX);
 printf("JTTHORNX               %#010x \n",(UI4)JTTHORNX);
 printf("JTTHORNXX              %#010x \n",(UI4)JTTHORNXX);
 printf("JTTHORNY               %#010x \n",(UI4)JTTHORNY);
 printf("JTTHORNYX              %#010x \n",(UI4)JTTHORNYX);
 printf("JTWILLBEOPENED         %#010x \n",(UI4)JTWILLBEOPENED);
 printf("JTWILLBEOPENEDX        %#010x \n",(UI4)JTWILLBEOPENEDX);
 printf("JTWILLBEREALIZED       %#010x \n",(UI4)JTWILLBEREALIZED);
 printf("JTWILLBEREALIZEDX      %#010x \n",(UI4)JTWILLBEREALIZEDX);
 printf("JTXDEFMODIFIER         %#010x \n",(UI4)JTXDEFMODIFIER);
 printf("JTXDEFMODIFIERX        %#010x \n",(UI4)JTXDEFMODIFIERX);
 printf("LASTNOUNX              %#010x \n",(UI4)LASTNOUNX);
 printf("LCH                    %#010x \n",(UI4)LCH);
 printf("LGRANK2TX              %#010x \n",(UI4)LGRANK2TX);
 printf("LGRANKTX               %#010x \n",(UI4)LGRANKTX);
 printf("LGRMAX                 %#010x \n",(UI4)LGRMAX);
 printf("LHASNAME               %#010x \n",(UI4)LHASNAME);
 printf("LHASVALUE              %#010x \n",(UI4)LHASVALUE);
 printf("LINFO                  %#010x \n",(UI4)LINFO);
 printf("LIT                    %#010x \n",(UI4)LIT);
 printf("LITSIZE                %#010x \n",(UI4)LITSIZE);
 printf("LITX                   %#010x \n",(UI4)LITX);
 printf("LPAR                   %#010x \n",(UI4)LPAR);
 printf("LPARSIZE               %#010x \n",(UI4)LPARSIZE);
 printf("LPARX                  %#010x \n",(UI4)LPARX);
 printf("LPERMANENT             %#010x \n",(UI4)LPERMANENT);
 printf("LPERMANENTX            %#010x \n",(UI4)LPERMANENTX);
 printf("LREADONLY              %#010x \n",(UI4)LREADONLY);
 printf("LWASABANDONED          %#010x \n",(UI4)LWASABANDONED);
 printf("LWASABANDONEDX         %#010x \n",(UI4)LWASABANDONEDX);
 printf("MARK                   %#010x \n",(UI4)MARK);
 printf("MARKSIZE               %#010x \n",(UI4)MARKSIZE);
 printf("MARKX                  %#010x \n",(UI4)MARKX);
 printf("MEMSET00LEN            %#010x \n",(UI4)MEMSET00LEN);
 printf("MEMSET01LEN            %#010x \n",(UI4)MEMSET01LEN);
 printf("MEMSETFFLEN            %#010x \n",(UI4)MEMSETFFLEN);
 printf("NAME                   %#010x \n",(UI4)NAME);
 printf("NAMEABANDON            %#010x \n",(UI4)NAMEABANDON);
 printf("NAMEABANDONX           %#010x \n",(UI4)NAMEABANDONX);
 printf("NAMEBYVALUE            %#010x \n",(UI4)NAMEBYVALUE);
 printf("NAMEBYVALUEX           %#010x \n",(UI4)NAMEBYVALUEX);
 printf("NAMESIZE               %#010x \n",(UI4)NAMESIZE);
 printf("NAMEX                  %#010x \n",(UI4)NAMEX);
 printf("NMCACHED               %#010x \n",(UI4)NMCACHED);
 printf("NMCACHEDX              %#010x \n",(UI4)NMCACHEDX);
 printf("NMILOC                 %#010x \n",(UI4)NMILOC);
 printf("NMIMPLOC               %#010x \n",(UI4)NMIMPLOC);
 printf("NMLOC                  %#010x \n",(UI4)NMLOC);
 printf("NMMNUVXY               %#010x \n",(UI4)NMMNUVXY);
 printf("NMSHARED               %#010x \n",(UI4)NMSHARED);
 printf("NMSHAREDX              %#010x \n",(UI4)NMSHAREDX);
 printf("NOUN                   %#010x \n",(UI4)NOUN);
 printf("NUMERIC                %#010x \n",(UI4)NUMERIC);
 printf("PMCOL                  %#010x \n",(UI4)PMCOL);
 printf("PYX                    %#010x \n",(UI4)PYX);
 printf("PYXX                   %#010x \n",(UI4)PYXX);
 printf("QCADV                  %#010x \n",(UI4)QCADV);
 printf("QCASGN                 %#010x \n",(UI4)QCASGN);
 printf("QCASGNISLOCAL          %#010x \n",(UI4)QCASGNISLOCAL);
 printf("QCASGNISTONAME         %#010x \n",(UI4)QCASGNISTONAME);
 printf("QCCONJ                 %#010x \n",(UI4)QCCONJ);
 printf("QCFAOWED               %#010x \n",(UI4)QCFAOWED);
 printf("QCFAOWEDMSK            %#010x \n",(UI4)QCFAOWEDMSK);
 printf("QCFAOWEDX              %#010x \n",(UI4)QCFAOWEDX);
 printf("QCISLKPNAME            %#010x \n",(UI4)QCISLKPNAME);
 printf("QCLPAR                 %#010x \n",(UI4)QCLPAR);
 printf("QCMASK                 %#010x \n",(UI4)QCMASK);
 printf("QCNAMEABANDON          %#010x \n",(UI4)QCNAMEABANDON);
 printf("QCNAMEASSIGNED         %#010x \n",(UI4)QCNAMEASSIGNED);
 printf("QCNAMEBYVALUE          %#010x \n",(UI4)QCNAMEBYVALUE);
 printf("QCNAMED                %#010x \n",(UI4)QCNAMED);
 printf("QCNAMEDLOC             %#010x \n",(UI4)QCNAMEDLOC);
 printf("QCNAMEDLOCX            %#010x \n",(UI4)QCNAMEDLOCX);
 printf("QCNAMEDX               %#010x \n",(UI4)QCNAMEDX);
 printf("QCNOUN                 %#010x \n",(UI4)QCNOUN);
 printf("QCNOUNX                %#010x \n",(UI4)QCNOUNX);
 printf("QCRAREQD               %#010x \n",(UI4)QCRAREQD);
 printf("QCRAREQDX              %#010x \n",(UI4)QCRAREQDX);
 printf("QCRPAR                 %#010x \n",(UI4)QCRPAR);
 printf("QCVERB                 %#010x \n",(UI4)QCVERB);
 printf("QP                     %#010x \n",(UI4)QP);
 printf("QPEXTTYPE              %#010x \n",(UI4)QPEXTTYPE);
 printf("QPSIZE                 %#010x \n",(UI4)QPSIZE);
 printf("QPX                    %#010x \n",(UI4)QPX);
 printf("R2MAX                  %#010x \n",(UI4)R2MAX);
 printf("RANK2TMSK              %#010x \n",(UI4)RANK2TMSK);
 printf("RANK2TX                %#010x \n",(UI4)RANK2TX);
 printf("RANKTMSK               %#010x \n",(UI4)RANKTMSK);
 printf("RANKTX                 %#010x \n",(UI4)RANKTX);
 printf("RAT                    %#010x \n",(UI4)RAT);
 printf("RATSIZE                %#010x \n",(UI4)RATSIZE);
 printf("RCALIGN                %#010x \n",(UI4)RCALIGN);
 printf("RATX                   %#010x \n",(UI4)RATX);
 printf("RECURSIBLE             %#010x \n",(UI4)RECURSIBLE);
 printf("RECURSIBLENOUN         %#010x \n",(UI4)RECURSIBLENOUN);
 printf("RHS                    %#010x \n",(UI4)RHS);
 printf("RMAX                   %#010x \n",(UI4)RMAX);
 printf("RPAR                   %#010x \n",(UI4)RPAR);
 printf("RPARSIZE               %#010x \n",(UI4)RPARSIZE);
 printf("RPARX                  %#010x \n",(UI4)RPARX);
 printf("SFNSIMPLEONLY          %#010x \n",(UI4)SFNSIMPLEONLY);
 printf("SP                     %#010x \n",(UI4)SP);
 printf("SPARSABLE              %#010x \n",(UI4)SPARSABLE);
 printf("SPARSE                 %#010x \n",(UI4)SPARSE);
 printf("SPARSEX                %#010x \n",(UI4)SPARSEX);
 printf("SPEXTTYPE              %#010x \n",(UI4)SPEXTTYPE);
 printf("SPSIZE                 %#010x \n",(UI4)SPSIZE);
 printf("SPX                    %#010x \n",(UI4)SPX);
 printf("STKFAOWED              %#010x \n",(UI4)STKFAOWED);
 printf("STKFAOWEDX             %#010x \n",(UI4)STKFAOWEDX);
 printf("STKNAMED               %#010x \n",(UI4)STKNAMED);
 printf("STKNAMEDMSK            %#010x \n",(UI4)STKNAMEDMSK);
 printf("STKNAMEDX              %#010x \n",(UI4)STKNAMEDX);
 printf("STKREFRESHRQD          %#010x \n",(UI4)STKREFRESHRQD);
 printf("STKREFRESHRQDX         %#010x \n",(UI4)STKREFRESHRQDX);
 printf("SYMB                   %#010x \n",(UI4)SYMB);
 printf("SYMBSIZE               %#010x \n",(UI4)SYMBSIZE);
 printf("SYMBX                  %#010x \n",(UI4)SYMBX);
 printf("SYMLEXECCT             %#010x \n",(UI4)SYMLEXECCT);
 printf("SYMLINFO               %#010x \n",(UI4)SYMLINFO);
 printf("SYMLINFOSIZE           %#010x \n",(UI4)SYMLINFOSIZE);
 printf("SYMNONPERM             %#010x \n",(UI4)SYMNONPERM);
 printf("SYMNONPERMX            %#010x \n",(UI4)SYMNONPERMX);
 printf("TCESXCECAN             %#010x \n",(UI4)TCESXCECAN);
 printf("TCESXCECANT            %#010x \n",(UI4)TCESXCECANT);
 printf("TCESXCECANTX           %#010x \n",(UI4)TCESXCECANTX);
 printf("TCESXCECANX            %#010x \n",(UI4)TCESXCECANX);
 printf("TCESXCEX               %#010x \n",(UI4)TCESXCEX);
 printf("TCESXSXMSK             %#010x \n",(UI4)TCESXSXMSK);
 printf("TCESXTYPE              %#010x \n",(UI4)TCESXTYPE);
 printf("TCESXTYPEX             %#010x \n",(UI4)TCESXTYPEX);
 printf("TRAVERSIBLE            %#010x \n",(UI4)TRAVERSIBLE);
 printf("TYPEVIPOK              %#010x \n",(UI4)TYPEVIPOK);
 printf("VALTYPENAMELESS        %#010x \n",(UI4)VALTYPENAMELESS);
 printf("VALTYPESPARSE          %#010x \n",(UI4)VALTYPESPARSE);
 printf("VCEIL                  %#010x \n",(UI4)VCEIL);
 printf("VDDHASCTL              %#010x \n",(UI4)VDDHASCTL);
 printf("VERB                   %#010x \n",(UI4)VERB);
 printf("VERBSIZE               %#010x \n",(UI4)VERBSIZE);
 printf("VERBX                  %#010x \n",(UI4)VERBX);
 printf("VF2ATOPOPEN1           %#010x \n",(UI4)VF2ATOPOPEN1);
 printf("VF2ATOPOPEN1X          %#010x \n",(UI4)VF2ATOPOPEN1X);
 printf("VF2ATOPOPEN2A          %#010x \n",(UI4)VF2ATOPOPEN2A);
 printf("VF2ATOPOPEN2AX         %#010x \n",(UI4)VF2ATOPOPEN2AX);
 printf("VF2ATOPOPEN2W          %#010x \n",(UI4)VF2ATOPOPEN2W);
 printf("VF2ATOPOPEN2WX         %#010x \n",(UI4)VF2ATOPOPEN2WX);
 printf("VF2BOXATOP1            %#010x \n",(UI4)VF2BOXATOP1);
 printf("VF2BOXATOP1X           %#010x \n",(UI4)VF2BOXATOP1X);
 printf("VF2BOXATOP2            %#010x \n",(UI4)VF2BOXATOP2);
 printf("VF2BOXATOP2X           %#010x \n",(UI4)VF2BOXATOP2X);
 printf("VF2CACHEABLE           %#010x \n",(UI4)VF2CACHEABLE);
 printf("VF2CACHEABLEX          %#010x \n",(UI4)VF2CACHEABLEX);
 printf("VF2CACHED              %#010x \n",(UI4)VF2CACHED);
 printf("VF2CACHEDX             %#010x \n",(UI4)VF2CACHEDX);
 printf("VF2IMPLOC              %#010x \n",(UI4)VF2IMPLOC);
 printf("VF2IMPLOCX             %#010x \n",(UI4)VF2IMPLOCX);
 printf("VF2NAMELESS            %#010x \n",(UI4)VF2NAMELESS);
 printf("VF2NAMELESSX           %#010x \n",(UI4)VF2NAMELESSX);
 printf("VF2NONE                %#010x \n",(UI4)VF2NONE);
 printf("VF2PRIM                %#010x \n",(UI4)VF2PRIM);
 printf("VF2PRIMX               %#010x \n",(UI4)VF2PRIMX);
 printf("VF2PSEUDONAME          %#010x \n",(UI4)VF2PSEUDONAME);
 printf("VF2PSEUDONAMEX         %#010x \n",(UI4)VF2PSEUDONAMEX);
 printf("VF2RANKATOP1           %#010x \n",(UI4)VF2RANKATOP1);
 printf("VF2RANKATOP1X          %#010x \n",(UI4)VF2RANKATOP1X);
 printf("VF2RANKATOP2           %#010x \n",(UI4)VF2RANKATOP2);
 printf("VF2RANKATOP2X          %#010x \n",(UI4)VF2RANKATOP2X);
 printf("VF2RANKONLY1           %#010x \n",(UI4)VF2RANKONLY1);
 printf("VF2RANKONLY1X          %#010x \n",(UI4)VF2RANKONLY1X);
 printf("VF2RANKONLY2           %#010x \n",(UI4)VF2RANKONLY2);
 printf("VF2RANKONLY2X          %#010x \n",(UI4)VF2RANKONLY2X);
 printf("VF2USESITEMCOUNT1      %#010x \n",(UI4)VF2USESITEMCOUNT1);
 printf("VF2USESITEMCOUNT1X     %#010x \n",(UI4)VF2USESITEMCOUNT1X);
 printf("VF2USESITEMCOUNT2A     %#010x \n",(UI4)VF2USESITEMCOUNT2A);
 printf("VF2USESITEMCOUNT2AX    %#010x \n",(UI4)VF2USESITEMCOUNT2AX);
 printf("VF2USESITEMCOUNT2W     %#010x \n",(UI4)VF2USESITEMCOUNT2W);
 printf("VF2USESITEMCOUNT2WX    %#010x \n",(UI4)VF2USESITEMCOUNT2WX);
 printf("VF2WILLOPEN1           %#010x \n",(UI4)VF2WILLOPEN1);
 printf("VF2WILLOPEN1PROP       %#010x \n",(UI4)VF2WILLOPEN1PROP);
 printf("VF2WILLOPEN1PROPX      %#010x \n",(UI4)VF2WILLOPEN1PROPX);
 printf("VF2WILLOPEN1X          %#010x \n",(UI4)VF2WILLOPEN1X);
 printf("VF2WILLOPEN2A          %#010x \n",(UI4)VF2WILLOPEN2A);
 printf("VF2WILLOPEN2APROP      %#010x \n",(UI4)VF2WILLOPEN2APROP);
 printf("VF2WILLOPEN2APROPX     %#010x \n",(UI4)VF2WILLOPEN2APROPX);
 printf("VF2WILLOPEN2AX         %#010x \n",(UI4)VF2WILLOPEN2AX);
 printf("VF2WILLOPEN2W          %#010x \n",(UI4)VF2WILLOPEN2W);
 printf("VF2WILLOPEN2WPROP      %#010x \n",(UI4)VF2WILLOPEN2WPROP);
 printf("VF2WILLOPEN2WPROPX     %#010x \n",(UI4)VF2WILLOPEN2WPROPX);
 printf("VF2WILLOPEN2WX         %#010x \n",(UI4)VF2WILLOPEN2WX);
 printf("VF2WILLREALIZE1        %#010x \n",(UI4)VF2WILLREALIZE1);
 printf("VF2WILLREALIZE1PROP    %#010x \n",(UI4)VF2WILLREALIZE1PROP);
 printf("VF2WILLREALIZE1PROPX   %#010x \n",(UI4)VF2WILLREALIZE1PROPX);
 printf("VF2WILLREALIZE1X       %#010x \n",(UI4)VF2WILLREALIZE1X);
 printf("VF2WILLREALIZE2A       %#010x \n",(UI4)VF2WILLREALIZE2A);
 printf("VF2WILLREALIZE2APROP   %#010x \n",(UI4)VF2WILLREALIZE2APROP);
 printf("VF2WILLREALIZE2APROPX  %#010x \n",(UI4)VF2WILLREALIZE2APROPX);
 printf("VF2WILLREALIZE2AX      %#010x \n",(UI4)VF2WILLREALIZE2AX);
 printf("VF2WILLREALIZE2W       %#010x \n",(UI4)VF2WILLREALIZE2W);
 printf("VF2WILLREALIZE2WPROP   %#010x \n",(UI4)VF2WILLREALIZE2WPROP);
 printf("VF2WILLREALIZE2WPROPX  %#010x \n",(UI4)VF2WILLREALIZE2WPROPX);
 printf("VF2WILLREALIZE2WX      %#010x \n",(UI4)VF2WILLREALIZE2WX);
 printf("VFATOPL                %#010x \n",(UI4)VFATOPL);
 printf("VFATOPPOLY             %#010x \n",(UI4)VFATOPPOLY);
 printf("VFATOPPOLYEXP          %#010x \n",(UI4)VFATOPPOLYEXP);
 printf("VFATOPPOLYNONE         %#010x \n",(UI4)VFATOPPOLYNONE);
 printf("VFATOPPOLYX            %#010x \n",(UI4)VFATOPPOLYX);
 printf("VFATOPR                %#010x \n",(UI4)VFATOPR);
 printf("VFCOMPCOMP             %#010x \n",(UI4)VFCOMPCOMP);
 printf("VFHKLVLDEC             %#010x \n",(UI4)VFHKLVLDEC);
 printf("VFHKLVLDECX            %#010x \n",(UI4)VFHKLVLDECX);
 printf("VFHKLVLGT              %#010x \n",(UI4)VFHKLVLGT);
 printf("VFHKLVLGTX             %#010x \n",(UI4)VFHKLVLGTX);
 printf("VFKEYSLASHF            %#010x \n",(UI4)VFKEYSLASHF);
 printf("VFKEYSLASHFX           %#010x \n",(UI4)VFKEYSLASHFX);
 printf("VFKEYSLASHT            %#010x \n",(UI4)VFKEYSLASHT);
 printf("VFKEYSLASHTX           %#010x \n",(UI4)VFKEYSLASHTX);
 printf("VFLAGNONE              %#010x \n",(UI4)VFLAGNONE);
 printf("VFLR                   %#010x \n",(UI4)VFLR);
 printf("VFSCANIRS              %#010x \n",(UI4)VFSCANIRS);
 printf("VFSCANIRSX             %#010x \n",(UI4)VFSCANIRSX);
 printf("VFUNDERHASINV          %#010x \n",(UI4)VFUNDERHASINV);
 printf("VFUNDERHASINVX         %#010x \n",(UI4)VFUNDERHASINVX);
 printf("VFUSEDOK2              %#010x \n",(UI4)VFUSEDOK2);
 printf("VGERL                  %#010x \n",(UI4)VGERL);
 printf("VGERLX                 %#010x \n",(UI4)VGERLX);
 printf("VGERR                  %#010x \n",(UI4)VGERR);
 printf("VIRS1                  %#010x \n",(UI4)VIRS1);
 printf("VIRS1X                 %#010x \n",(UI4)VIRS1X);
 printf("VIRS2                  %#010x \n",(UI4)VIRS2);
 printf("VIRS2X                 %#010x \n",(UI4)VIRS2X);
 printf("VISATOMIC1             %#010x \n",(UI4)VISATOMIC1);
 printf("VISATOMIC2             %#010x \n",(UI4)VISATOMIC2);
 printf("VISDD                  %#010x \n",(UI4)VISDD);
 printf("VLOCK                  %#010x \n",(UI4)VLOCK);
 printf("VNOLOCCHG              %#010x \n",(UI4)VNOLOCCHG);
 printf("VNOLOCCHGX             %#010x \n",(UI4)VNOLOCCHGX);
 printf("VNONAME                %#010x \n",(UI4)VNONAME);
 printf("VNONAMEX               %#010x \n",(UI4)VNONAMEX);
 printf("VNOSELF                %#010x \n",(UI4)VNOSELF);
 printf("VNOSELFX               %#010x \n",(UI4)VNOSELFX);
 printf("VRTNNONE               %#010x \n",(UI4)VRTNNONE);
 printf("VTRY1                  %#010x \n",(UI4)VTRY1);
 printf("VTRY2                  %#010x \n",(UI4)VTRY2);
 printf("VUNARYCODE0            %#010x \n",(UI4)VUNARYCODE0);
 printf("VUNARYCODE1            %#010x \n",(UI4)VUNARYCODE1);
 printf("VUNARYCODE2            %#010x \n",(UI4)VUNARYCODE2);
 printf("VUNARYCODE3            %#010x \n",(UI4)VUNARYCODE3);
 printf("VUNARYCODEX            %#010x \n",(UI4)VUNARYCODEX);
 printf("VWASUNARY              %#010x \n",(UI4)VWASUNARY);
 printf("VXOP                   %#010x \n",(UI4)VXOP);
 printf("VXOPCALL               %#010x \n",(UI4)VXOPCALL);
 printf("VXOPR                  %#010x \n",(UI4)VXOPR);
 printf("VXOPRX                 %#010x \n",(UI4)VXOPRX);
 printf("XCVTXNUMORIDE          %#010x \n",(UI4)XCVTXNUMORIDE);
 printf("XCVTXNUMORIDEMSK       %#010x \n",(UI4)XCVTXNUMORIDEMSK);
 printf("XCVTXNUMORIDEX         %#010x \n",(UI4)XCVTXNUMORIDEX);
 printf("XDX                    %#010x \n",(UI4)XDX);
 printf("XNUM                   %#010x \n",(UI4)XNUM);
 printf("XNUMSIZE               %#010x \n",(UI4)XNUMSIZE);
 printf("XNUMX                  %#010x \n",(UI4)XNUMX);
*/
/* JCON1 */
#if NORMAHN
 ASSERTSYS((!AT(FUNCTYPE0)),"FUNCTYPE0 invalid");
 ASSERTSYS((!(FAV(FUNCID0)->id)),"FUNCID0 invalid");
 ASSERTSYS((!AFLAG(AFLAG0)&&!AT(AFLAG0)),"AFLAG0 invalid");
 ASSERTSYS((!AN(ANLEN0)),"ANLEN0 invalid");
#endif
 jtmeminits(jjt);
 jtmeminitt(jt);  // init allocation queues & tpop stack, master thread only
 RZ(mnuvxynam[0]=makename("m",0));
 RZ(mnuvxynam[1]=makename("n",0));
 RZ(mnuvxynam[2]=makename("u",NAMEBYVALUE));  // uv (and thus u. v.) must be defined when used in an explicit def.  Otherwise OK
 RZ(mnuvxynam[3]=makename("v",NAMEBYVALUE));
 RZ(mnuvxynam[4]=makename("x",0));
 RZ(mnuvxynam[5]=makename("y",0));
 // can be left at initial value v00[0]=v00[1]=0;   // vector 0 0, for rank
 pf=qpf();  // init performance monitor count info
 pinit();  // init block for a.
 cpuInit();  // get CPU characteristics
#if defined(__aarch64__)
 hwaes=(getCpuFeatures()&ARM_HWCAP_AES)?1:0;
 hwfma=1;
#elif (defined(__i386__) || defined(_M_X64) || defined(__x86_64__))
 hwaes=((getCpuFeatures()&CPU_X86_FEATURE_SSE4_1)&&(getCpuFeatures()&CPU_X86_FEATURE_AES_NI))?1:0;
#endif
#if defined(__x86_64__)
 hwfma=(getCpuFeatures()&CPU_X86_FEATURE_FMA)?1:0;
#endif
 DO(IOTAVECLEN, iotavec[i]=i+IOTAVECBEGIN;)  // init our vector of ascending integers
 // We have no more use for the jt block.  The values allocated from it will never be freed.  We leave them extant,
 // but we can free the tpop stack that we allocated
 jtforeigninit(jt);  // init m!:n blocks, master thread only
 FREE(jt->tstackcurr);
 R 1;
}

static B jtevinit(JS jjt){A q,*v;JJ jt=MTHREAD(jjt);
 GA10(q,BOX,1+NEVM); v=AAV(q);
 DO(AN(q), v[i]=mtv;);
 v[EVALLOC  ]=INCORPNA(cstr("allocation error"           ));
 v[EVASSERT ]=INCORPNA(cstr("assertion failure"          ));
 v[EVATTN   ]=INCORPNA(cstr("attention interrupt"        ));
 v[EVBREAK  ]=INCORPNA(cstr("break"                      ));
 v[EVCTRL   ]=INCORPNA(cstr("control error"              ));
 v[EVDOMAIN ]=INCORPNA(cstr("domain error"               ));
 v[EVFACCESS]=INCORPNA(cstr("file access error"          ));
 v[EVFNAME  ]=INCORPNA(cstr("file name error"            ));
 v[EVFOLDLIMIT]=INCORPNA(cstr("fold limit"            ));
 v[EVFNUM   ]=INCORPNA(cstr("file number error"          ));
 v[EVILNAME ]=INCORPNA(cstr("ill-formed name"            ));
 v[EVILNUM  ]=INCORPNA(cstr("ill-formed number"          ));
 v[EVINDEX  ]=INCORPNA(cstr("index error"                ));
 v[EVINPRUPT]=INCORPNA(cstr("input interrupt"            ));
 v[EVFACE   ]=INCORPNA(cstr("interface error"            ));
 v[EVLENGTH ]=INCORPNA(cstr("length error"               ));
 v[EVLIMIT  ]=INCORPNA(cstr("limit error"                ));
 v[EVLOCALE ]=INCORPNA(cstr("locale error"               ));
 v[EVNAN    ]=INCORPNA(cstr("NaN error"                  ));
 v[EVNONCE  ]=INCORPNA(cstr("nonce error"                ));
 v[EVNONNOUN]=INCORPNA(cstr("noun result was required"   ));
 v[EVSPARSE ]=INCORPNA(cstr("non-unique sparse elements" ));
 v[EVOPENQ  ]=INCORPNA(cstr("open quote"                 ));
 v[EVWSFULL ]=INCORPNA(cstr("out of memory"              ));
 v[EVRANK   ]=INCORPNA(cstr("rank error"                 ));
 v[EVRO     ]=INCORPNA(cstr("read-only data"             ));
 v[EVSECURE ]=INCORPNA(cstr("security violation"         ));
 v[EVSPELL  ]=INCORPNA(cstr("spelling error"             ));
 v[EVSTACK  ]=INCORPNA(cstr("stack error"                ));
 v[EVSTOP   ]=INCORPNA(cstr("stop"                       ));
 v[EVSYNTAX ]=INCORPNA(cstr("syntax error"               ));
 v[EVSYSTEM ]=INCORPNA(cstr("system error"               ));
 v[EVTHROW  ]=INCORPNA(cstr("uncaught throw."            ));
 v[EVTIME   ]=INCORPNA(cstr("time limit"                 ));
 v[EVVALENCE]=INCORPNA(cstr("valence error"              ));
 v[EVVALUE  ]=INCORPNA(cstr("value error"                ));
 v[EVINHOMO  ]=v[EVDOMAIN];ras(v[EVINHOMO]);
 v[EVINDEXDUP  ]=v[EVINDEX];ras(v[EVINDEXDUP]);
 v[EVEMPTYT  ]=v[EVCTRL];ras(v[EVEMPTYT]);
 v[EVEMPTYDD  ]=v[EVCTRL];ras(v[EVEMPTYDD]);
 v[EVMISSINGGMP  ]=v[EVFACE];ras(v[EVFACE]);
 v[EVSIDAMAGE  ]=v[EVSTACK];ras(v[EVSTACK]);
 v[EVDEADLOCK  ]=INCORPNA(cstr("would deadlock"));
 v[EVASSEMBLY  ]=INCORPNA(cstr("domain error"));
 v[EVOFLO  ]=INCORPNA(cstr("fixed-precision overflow"));
 v[EVFOLDTHROW  ]=INCORPNA(cstr("Z: throw"));
 v[EVNORESULT  ]=INCORPNA(cstr("no result"));
 v[EVTYPECHG  ]=v[EVDOMAIN];ras(v[EVDOMAIN]);
 v[EVUNTIMELY ]=INCORPNA(cstr("untimely request"));
 ACINITZAPRECUR(q,BOX); INITJT(jjt,evm)=q;   // q and its contents are not on tstack; this way the contents are freed on assignment
 if(jt->jerr){printf("evinit failed; error %hhi\n", jt->jerr); R 0;} else R 1;
}

/* static void sigflpe(int k){jsignal(EVDOMAIN); signal(SIGFPE,sigflpe);} */

static B jtconsinits(JS jjt){D y;JJ jt=MTHREAD(jjt);
// This is an initialization routine, so memory allocations performed here are NOT
// automatically freed by tpop()
#if AUDITCOMPILER
// verify that CTLZ works correctly, and that all calls to BP return what they used to
if (CTTZ(0x1000LL) != 12)*(I *)0 = 100;   // Create program check if error
if (CTTZZ(0x80000000LL) != 31)*(I *)1 = 101;   // Create program check if error
if (CTTZZ(0x100000002LL) != 1)*(I *)2 = 102;   // Create program check if error
if (CTTZZ(0x140000000LL) != 30)*(I *)3 = 103;   // Create program check if error
// verify that (I)x >> does sign-extension.  jtmult relies on that
if(((-1) >> 1) != -1)*(I *)4 = 104;
#endif

 INITJT(jjt,asgzomblevel) = 1;  // allow premature change to zombie names, but not data-dependent errors
 INITJT(jjt,assert) = 1;
 MC(INITJT(jjt,bx),"+++++++++|-",sizeof(INITJT(jjt,bx)));
 INITJT(jjt,disp)[0]=1; INITJT(jjt,disp)[1]=5;
 INITJT(jjt,outmaxafter)=ITOFLOAT16(222);
 INITJT(jjt,outmaxlen)=ITOFLOAT16(256);
 INITJT(jjt,transposeflag)=1;
// INITJT(jjt,int64rflag)=0;
 MC(INITJT(jjt,baselocale),"base",sizeof(INITJT(jjt,baselocale)));   // establish value & hash of "base"
 INITJT(jjt,baselocalehash)=(UI4)nmhash(sizeof(INITJT(jjt,baselocale)),INITJT(jjt,baselocale));
  // Init for u./v.
 A uimp=ca(mnuvxynam[2]); NAV(uimp)->flag|=NMIMPLOC;  // create the name for u.
 INITJT(jjt,implocref)[0] = fdef(0,CTILDE,VERB,jtvalenceerr,jtvalenceerr, uimp,0L,0L, VNOSELF, RMAX,RMAX,RMAX); AC(INITJT(jjt,implocref)[0])=ACUC1;  //create 'u.'~, mark as not abandoned (no ra() needed)
 A vimp=ca(mnuvxynam[3]); NAV(vimp)->flag|=NMIMPLOC;
 INITJT(jjt,implocref)[1] = fdef(0,CTILDE,VERB,jtvalenceerr,jtvalenceerr, vimp,0L,0L, VNOSELF, RMAX,RMAX,RMAX); AC(INITJT(jjt,implocref)[1])=ACUC1;  //create 'v.'~
 INITJT(jjt,igemm_thres)=FLOATTOFLOAT16((hascblas)?1000L:IGEMM_THRES);   // tuning parameters for cip.c
 INITJT(jjt,dgemm_thres)=FLOATTOFLOAT16((hascblas)?1000L:DGEMM_THRES);
 INITJT(jjt,zgemm_thres)=FLOATTOFLOAT16((hascblas)?1000L:ZGEMM_THRES);
 jt->cstackinit=(uintptr_t)&y;  // use a static variable to get the stack address
 jt->cstackmin=jt->cstackinit-(CSTACKSIZE-CSTACKRESERVE);
 MTHREAD(jjt)->threadpoolno=-1; // the master thread is in no pool, ever
 R 1;
}

static B jtconsinitt(J jt){
 jt->ppn=6;  // default precision for printf
 jt->ecmtries=3;  // number of tries for elliptic-curve factoring
 jt->cct= 1.0-FUZZ;
 jt->xmode=XMEXACT;
 // create an initial stack, so that stack[-1] can be used for saving error messages
 jt->parserstackframe.parserstkbgn=jt->parserstackframe.parserstkend1=&initparserstack[1];  // ensure valid error stack after final return (start off the end)
 R 1;
}

// initialise shared buffers
static B jtbufferinits(JS jjt){
 R !!(INITJT(jjt,breakfn)=calloc(1,NPATH)); // place to hold the break filename
}

// initialise thread-local buffers for thread threadno.  Requires synchronisation
B jtbufferinitt(J jt){
 RZ(jt->etxinfo=malloc(sizeof(ETXDATA)));  // error-message buffer
 RZ(jt->rngdata=aligned_malloc(sizeof(RNG),CACHELINESIZE)); // place to hold RNG data, aligned to cacheline
 memset(jt->rngdata,0,sizeof(RNG));
 R 1;
}

// We have completed initial allocation.  Everything allocated so far will not be freed by a tpop, because
// tpop() isn't called during initialization.  So, to keep the memory auditor happy, we reset ttop so that it doesn't
// look like those symbols have a free outstanding.
// This also has the effect that buffers allocated during init do not need ra() to protect them, since they have no free outstanding
static B jtinitfinis(J jt){
 jt->tnextpushp=(A*)(((I)jt->tstackcurr+NTSTACKBLOCK)&(-NTSTACKBLOCK))+1;  // first store is to entry 1 of the first block
 R 1;
}

// Initialise thread-specific data for jt.  Idempotent.  Requires synchronisation.
C jtjinitt(J jt){
 if(jt->etxinfo)R 1; // already initialised; ok
 RZ(jtbufferinitt(jt));  // init thread-local buffers
 RZ(jtmeminitt(jt));
 RZ(jtconsinitt(jt));
 RZ(jtrnginit(jt)); // thread only
 RZ(jtecvtinit(jt));
 RZ(jtinitfinis(jt));
 R 1;}

// initialize the master thread for a new instance.  This fills in the JS block, which will remain
// for the duration of the instance.  It also fills in the JJ block for each thread
static C jtjinit3(JS jjt){S t;JJ jt=MTHREAD(jjt);
/* required for jdll and doesn't hurt others */
 gjt=jjt; // global jt for JPF debug
 jt->taskstate=0;  // The master thread is non-running when it is idle, so that system lock doesn't wait for it
// only for looking for rank flaws     jjt->forcedeprecmsg=1;  // init displaying msgs
 
#if (SYS & SYS_DOS)
 t=EM_ZERODIVIDE+EM_INVALID; _controlfp(t,t);
#endif
#if (SYS & SYS_OS2)
 t=EM_ZERODIVIDE+EM_INVALID+EM_OVERFLOW+EM_UNDERFLOW; _control87(t,t);
#endif
#if (SYS & SYS_FREEBSD)
 fpsetmask(0);
#endif
 INITJT(jjt,tssbase)=tod();  // starting time for all threads
#if PYXES
 INITJT(jjt,jobqueues)=aligned_malloc(sizeof(JOBQ[MAXTHREADPOOLS]),CACHELINESIZE); // job queue, cache-line aligned
 memset(INITJT(jjt,jobqueues),0,sizeof(JOBQ[MAXTHREADPOOLS]));
 DO(MAXTHREADPOOLS, (*INITJT(jjt,jobqueues))[i].ht[1]=(JOB *)&(*INITJT(jjt,jobqueues))[i].ht[1];)  // when q is empty, tail points to itself, as a safe NOP store
#endif
 DO(256, if(primtab[i].prim.primvb.flag&VISATOMIC2)va2ctoc[primtab[i].prim.primvb.lu2.lc&0x7f]=i;)
// only crashing on startup INITJT(jjt,peekdata)=1;  // wake up auditing
 // Initialize subsystems in order.  Each initializes all threads, if there are thread variables
 RZ(jtbufferinits(jjt)); // init the buffers pointed to by jjt
 RZ(jtbufferinitt(jt));  // init thread-local buffers
 RZ(jtmeminits(jjt));
 RZ(jtmeminitt(jt));
 RZ(jtsesminit(jjt,1));  // master only
 RZ(jtcdinit(jjt));  // master only
 RZ(jtevinit(jjt));  // master only
 RZ(jtconsinits(jjt));
 RZ(jtconsinitt(jt));
 RZ(jtxsinit(jjt));  // must be before symbinit  master only
 RZ(jtsymbinit(jjt));  // must be after consinit   master only - global/locsyms must init at start of op
 RZ(jtxoinit(jjt));  // master only
 RZ(jtrnginit(jt));
 RZ(jtforeignassigninit(jt));  // master only
// #if (SYS & SYS_DOS+SYS_MACINTOSH+SYS_UNIX)
#if (SYS & SYS_DOS+SYS_MACINTOSH)
 RZ(jtxlinit(jjt));  // file info, master only
#endif
 RZ(jtecvtinit(jt));
 RZ(jtinitfinis(jt));
 R 1;
}

// Here we initialize the new jt for a new J instance.
C jtjinit2(JS jt,int dummy0,C**dummy1){INITJT(jt,sesm)=1; R jinit3();}
