t1=: 3 : 0
if. 0=y do.
s=. LF,~ '#if NORMAHN==',":y
s=. s, LF,~ '#define Xhr0'
s=. s, LF,~ '#define Xhrg0'
else.
s=. LF,~ '#elif NORMAHN==',":y
s=. s, LF,~ '#define Xhr0    0,',,(y-1)#,:'XHEADERFILL,'
s=. s, LF,~ '#define Xhrg0 0,0,', (y>1)# '{', '},',~ }:,(0>.y-1)#,:'XHEADERFILL,'
end.
<s
)

t2=: 3 : 0
s=. t1"0 i.y
s=. s, <'#else',LF,'#error invalid NORMAHN',LF,'#endif',LF
(;s) fwrite '~/xhr.h'
)

t3=: 3 : 0
if. 2=y do.
s=. LF,~ '#if NORMAHN==',":y
else.
s=. LF,~ '#elif NORMAHN==',":y
end.
s=. s, LF,~ '#define APINIT(x,v) ','(v)',~ ; (<'])=') ,&.>~ (<'((x)->p[') ,&.> (<@":"0 i.y-1)
<s
)

t4=: 3 : 0
s=. <'#if NORMAHN>1 && MEMAUDIT&0x80',LF,'#define APX(x)      ((x)->p[0])',LF
s=. s, t3"0 [ 2}. i.y
s=. s, <'#endif',LF,'#else',LF,'#define APINIT(x,v)',LF,'#endif',LF
(;s) fwrite '~/apinit.h'
)

t2 100
t4 100
