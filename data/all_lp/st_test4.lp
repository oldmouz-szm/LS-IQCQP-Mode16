\ Equation counts
\     Total        E        G        L        N        X        C        B
\         5        0        0        5        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\         6        0        0        6        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\        36       34        2        0
\ 
Minimize
 obj: 6.5 i1 - i2 - 2 i3 + 3 i4 - 2 i5 - i6 + [ i1^2 + 14 i6^2 ]/2

Subject To
 e1: i1 + 2 i2 + 8 i3 + i4 + 3 i5 + 5 i6 <= 16
 e2: - 8 i1 - 4 i2 - 2 i3 + 2 i4 + 4 i5 - i6 <= -1
 e3: 2 i1 + .5 i2 + .2 i3 - 3 i4 - i5 - 4 i6 <= 24
 e4: .2 i1 + 2 i2 + .1 i3 - 4 i4 + 2 i5 + 2 i6 <= 12
 e5: - .1 i1 - .5 i2 + 2 i3 + 5 i4 - 5 i5 + 3 i6 <= 3

Bounds
 i1 <= 1e15
 i2 <= 1e15
 i3 <= 1e15
 i4 <= 1
 i5 <= 1
 i6 <= 2

General
 i1 i2 i3 i4 i5 i6

End
