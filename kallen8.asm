FUN [30] 2
200             ;array start
206             ;array end addr
FUN [60] 2
200
206
FUN [100] 2
200
206
HALT








MOV BX [29]     ;get array
MOV CX [28]
GET
CMP AX 0
JE [47]
MOV [BX] AX
CMP BX CX
JE [47]
ADD BX 1
JMP [34]
RET [27]











MOV BX [59]     ;sort array
CMP BX [58]       
JE [84]          
MOV CX [BX+1]   
CMP CX [BX]     
JAE [80]
MOV AX [BX]
MOV [BX] CX
ADD BX 1
MOV [BX] AX
ADD DX 1
JMP [62]
ADD BX 1
JMP [62]
CMP DX 1
JE [94]
MOV BX [59]
MOV DX 0
JMP [60]
RET [57]




MOV BX [99]     ;print array
MOV AX [BX]
PUT
CMP BX [98]
JE [112]
ADD BX 1
JMP [102]
RET [97]
