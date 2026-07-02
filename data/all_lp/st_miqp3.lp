\ Equation counts
\     Total        E        G        L        N        X        C        B
\         1        0        0        1        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\         2        0        0        2        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\         4        3        1        0
\ 
Minimize
 obj: 0 i1 - 3 i2 + [ 12 i1^2 ]/2

Subject To
 e1: - 4 i1 + i2 <= 0

Bounds
 i1 <= 3
 i2 <= 1e15

General
 i1 i2

End
