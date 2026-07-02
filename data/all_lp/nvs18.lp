\ Equation counts
\     Total        E        G        L        N        X        C        B
\         6        0        6        0        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\         6        0        0        6        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\        42        0       42        0
\ 
Minimize
 obj: .2 i1 - 53.6 i2 + 4.4 i3 - 24.8 i4 - 104.8 i5 - 56.4 i6 + [ 14 i1^2 - 12 i1 * i3 + 4 i1 * i4 - 8 i1 * i5 
  + 4 i1 * i6 + 12 i2^2 + 8 i2 * i3 - 4 i2 * i5 - 8 i2 * i6 + 16 i3^2 + 4 i3 * i4 - 12 i3 * i5 - 8 i3 * i6 
  + 12 i4^2 - 4 i4 * i6 + 14 i5^2 + 12 i5 * i6 + 8 i6^2 ]/2

Subject To
 e1: [ - 9 i1^2 - 10 i1 * i2 - 6 i1 * i3 - 10 i1 * i4 - 6 i1 * i6 - 8 i2^2 - 10 i2 * i3 - 6 i2 * i4 - 2 i2 * i5 
  - 2 i2 * i6 - 5 i3^2 - 2 i3 * i4 - 7 i4^2 - 2 i4 * i6 - 7 i5^2 - 5 i6^2 ] >= -1.8e3
 e2: [ - 6 i1^2 - 8 i1 * i2 - 2 i1 * i3 + 2 i1 * i4 - 2 i1 * i5 - 6 i2^2 - 2 i2 * i3 + 10 i2 * i4 - 6 i2 * i5 
  - 2 i2 * i6 - 4 i3^2 + 8 i3 * i6 - 8 i4^2 + 6 i4 * i5 + 2 i4 * i6 + 7 i5^2 - 4 i5 * i6 - 8 i6^2 ] >= 
  -1.52e3
 e3: [ - 9 i1^2 + 2 i1 * i2 + 4 i1 * i4 - 6 i1 * i5 + 2 i1 * i6 - 6 i2^2 + 2 i2 * i3 + 4 i2 * i4 - 2 i2 * i5 
  + 4 i2 * i6 - 8 i3^2 - 2 i3 * i4 - 6 i4^2 + 4 i4 * i5 - 6 i4 * i6 + 6 i5^2 - 2 i5 * i6 - 5 i6^2 ] >= 
  -1e3
 e4: [ - 8 i1^2 - 2 i1 * i2 - 2 i1 * i3 + 6 i1 * i4 - 6 i1 * i5 - 10 i1 * i6 - 4 i2^2 - 4 i2 * i3 + 2 i2 * i4 
  - 4 i2 * i5 - 9 i3^2 - 2 i3 * i4 - 2 i3 * i5 - 10 i3 * i6 - 7 i4^2 + 6 i4 * i5 + 4 i4 * i6 + 6 i5^2 
  - 2 i5 * i6 - 7 i6^2 ] >= -1745
 e5: [ - 4 i1^2 + 2 i1 * i2 - 6 i1 * i3 - 2 i1 * i4 - 4 i1 * i5 + 4 i1 * i6 - 5 i2^2 + 6 i2 * i4 + 2 i2 * i5 
  - 4 i2 * i6 - 8 i3^2 - 2 i3 * i4 - 6 i3 * i5 + 6 i3 * i6 - 6 i4^2 - 8 i4 * i5 - 7 i5^2 + 4 i5 * i6 
  - 7 i6^2 ] >= -1.07e3
 e6: [ - 7 i1^2 + 2 i1 * i2 - 6 i1 * i3 - 2 i1 * i4 - 2 i1 * i5 + 2 i1 * i6 - 7 i2^2 - 2 i2 * i3 + 2 i2 * i4 
  - 4 i2 * i6 - 6 i3^2 - 2 i3 * i4 - 4 i3 * i5 + 4 i3 * i6 - 5 i4^2 + 2 i4 * i5 + 2 i4 * i6 - 5 i5^2 
  + 6 i5 * i6 - 9 i6^2 ] >= -9.9e2

Bounds
 i1 <= 200
 i2 <= 200
 i3 <= 200
 i4 <= 200
 i5 <= 200
 i6 <= 200

General
 i1 i2 i3 i4 i5 i6

End
