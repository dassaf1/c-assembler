MAT1:   .mat [2][3] 1,2,3,4,5,6
    .mat [2][3]
cmp r1,STR1[r4][r4]
Start:  dec DATA4
    .extern DATA4
Next:   cmp  #5,#6
DATA1:  .data   100,   200 ,  -500, 300
Y:      .extern DATA8
STR1:   .string "abc, ,defg"


;
X:   cmp r1,r2
    .mat  [2][2] 1, 2, 511, 3
stop