\ Equation counts
\     Total        E        G        L        N        X        C        B
\         5        0        0        5        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\        10        0        0       10        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\        51       41       10        0
\ 
Minimize
 obj: - 5.83e-2 i1 + .2318 i2 - 1.08e-2 i3 + .1634 i4 - .138 i5 + .357 i6 - .1953 i7 - .361 i8 + .1824 i9 
  - .162 i10 + [ 1.1e-3 i1^2 + 3.8e-3 i2^2 + 4e-4 i3^2 + 1.9e-3 i4^2 + 9.2e-3 i5^2 + 7e-3 i6^2 + 6.3e-3 i7^2 
  + 9.5e-3 i8^2 + 9.6e-3 i9^2 + 6e-3 i10^2 ]/2

Subject To
 e1: 8 i1 + 7 i2 + 9 i3 + 9 i5 + 8 i6 + 2 i7 + 4 i9 + i10 <= 5.3e2
 e2: 3 i1 + 4 i2 + 6 i3 + 9 i4 + 6 i6 + 9 i7 + i8 + i10 <= 395
 e3: 2 i2 + i3 + 5 i4 + 5 i5 + 7 i7 + 4 i8 + 2 i9 <= 3.5e2
 e4: 5 i1 + 7 i3 + i4 + 7 i5 + 5 i6 + 7 i8 + 9 i9 + 5 i10 <= 405
 e5: i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 <= 200

Bounds
 i1 <= 100
 i2 <= 100
 i3 <= 100
 i4 <= 100
 i5 <= 100
 i6 <= 100
 i7 <= 100
 i8 <= 100
 i9 <= 100
 i10 <= 100

General
 i1 i2 i3 i4 i5 i6 i7 i8 i9 i10

End
