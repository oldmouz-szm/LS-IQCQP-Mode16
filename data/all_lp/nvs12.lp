\ Equation counts
\     Total        E        G        L        N        X        C        B
\         4        0        4        0        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\         4        0        0        4        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\        20        0       20        0
\ 
Minimize
 obj: - 20 i1 - 93.2 i2 - 67.2 i3 - 36.6 i4 + [ 14 i1^2 - 12 i1 * i3 + 4 i1 * i4 + 12 i2^2 + 8 i2 * i3 
  + 16 i3^2 + 4 i3 * i4 + 12 i4^2 ]/2

Subject To
 e1: [ - 9 i1^2 - 10 i1 * i2 - 6 i1 * i3 - 10 i1 * i4 - 8 i2^2 - 10 i2 * i3 - 6 i2 * i4 - 5 i3^2 - 2 i3 * i4 
  - 7 i4^2 ] >= -1.1e3
 e2: [ - 6 i1^2 - 8 i1 * i2 - 2 i1 * i3 + 2 i1 * i4 - 6 i2^2 - 2 i2 * i3 + 10 i2 * i4 - 4 i3^2 - 8 i4^2 
  ] >= -4.4e2
 e3: [ - 9 i1^2 + 2 i1 * i2 + 4 i1 * i4 - 6 i2^2 + 2 i2 * i3 + 4 i2 * i4 - 8 i3^2 - 2 i3 * i4 - 6 i4^2 
  ] >= -3.1e2
 e4: [ - 8 i1^2 - 2 i1 * i2 - 2 i1 * i3 + 6 i1 * i4 - 4 i2^2 - 4 i2 * i3 + 2 i2 * i4 - 9 i3^2 - 2 i3 * i4 
  - 7 i4^2 ] >= -4.6e2

Bounds
 i1 <= 200
 i2 <= 200
 i3 <= 200
 i4 <= 200

General
 i1 i2 i3 i4

End
