\ Equation counts
\     Total        E        G        L        N        X        C        B
\         1        0        0        1        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\        20        0        0       20        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\        40       20       20        0
\ 
Minimize
 obj: 19 i2 + 18 i3 + 17 i4 + 16 i5 + 15 i6 + 14 i7 + 13 i8 + 12 i9 + 11 i10 + 10 i11 + 9 i12 + 8 i13 
  + 7 i14 + 6 i15 + 5 i16 + 4 i17 + 3 i18 + 2 i19 + i20 + 20 i21

Subject To
 e2: - 98 i2 - 98 i3 - 98 i4 - 98 i5 - 98 i6 - 98 i7 - 98 i8 - 98 i9 - 98 i10 - 98 i11 - 98 i12 - 98 i13 
  - 98 i14 - 98 i15 - 98 i16 - 98 i17 - 98 i18 - 98 i19 - 98 i20 - 98 i21 + [ 100 i2^2 - 4 i2 * i21 + 100 i3^2 
  - 4 i3 * i4 + 100 i4^2 + 100 i5^2 - 4 i5 * i6 + 100 i6^2 + 100 i7^2 - 4 i7 * i8 + 100 i8^2 + 100 i9^2 
  - 4 i9 * i10 + 100 i10^2 + 100 i11^2 - 4 i11 * i12 + 100 i12^2 + 100 i13^2 - 4 i13 * i14 + 100 i14^2 
  + 100 i15^2 - 4 i15 * i16 + 100 i16^2 + 100 i17^2 - 4 i17 * i18 + 100 i18^2 + 100 i19^2 - 4 i19 * i20 
  + 100 i20^2 + 100 i21^2 ] <= -1

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
 -100 <= i12
 -100 <= i13
 -100 <= i14
 -100 <= i15
 -100 <= i16
 -100 <= i17
 -100 <= i18
 -100 <= i19
 -100 <= i20
 -100 <= i21

General
 i2 i3 i4 i5 i6 i7 i8 i9 i10 i11 i12 i13 i14 i15 i16 i17 i18 i19 i20 i21

End
