prolog './g530m1.ijs'
NB. m}y and u}y ---------------------------------------------------------

NB. debug failure on windows

NB. complementary indexing and axes in full
NB. x has an atom for each leading axis of y to be fetched from:
NB. 0=atom 1=list 2=complementary 3=axis in full
NB. we calculate a selector for the given x.  Then we amend with it, comparing against an exploded version
NB. We do amends with each possible frame of replacement data
test1 =: {{
axes =. 0$a: [ exaxes =. 0$0  NB. axes, exploded axes
for_i. x do.
 ys =. i_index { $y  NB. length of selected axis
 select. i
 case. 0 do. exaxis =. axis =. ?ys
 case. 1 do. exaxis =. axis =. (?ys) ? ys
 case. 2 do. exaxis =. (i.ys) -. >axis =. < (?ys) ? ys
 case. 3 do. exaxis =. i. ys [ axis =. a:
 end.
 axes =. axes , <axis [ exaxes =. exaxes ,"1 0"1 _ exaxis
end.
NB. We have the selectors.
selshape =. (}:$exaxes) , ({:$exaxes)}.$y  NB. shape of m{y
NB. obsolete ax   =: axes
NB. obsolete exax   =: exaxes
ip =. 1 |. y  NB. inplaceable version
for_i. i.#selshape do.
 repldata =. (,y) {~ (i}.selshape)?@$ */@$ y
NB. obsolete rd   =: repldata
NB. 'repldata__ axes__ exaxes__ y__'   =: repldata;axes;exaxes;<y
 assert. repldata ((<axes)} -: (<^:(1=#@$) exaxes)}) y
 NB. Also execute inplace, to exercise usecount management
 ip =. repldata (<axes)} ip
end.
1
}}"1 _
NB. x is number of leading axes to test
test2 =: {{
((#: i.@(*/)) x # 4) test1 y
1
}}"0 _

0 1 2 3 test2 <"0 i. 4 3 4 5 2
0 1 2 3 test2 1r2 + i. 4 3 4 5 3
0 1 2 3 test2 0x + i. 4 3 4 5 3

epilog''

