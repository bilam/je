/* Copyright 1990-2005, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Verbs: Assembly Routines for Integer + * - with Overflow                */

/* fvv  zv=.xv+yv     0<n */
/* fv1  zv=.xv+y      0<n */
/* f1v  zv=.x +yv     0<n */
/* frv  zv=.xv+zv     0<n */
/* fr   z =.+/  xv    1<n */
/* fp   zv=.+/\ xv    1<n */
/* fs   zv=.+/\.xv    1<n */

// #if SY_64
// #define DI LD
// #else
// #define DI D
// #endif

// #define  PLUSVV(m,z,x,y)   {B p;  DO(m, p=0>*x; *z=*x+*y;     BOV(p==0>*y&&p!=0>*z); z++; x++; y++;);}
// #define MINUSVV(m,z,x,y)   {B p;  DO(m, p=0>*x; *z=*x-*y;     BOV(p!=0>*y&&p!=0>*z); z++; x++; y++;);}
// #define TYMESVV(m,z,x,y)   {DI t; DO(m, t=*x*(DI)*y; *z=(I)t; BOV(t<IMIN||IMAX<t  ); z++; x++; y++;);}
#define  PLUSVV(m,z,x,y)   {DO(m, if(unlikely(__builtin_add_overflow(*x,*y,z))){er=EWOV; break;} z++; x++; y++;);}
#define MINUSVV(m,z,x,y)   {DO(m, if(unlikely(__builtin_sub_overflow(*x,*y,z))){er=EWOV; break;} z++; x++; y++;);}
#define TYMESVV(m,z,x,y)   {DO(m, if(unlikely(__builtin_mul_overflow(*x,*y,z))){er=EWOV; break;} z++; x++; y++;);}

// #define  PLUS1V(n,z,u,y)   {B p=0>u;  DO(n, z[i]=u+y[i];         BOV(p==0>y[i]&&p!=0>z[i]););}
// #define MINUS1V(n,z,u,y)   {B p=0>u;  DO(n, z[i]=u-y[i];         BOV(p!=0>y[i]&&p!=0>z[i]););}
// #define TYMES1V(n,z,u,y)   {DI d=u,t; DO(n, t=d*y[i]; z[i]=(I)t; BOV(t<IMIN||IMAX<t      ););}
// cuation. use y[i] z[i] , don't change pointer y z
#define  PLUS1V(n,z,u,y)   {DO(n, if(unlikely(__builtin_add_overflow(u,y[i],z+i))){er=EWOV; break;});}
#define MINUS1V(n,z,u,y)   {DO(n, if(unlikely(__builtin_sub_overflow(u,y[i],z+i))){er=EWOV; break;});}
#define TYMES1V(n,z,u,y)   {DO(n, if(unlikely(__builtin_mul_overflow(u,y[i],z+i))){er=EWOV; break;});}

#define  PLUSV1(n,z,x,v)   PLUS1V(n,z,v,x)
#define TYMESV1(n,z,x,v)   TYMES1V(n,z,v,x)
// #define MINUSV1(n,z,x,v)   {B p=0>v; DO(n, z[i]=x[i]-v;   BOV(p!=0>x[i]&&p==0>z[i]););}
#define MINUSV1(n,z,x,v)   {DO(n, if(unlikely(__builtin_sub_overflow(x[i],v,z+i))){er=EWOV; break;});}

// #define  PLUSP(n,z,x)      {B p;I s=0;   DO(n, p=0>s; *z=s+=*x;        BOV(p==0>*x&&p!=0>s); z++; x++;);}
// #define MINUSP(n,z,x)      {B p=0;DI t=0; DO(n, t=p?t-*x:t+*x; *z=(I)t; BOV(t<IMIN||IMAX<t ); z++; x++; p=!p;);}
// #define TYMESP(n,z,x)      {DI t=1;      DO(n, t*=*x;         *z=(I)t; BOV(t<IMIN||IMAX<t ); z++; x++;);}
#define  PLUSP(n,z,x)      {I s=0; DO(n, if(unlikely(__builtin_add_overflow(s,*x,&s))){er=EWOV; break;} *z=s; z++; x++;);}
#define MINUSP(n,z,x)      {B p=0; I t=0; DO(n, if(p){if(unlikely(__builtin_sub_overflow(t,*x,&t))){er=EWOV; break;}}else{if(unlikely(__builtin_add_overflow(t,*x,&t))){er=EWOV; break;}} *z=t; z++; x++; p=!p;);}
#define TYMESP(n,z,x)      {I t=1; DO(n, if(unlikely(__builtin_mul_overflow(t,*x,&t))){er=EWOV; break;} *z=t; z++; x++;);}

