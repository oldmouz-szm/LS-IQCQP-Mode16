\ Equation counts
\     Total        E        G        L        N        X        C        B
\         1        0        1        0        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\         3        0        0        3        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\         6        3        3        0
\ 
Minimize
 obj: - 8 i1 - 6 i2 - 4 i3 + [ 4 i1^2 + 4 i1 * i2 + 4 i1 * i3 + 4 i2^2 + 2 i3^2 ]/2 + objconstant

Subject To
 e1: - i1 - i2 - 2 i3 >= -3

Bounds
 i1 <= 200
 i2 <= 200
 i3 <= 200
 objconstant = 9

General
 i1 i2 i3

End
