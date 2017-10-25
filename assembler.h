
//Machine Code for the REGISTERS
const int AXREG = 0;
const int BXREG = 1;
const int CXREG = 2;
const int DXREG = 3;

//Machine Code for OPERANDS
const int CONSTANT = 0x07;				// 00000111
const int ADDRESS = 0x06;				// 00000110

/******************************
 * Machine Codes for Various Commands
 * Organized by Position in the command Bit
 */

/* Top Bits */
const int HALT = 0x05;					// 00000101
const int MOVREG = 0xc0;				// 11000000
const int ADD = 0xa0;					// 10100000
const int SUB = 0x80;					// 10000000
const int OR = 0x20;					// 00100000
const int AND = 0x40;					// 01000000
const int MOVMEM = 0xe0;				// 11100000
const int COMPARE = 0x60;				// 01100000
const int SPECIAL = 0x00;				// 00000000

/* Mid Bits */
const int ZERO_OPS = 0x00;				// 00000000
const int JUMP_INST = 0x08;				// 00001000

/* Bottom Bits */
const int PUT = 0x07;					// 00000111
const int GET = 0x06;					// 00000110
const int JUMP_EQUAL = 0x00;				// 00000000
const int JUMP_NOT_EQUAL = 0x01;			// 00000001
const int JUMP_BELOW = 0x02;				// 00000010
const int JUMP_BELOW_EQ = 0x03;				// 00000011
const int JUMP_ABOVE = 0x04;				// 00000100
const int JUMP_ABOVE_EQ = 0x05;				// 00000101
const int JUMP = 0x06;					// 00000110
const int FUN = 0x03;					// 00000011
const int RET = 0x04;					// 00000100

enum paramType {reg, mem, constant, arrayBx, arrayBxPlus, none};

typedef short int Memory;

class Registers						//Class defines object to hold
{							//all registers, flags, instruction ptr
public:
	int AX;						//Holds current value of registers
	int BX;
	int CX;
	int DX;
	int EX;						//Stack pointer
	int instrAddr;					//Address of current instruction
	int flag;					//Current value of flag

	int getReg(int i);				//Takes the machine code value of a register and returns its value
	void setReg(int input, int reg);		//Sets the given register index to the providede value
	void pushStack( int reg);			//Leverages the stack pointer to build the stack
	void popStack();				//Decrements EX and clears memory
}regis;

