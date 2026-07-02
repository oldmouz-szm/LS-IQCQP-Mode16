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
\        56       46       10        0
\ 
Minimize
 obj: 48 i1 + 42 i2 + 48 i3 + 45 i4 + 44 i5 + 41 i6 + 47 i7 + 42 i8 + 45 i9 + 46 i10 + [ 100 i1^2 + 100 i2^2 
  + 100 i3^2 + 100 i4^2 + 100 i5^2 + 100 i6^2 + 100 i7^2 + 100 i8^2 + 100 i9^2 + 100 i10^2 ]/2

Subject To
 e1: - 2 i1 - 6 i2 - i3 - 3 i5 - 3 i6 - 2 i7 - 6 i8 - 2 i9 - 2 i10 <= -4
 e2: 6 i1 - 5 i2 + 8 i3 - 3 i4 + i6 + 3 i7 + 8 i8 + 9 i9 - 3 i10 <= 22
 e3: - 5 i1 + 6 i2 + 5 i3 + 3 i4 + 8 i5 - 8 i6 + 9 i7 + 2 i8 - 9 i10 <= -6
 e4: 9 i1 + 5 i2 - 9 i4 + i5 - 8 i6 + 3 i7 - 9 i8 - 9 i9 - 3 i10 <= -23
 e5: - 8 i1 + 7 i2 - 4 i3 - 5 i4 - 9 i5 + i6 - 7 i7 - i8 + 3 i9 - 2 i10 <= -12

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