// #define  PLUSR(n,z,x)      {B p;I s=0;   DO(n, p=0>s; s+=*x;  BOV(p==0>*x&&p!=0>s); x++;      ); *z=s;}
// #define MINUSR(n,z,x)      {B p=0;DI t=0; DO(n, t=p?t-*x:t+*x; BOV(t<IMIN||IMAX<t ); x++; p=!p;); *z=(I)t;}
// #define TYMESR(n,z,x)      {DI t=1;      DO(n, t*=*x;         BOV(t<IMIN||IMAX<t ); x++;      ); *z=(I)t;}
#define  PLUSR(n,z,x)      {I s=0; DO(n, if(unlikely(__builtin_add_overflow(s,*x,&s))){er=EWOV; break;} x++;); *z=s;}
#define MINUSR(n,z,x)      {B p=0; I t=0; DO(n, if(p){if(unlikely(__builtin_sub_overflow(t,*x,&t))){er=EWOV; break;}}else{if(unlikely(__builtin_add_overflow(t,*x,&t))){er=EWOV; break;}} x++; p=!p;); *z=t;}
#define TYMESR(n,z,x)      {I t=1; DO(n, if(unlikely(__builtin_mul_overflow(t,*x,&t))){er=EWOV; break;} x++;); *z=t;}

// #define  PLUSRV(d,z,x)     {B p;  DO(d, p=0>*z; *z+=*x;       BOV(p==0>*x&&p!=0>*z); x++; z++;);}
// #define MINUSRV(d,z,x)     {DI t; DO(d, t=*x-(DI)*z; *z=(I)t; BOV(t<IMIN||IMAX<t  ); x++; z++;);}
// #define TYMESRV(d,z,x)     {DI t; DO(d, t=*x*(DI)*z; *z=(I)t; BOV(t<IMIN||IMAX<t  ); x++; z++;);}
#define  PLUSRV(d,z,x)     {DO(d, if(unlikely(__builtin_add_overflow(*x,*z,z))){er=EWOV; break;} z++; x++;);}
#define MINUSRV(d,z,x)     {DO(d, if(unlikely(__builtin_sub_overflow(*x,*z,z))){er=EWOV; break;} x++; z++;);}
#define TYMESRV(d,z,x)     {DO(d, if(unlikely(__builtin_mul_overflow(*x,*z,z))){er=EWOV; break;} z++; x++;);}

// #define  PLUSS(n,z,x)      {B p;I s=0; x+=n; z+=n; DO(n, --x; p=0>s; *--z=s+=*x;       BOV(p==0>*x&&p!=0>s););}
// #define MINUSS(n,z,x)      {B p;I s=0; x+=n; z+=n; DO(n, --x; p=0>s; *--z=s=*x-s;      BOV(p!=0>*x&&p==0>s););}
// #define TYMESS(n,z,x)      {DI t=1;    x+=n; z+=n; DO(n, --x;        t*=*x; *--z=(I)t; BOV(t<IMIN||IMAX<t ););}
#define  PLUSS(n,z,x)      {I s=0; x+=n; z+=n; DO(n, --x; if(unlikely(__builtin_add_overflow(*x,s,&s))){er=EWOV; break;} *--z=s;);}
#define MINUSS(n,z,x)      {I s=0; x+=n; z+=n; DO(n, --x; if(unlikely(__builtin_sub_overflow(*x,s,&s))){er=EWOV; break;} *--z=s;);}
#define TYMESS(n,z,x)      {I t=1; x+=n; z+=n; DO(n, --x; if(unlikely(__builtin_mul_overflow(*x,t,&t))){er=EWOV; break;} *--z=t;);}

