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
\         4        2        2        0
\ 
Minimize
 obj: 3 i1 + 2 i2

Subject To
 e2: [ - i1 * i2 ] <= -3.5

Bounds
 1 <= i1 <= 5
 1 <= i2 <= 5

General
 i1 i2

End
