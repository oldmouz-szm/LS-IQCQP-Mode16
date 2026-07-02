\ Equation counts
\     Total        E        G        L        N        X        C        B
\         8        0        1        7        0        0        0        0
\ 
\ Variable counts
\                  x        b        i      s1s      s2s       sc       si
\     Total     cont   binary  integer     sos1     sos2    scont     sint
\         6        0        0        6        0        0        0        0
\ 
\ Nonzero counts
\     Total    const       NL      DLL
\        31       21       10        0
\ 
Minimize
 obj: 8e3 i1 - 3.3e2 i2 - 3.6e2 i3 - 3.7e2 i4 - 415 i5 - 435 i6

Subject To
 e2: 3.3e2 i2 + 3.6e2 i3 + 3.7e2 i4 + 415 i5 + 435 i6 <= 8e3
 e3: 3.3e2 i2 + 3.6e2 i3 + 3.7e2 i4 + 415 i5 + 435 i6 >= 7.7e3
 e4: i2 + i3 + i4 + i5 + i6 <= 20
 e5: [ - i1 * i2 ] <= -60
 e6: [ - i1 * i3 ] <= -30
 e7: [ - i1 * i4 ] <= -75
 e8: [ - i1 * i5 ] <= -30
 e9: [ - i1 * i6 ] <= -100

Bounds
 1 <= i1 <= 100
 1 <= i2 <= 100
 1 <= i3 <= 100
 1 <= i4 <= 100
 1 <= i5 <= 100
 1 <= i6 <= 100

General
 i1 i2 i3 i4 i5 i6

End
