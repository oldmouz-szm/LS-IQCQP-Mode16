\ Equation counts
\     Total        E        G        L        N        X        C        B
\        11        0        0       11        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\        10        0        0       10        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\       111      104        7        0
\ 
Minimize
 obj: - 20 i1 - 80 i2 - 20 i3 - 50 i4 - 60 i5 - 90 i6 + 0 i7 + 10 i8 + 10 i9 + 10 i10 + [ 10 i1^2 + 10 i2^2 
  + 10 i3^2 + 10 i4^2 + 10 i5^2 + 10 i6^2 + 10 i7^2 ]/2

Subject To
 e1: - 2 i1 - 6 i2 - i3 - 3 i5 - 3 i6 - 2 i7 - 6 i8 - 2 i9 - 2 i10 <= -4
 e2: 6 i1 - 5 i2 + 8 i3 - 3 i4 + i6 + 3 i7 + 8 i8 + 9 i9 - 3 i10 <= 22
 e3: - 5 i1 + 6 i2 + 5 i3 + 3 i4 + 8 i5 - 8 i6 + 9 i7 + 2 i8 - 9 i10 <= -6
 e4: 9 i1 + 5 i2 - 9 i4 + i5 - 8 i6 + 3 i7 - 9 i8 - 9 i9 - 3 i10 <= -23
 e5: - 8 i1 + 7 i2 - 4 i3 - 5 i4 - 9 i5 + i6 - 7 i7 - i8 + 3 i9 - 2 i10 <= -12
 e6: - 7 i1 - 5 i2 - 2 i3 - 6 i5 - 6 i6 - 7 i7 - 6 i8 + 7 i9 + 7 i10 <= -3
 e7: i1 - 3 i2 - 3 i3 - 4 i4 - i5 - 4 i7 + i8 + 6 i9 <= 1
 e8: i1 - 2 i2 + 6 i3 + 9 i4 - 7 i6 + 9 i7 - 9 i8 - 6 i9 + 4 i10 <= 12
 e9: - 4 i1 + 6 i2 + 7 i3 + 2 i4 + 2 i5 + 6 i7 + 6 i8 - 7 i9 + 4 i10 <= 15
 e10: i1 + i2 + i3 + i4 + i5 + i6 + i7 + i8 + i9 + i10 <= 9
 e11: - i1 - i2 - i3 - i4 - i5 - i6 - i7 - i8 - i9 - i10 <= -1

Bounds
 i1 <= 1
 i2 <= 1
 i3 <= 1
 i4 <= 1
 i5 <= 1
 i6 <= 1
 i7 <= 1
 i8 <= 1
 i9 <= 1
 i10 <= 1

General
 i1 i2 i3 i4 i5 i6 i7 i8 i9 i10

End
