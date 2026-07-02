\ Equation counts
\     Total        E        G        L        N        X        C        B
\         2        0        0        2        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\         6        0        0        6        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\        14        9        5        0
\ 
Minimize
 obj: 10.5 i1 - 7.5 i2 - 3.5 i3 + 2.5 i4 - 1.5 i5 + 10 i6 + [ i1^2 + .5 i2^2 + 3 i3^2 + i4^2 + i5^2 ]
  /2

Subject To
 e1: 6 i1 + 3 i2 + 3 i3 + 2 i4 + i5 <= 6.5
 e2: 10 i1 + 10 i3 + i6 <= 20

Bounds
 i1 <= 1
 i2 <= 1
 i3 <= 1
 i4 <= 1
 i5 <= 1
 i6 <= 1e15

General
 i1 i2 i3 i4 i5 i6

End
