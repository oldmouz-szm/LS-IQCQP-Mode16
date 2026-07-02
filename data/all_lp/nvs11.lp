\ Equation counts
\     Total        E        G        L        N        X        C        B
\         3        0        3        0        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\         3        0        0        3        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\        12        0       12        0
\ 
Minimize
 obj: - 15.8 i1 - 93.2 i2 - 63 i3 + [ 14 i1^2 - 12 i1 * i3 + 12 i2^2 + 8 i2 * i3 + 16 i3^2 ]/2

Subject To
 e1: [ - 9 i1^2 - 10 i1 * i2 - 6 i1 * i3 - 8 i2^2 - 10 i2 * i3 - 5 i3^2 ] >= -1e3
 e2: [ - 6 i1^2 - 8 i1 * i2 - 2 i1 * i3 - 6 i2^2 - 2 i2 * i3 - 4 i3^2 ] >= -5.5e2
 e3: [ - 9 i1^2 + 2 i1 * i2 - 6 i2^2 + 2 i2 * i3 - 8 i3^2 ] >= -3.4e2

Bounds
 i1 <= 200
 i2 <= 200
 i3 <= 200

General
 i1 i2 i3

End
