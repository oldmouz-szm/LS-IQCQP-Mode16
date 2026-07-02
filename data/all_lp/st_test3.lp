\ Equation counts
\     Total        E        G        L        N        X        C        B
\        10        0        0       10        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\        13        0        0       13        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\        41       36        5        0
\ 
Minimize
 obj: 5 i1 - 5 i2 + 5 i3 + 5 i4 - i5 + i6 - i7 - i8 - i9 - i10 - i11 - i12 - i13 + [ 10 i1^2 + 10 i2^2 
  + 10 i3^2 + 10 i4^2 + 18 i10^2 ]/2

Subject To
 e1: 2 i1 + 2 i2 + i10 + i11 <= 10
 e2: 2 i1 + 2 i3 + i10 + i12 <= 10
 e3: 2 i2 + 2 i3 + i11 + i12 <= 10
 e4: - 8 i1 + i10 <= 0
 e5: - 8 i2 + i11 <= 0
 e6: - 8 i3 + i12 <= 0
 e7: - 2 i4 - i5 + i10 <= 0
 e8: - 2 i6 - i7 + i11 <= 0
 e9: - 2 i8 - i9 + i12 <= 0
 e10: i13 <= 1

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
 i10 <= 1e15
 i11 <= 1e15
 i12 <= 1e15
 i13 <= 1

General
 i1 i2 i3 i4 i5 i6 i7 i8 i9 i10 i11 i12 i13

End
