\ Equation counts
\     Total        E        G        L        N        X        C        B
\         1        0        1        0        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\         5        0        0        5        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\        10        5        5        0
\ 
Minimize
 obj: 42 i1 + 44 i2 + 45 i3 + 47 i4 + 47.5 i5 + [ 100 i1^2 + 100 i2^2 + 100 i3^2 + 100 i4^2 + 100 i5^2 
  ]/2

Subject To
 e1: 20 i1 + 12 i2 + 11 i3 + 7 i4 + 4 i5 >= 40

Bounds
 i1 <= 1
 i2 <= 1
 i3 <= 1
 i4 <= 1
 i5 <= 1

General
 i1 i2 i3 i4 i5

End
