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
\         6        0        6        0
\ 
Minimize
 obj: - 35 i1 - 80.4 i2 + [ 14 i1^2 + 12 i2^2 ]/2

Subject To
 e1: [ - 9 i1^2 - 10 i1 * i2 - 8 i2^2 ] >= -583
 e2: [ - 6 i1^2 - 8 i1 * i2 - 6 i2^2 ] >= -441

Bounds
 i1 <= 200
 i2 <= 200

General
 i1 i2

End
