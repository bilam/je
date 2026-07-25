(9!:14'') 1!:2 [2

GITHUBCI=: 'true'-:2!:5'GITHUB_ACTIONS'

1!:55 ::empty <'jobdone'
testpath=: (1!:43''),'/test/'
0!:0 <testpath,'tsu.ijs'

os=: (('Linux';'Darwin';'OpenBSD';'FreeBSD') i. <UNAME) pick ;:'linux darwin openbsd freebsd win'
os=: ((IF64{::'rpi32';'rpi64')"_)^:IFRASPI os
os=: ((IF64{::'win32';'win')"_)^:IFWIN os
os=: os, ((<os)e.'openbsd';'freebsd')#(('arm64'-:9!:56'cpu'){::'';'arm64')
os=: os, ((<os)e.<'darwin')#('ARM64'-:2!:5'RUNNER_ARCH'){::'';'arm'
os=: os, ((<os)e.<'win')#(('arm64'-:9!:56'cpu'){::'';'arm64')
os=: os, (IFRASPI<(<os)e.<'linux')#(IF64{::'32';'')
NB. os=: os, ((,'3')-:2!:5'_DEBUG'){::'';'d'
os=: os, ((<ma) -.@e. (0;,'0')){::'';'ma' [ ma=. 2!:5'_MEMAUDIT'
4!:55 <'ma'
testres=: 'test',os,'.txt'

ECHOFILENAME=: 1   NB. echo file name
stdout LF ,~ 9!:14''
echo '_DEBUG: ',": 2!:5'_DEBUG'
echo 'RUNNER_ARCH: ',": 2!:5'RUNNER_ARCH'

ddall=: ~. ddall ,~ (<testpath),each 'glapack.ijs';'glapackcb.ijs';'gregex.ijs'
ddall=: ddall -. blacklist=: blacklist, ('OpenBSD'-:UNAME)#(<testpath),each <'gstack.ijs' NB. temporarily disable
ddall=: ddall -. blacklist=: blacklist, ('OpenBSD'-:UNAME)#(<testpath),each 'gtdot.ijs';'gtdot3.ijs';'gtdot4.ijs';'gtdot5.ijs' NB. temporarily disable
ddall=: ddall -. blacklist=: blacklist, ((1 -.@e. '/gcc-' E. 9!:14'')+.('0'={.":2!:5'_DEBUG')+.IF64+.UNAME-.@-:'linux')#(<testpath),each 'glapack.ijs';'glapackcb.ijs'  NB. linux32 gcc lapack issue running under gdb

NB. smoke test

NB. RES=: RUN4 (<testpath),each IF64{:: (<'gstack.ijs') ,&< 'gtdot1.ijs';'gtdot5.ijs'
NB. echo^:(*@#RES) RES
NB. RUN1 ::0:@dtb"1^:(*@#RES) RES
NB. exit^:(*@#RES) *@#RES

oldnl=: (;:'x y') -.~ ~. oldnl, (nl__ i.4) , ;:'FINISH RES'

FINISH=: 3 : 0
msg=. 9!:14''
if. 0=#RES do.
  msg=. msg,LF,'all tests correct'
else.
  msg=. msg,LF,'test fails:'
  msg=. msg,;<@(LF,dtb) "1 RES
end.
msg fappends testres
'' (1!:2 ::empty)^:(-.@*@#RES) <'jobdone'
echo^:(*@#RES) RES
)

9!:27'FINISH RES=: RUN4 ddall'
9!:29]1
exit *@#RES
