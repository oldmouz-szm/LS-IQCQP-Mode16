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
 obj: - i2 - i3 - i4 - i5 - i6 - i7 - i8 - i9 - i10 - i11

Subject To
 e2: - .987420882906575 i2 - .987420882906575 i3 - .987420882906575 i4 - .987420882906575 i5 - .987420882906575 i6 
  - .987420882906575 i7 - .987420882906575 i8 - .987420882906575 i9 - .987420882906575 i10 - .987420882906575 i11
   + [ i2^2 + i3^2 + i4^2 + i5^2 + i6^2 + i7^2 + i8^2 + i9^2 + i10^2 + i11^2 ] <= 0

Bounds
 -1 <= i2 <= 1
 -1 <= i3 <= 1
 -1 <= i4 <= 1
 -1 <= i5 <= 1
 -1 <= i6 <= 1
 -1 <= i7 <= 1
 -1 <= i8 <= 1
 -1 <= i9 <= 1
 -1 <= i10 <= 1
 -1 <= i11 <= 1

General
 i2 i3 i4 i5 i6 i7 i8 i9 i10 i11

End
