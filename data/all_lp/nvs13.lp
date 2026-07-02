\ Equation counts
\     Total        E        G        L        N        X        C        B
\         5        0        5        0        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\         5        0        0        5        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\        30        0       30        0
\ 
Minimize
 obj: 12 i1 - 77.2 i2 - 19.2 i3 - 36.6 i4 - 69.4 i5 + [ 14 i1^2 - 12 i1 * i3 + 4 i1 * i4 - 8 i1 * i5 
  + 12 i2^2 + 8 i2 * i3 - 4 i2 * i5 + 16 i3^2 + 4 i3 * i4 - 12 i3 * i5 + 12 i4^2 + 14 i5^2 ]/2

Subject To
 e1: [ - 9 i1^2 - 10 i1 * i2 - 6 i1 * i3 - 10 i1 * i4 - 8 i2^2 - 10 i2 * i3 - 6 i2 * i4 - 2 i2 * i5 - 5 i3^2 
  - 2 i3 * i4 - 7 i4^2 - 7 i5^2 ] >= -1.43e3
 e2: [ - 6 i1^2 - 8 i1 * i2 - 2 i1 * i3 + 2 i1 * i4 - 2 i1 * i5 - 6 i2^2 - 2 i2 * i3 + 10 i2 * i4 - 6 i2 * i5 
  - 4 i3^2 - 8 i4^2 + 6 i4 * i5 + 7 i5^2 ] >= -1.15e3
 e3: [ - 9 i1^2 + 2 i1 * i2 + 4 i1 * i4 - 6 i1 * i5 - 6 i2^2 + 2 i2 * i3 + 4 i2 * i4 - 2 i2 * i5 - 8 i3^2 
  - 2 i3 * i4 - 6 i4^2 + 4 i4 * i5 + 6 i5^2 ] >= -8.5e2
 e4: [ - 8 i1^2 - 2 i1 * i2 - 2 i1 * i3 + 6 i1 * i4 - 6 i1 * i5 - 4 i2^2 - 4 i2 * i3 + 2 i2 * i4 - 4 i2 * i5 
  - 9 i3^2 - 2 i3 * i4 - 2 i3 * i5 - 7 i4^2 + 6 i4 * i5 + 6 i5^2 ] >= -1125
 e5: [ - 4 i1^2 + 2 i1 * i2 - 6 i1 * i3 - 2 i1 * i4 - 4 i1 * i5 - 5 i2^2 + 6 i2 * i4 + 2 i2 * i5 - 8 i3^2 
  - 2 i3 * i4 - 6 i3 * i5 - 6 i4^2 - 8 i4 * i5 - 7 i5^2 ] >= -1.03e3

Bounds
 i1 <= 200
 i2 <= 200
 i3 <= 200
 i4 <= 200
 i5 <= 200

General
 i1 i2 i3 i4 i5

End
