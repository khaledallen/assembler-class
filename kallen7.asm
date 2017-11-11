GET		;user sets top bound
MOV [100] AX	;place top bound into [100]
GET		;user sets lower bound
MOV [101] AX	;place lower bound into [101]
FUN [30] 1	;call checkNumInBounds
100		;max bound
GET		;user sets new top bound
MOV [110] AX	
GET		;user sets new low bound
MOV [111] AX
MOV BX 110	;move location of top bound into BX
FUN [50] 2	;call checkNumv2
[BX]		;pass param1 by reference
[BX+1]		;pass param2 by reference
HALT




MOV BX [29]	;function checkNumInBounds(max,min) by ref
GET             ;request user input
CMP AX [BX]	;compare to max (by reference)
JA [30]		;if AX > max, request new input
CMP AX [BX+1]	;compare to min (by reference)
JB [30]		;if AX < min, request new input
PUT		;print AX if within bounds
MOV [BX] AX
RET [99]






GET		;function checkNumv2
CMP AX [49]
JA [50]
CMP AX [48]
JB [50]
PUT
MOV [BX] AX
RET [109]
