FUN [20] 2	;call checkNumInBounds
100		;max bound
0		;min bound
FUN [20] 2	;call checkNumInBounds
35		;max bound
25		;min bound
HALT




		;function checkNumInBounds(max,min)
GET             ;request user input
CMP AX [19]	;compare to max
JA [20]		;if AX > max, request new input
CMP AX [18]	;compare to min
JB [20]		;if AX < min, request new input
PUT		;print AX if within bounds
RET [99]
