\ Equation counts
\     Total        E        G        L        N        X        C        B
\         2        0        2        0        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\         2        0        0        2        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\         6        3        3        0
\ 
Minimize
 obj: - 16 i1 - 4 i2 + [ 2 i1^2 + 2 i2^2 ]/2 + objconstant

Subject To
 e1: i2 + [ - .1 i1^2 ] >= 0
 e2: - .333333333333333 i1 - i2 >= -4.5

Bounds
 i1 <= 200
 i2 <= 200
 objconstant = 68

General
 i1 i2

End
