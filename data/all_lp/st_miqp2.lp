\ Equation counts
\     Total        E        G        L        N        X        C        B
\         3        0        1        2        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\         4        0        0        4        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\        10        8        2        0
\ 
Minimize
 obj: 4 i1 + 5 i2 - 3 i3 - 10 i4 + [ 8 i3^2 + 4 i4^2 ]/2

Subject To
 e1: - 10 i1 + i3 <= 0
 e2: - 20 i2 + i4 <= 0
 e3: i3 + i4 >= 5

Bounds
 i1 <= 1
 i2 <= 1
 i3 <= 1e10
 i4 <= 1e10

General
 i1 i2 i3 i4

End
