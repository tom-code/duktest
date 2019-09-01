

prt('aaa');

a = 100
//callback(function f() {
//    prt('bbb')
//    prt(a)
//})

function zz(inx) {
    prt(inx)
    prt('zzzzz')
    callback(function f() {
        prt('bbb')
        prt(inx)
    })
}

function otest(s1, obj, s2) {
    prt(s1)
    prt(s2)
    obj.test()
}

1+2
