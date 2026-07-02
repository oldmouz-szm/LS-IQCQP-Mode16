\ Equation counts
\     Total        E        G        L        N        X        C        B
\         1        0        0        1        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\        10        0        0       10        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\        20       10       10        0
\ 
Minimize
 obj: 9 i2 + 8 i3 + 7 i4 + 6 i5 + 5 i6 + 4 i7 + 3 i8 + 2 i9 + i10 + 10 i11

Subject To
 e2: - 98 i2 - 98 i3 - 98 i4 - 98 i5 - 98 i6 - 98 i7 - 98 i8 - 98 i9 - 98 i10 - 98 i11 + [ 100 i2^2 - 4 i2 * i11 
  + 100 i3^2 - 4 i3 * i4 + 100 i4^2 + 100 i5^2 - 4 i5 * i6 + 100 i6^2 + 100 i7^2 - 4 i7 * i8 + 100 i8^2 
  + 100 i9^2 - 4 i9 * i10 + 100 i10^2 + 100 i11^2 ] <= -1

Bounds
 -100 <= i2
 -100 <= i3
 -100 <= i4
 -100 <= i5
 -100 <= i6
 -100 <= i7
 -100 <= i8
 -100 <= i9
 -100 <= i10
 -100 <= i11

General
 i2 i3 i4 i5 i6 i7 i8 i9 i10 i11

End
