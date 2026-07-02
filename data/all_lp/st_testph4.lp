\ Equation counts
\     Total        E        G        L        N        X        C        B
\        10        0        0       10        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\         3        0        0        3        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\        27       24        3        0
\ 
Minimize
 obj: - 35 i1 + 3 i2 + 4 i3 + [ 7 i1^2 + i2^2 + 4 i3^2 ]/2

Subject To
 e1: i1 <= 4
 e2: i2 <= 4
 e3: i3 <= 4
 e4: 2 i1 + 3 i2 + 4 i3 <= 35
 e5: 2 i1 + 3 i2 - 4 i3 <= 19
 e6: 2 i1 - 3 i2 + 4 i3 <= 23
 e7: - 2 i1 + 3 i2 + 4 i3 <= 27
 e8: 2 i1 - 3 i2 - 4 i3 <= 7
 e9: - 2 i1 - 3 i2 + 4 i3 <= 15
 e10: - 2 i1 + 3 i2 - 4 i3 <= 11

Bounds
 i1 <= 100
 i2 <= 100
 i3 <= 100

General
 i1 i2 i3

End
