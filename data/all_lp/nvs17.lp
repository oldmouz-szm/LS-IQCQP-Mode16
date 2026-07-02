\ Equation counts
\     Total        E        G        L        N        X        C        B
\         7        0        7        0        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\         7        0        0        7        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\        56        0       56        0
\ 
Minimize
 obj: 14.2 i1 - 11.6 i2 + 18.4 i3 - 24.8 i4 - 132.8 i5 - 84.4 i6 - 88 i7 + [ 14 i1^2 - 12 i1 * i3 + 4 i1 * i4 
  - 8 i1 * i5 + 4 i1 * i6 - 4 i1 * i7 + 12 i2^2 + 8 i2 * i3 - 4 i2 * i5 - 8 i2 * i6 - 12 i2 * i7 + 16 i3^2 
  + 4 i3 * i4 - 12 i3 * i5 - 8 i3 * i6 - 4 i3 * i7 + 12 i4^2 - 4 i4 * i6 + 14 i5^2 + 12 i5 * i6 + 8 i5 * i7 
  + 8 i6^2 + 8 i6 * i7 + 12 i7^2 ]/2

Subject To
 e1: [ - 9 i1^2 - 10 i1 * i2 - 6 i1 * i3 - 10 i1 * i4 - 6 i1 * i6 + 6 i1 * i7 - 8 i2^2 - 10 i2 * i3 - 6 i2 * i4 
  - 2 i2 * i5 - 2 i2 * i6 + 2 i2 * i7 - 5 i3^2 - 2 i3 * i4 + 4 i3 * i7 - 7 i4^2 - 2 i4 * i6 + 2 i4 * i7 
  - 7 i5^2 - 4 i5 * i7 - 5 i6^2 + 4 i6 * i7 - 8 i7^2 ] >= -1.93e3
 e2: [ - 6 i1^2 - 6 i1 * i2 - 2 i1 * i3 + 2 i1 * i4 - 2 i1 * i5 - 6 i1 * i7 - 6 i2^2 - 2 i2 * i3 + 10 i2 * i4 
  - 6 i2 * i5 - 2 i2 * i6 - 10 i2 * i7 - 4 i3^2 + 8 i3 * i6 - 2 i3 * i7 - 8 i4^2 + 6 i4 * i5 + 2 i4 * i6 
  + 10 i4 * i7 + 7 i5^2 - 4 i5 * i6 - 10 i5 * i7 - 8 i6^2 - 8 i7^2 ] >= -2.94e3
 e3: [ - 9 i1^2 + 2 i1 * i2 + 4 i1 * i4 - 6 i1 * i5 + 2 i1 * i6 - 6 i2^2 + 2 i2 * i3 + 4 i2 * i4 - 2 i2 * i5 
  + 4 i2 * i6 + 2 i2 * i7 - 8 i3^2 - 2 i3 * i4 - 4 i3 * i7 - 6 i4^2 + 4 i4 * i5 - 6 i4 * i6 + 6 i5^2 
  - 2 i5 * i6 - 6 i5 * i7 - 5 i6^2 - 4 i6 * i7 - 7 i7^2 ] >= -1.84e3
 e4: [ - 8 i1^2 - 2 i1 * i2 - 2 i1 * i3 + 6 i1 * i4 - 6 i1 * i5 - 10 i1 * i6 + 6 i1 * i7 - 4 i2^2 - 4 i2 * i3 
  + 2 i2 * i4 - 4 i2 * i5 - 2 i2 * i7 - 9 i3^2 - 2 i3 * i4 - 2 i3 * i5 - 10 i3 * i6 - 2 i3 * i7 - 7 i4^2 
  + 6 i4 * i5 + 4 i4 * i6 + 6 i5^2 - 2 i5 * i6 + 6 i5 * i7 - 7 i6^2 + 2 i6 * i7 - 6 i7^2 ] >= -1.65e3
  
 e5: [ - 4 i1^2 + 2 i1 * i2 - 6 i1 * i3 - 2 i1 * i4 - 4 i1 * i5 + 4 i1 * i6 + 4 i1 * i7 - 5 i2^2 + 6 i2 * i4 
  + 2 i2 * i5 - 4 i2 * i6 - 4 i2 * i7 - 8 i3^2 - 2 i3 * i4 - 6 i3 * i5 + 6 i3 * i6 - 4 i3 * i7 - 6 i4^2 
  - 8 i4 * i5 + 4 i4 * i7 - 7 i5^2 + 4 i5 * i6 + 4 i5 * i7 - 7 i6^2 + 4 i6 * i7 - 8 i7^2 ] >= -1.22e3
  
 e6: [ - 7 i1^2 + 2 i1 * i2 - 6 i1 * i3 - 2 i1 * i4 - 2 i1 * i5 + 2 i1 * i6 - 7 i2^2 - 2 i2 * i3 + 2 i2 * i4 
  - 4 i2 * i6 + 4 i2 * i7 - 6 i3^2 - 2 i3 * i4 - 4 i3 * i5 + 4 i3 * i6 - 4 i3 * i7 - 5 i4^2 + 2 i4 * i5 
  + 2 i4 * i6 + 4 i4 * i7 - 5 i5^2 + 6 i5 * i6 - 4 i5 * i7 - 9 i6^2 + 8 i6 * i7 - 6 i7^2 ] >= -1.02e3
  
 e7: [ - 9 i1^2 - 4 i1 * i2 + 4 i1 * i3 + 4 i1 * i4 - 2 i1 * i5 - 8 i1 * i6 - 4 i1 * i7 - 15 i2^2 + 2 i2 * i3 
  - 12 i2 * i5 + 2 i2 * i6 - 6 i2 * i7 - 7 i3^2 + 4 i3 * i4 - 4 i3 * i5 - 2 i3 * i7 - 7 i4^2 + 10 i4 * i7 
  - 8 i5^2 - 2 i5 * i6 - 2 i5 * i7 - 6 i6^2 + 2 i6 * i7 ] >= -2.74e3

Bounds
 i1 <= 200
 i2 <= 200
 i3 <= 200
 i4 <= 200
 i5 <= 200
 i6 <= 200
 i7 <= 200

General
 i1 i2 i3 i4 i5 i6 i7

End
