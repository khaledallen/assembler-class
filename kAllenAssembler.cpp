//*********************************
//Name: Khaled Allen
//Project: Assembler Part 3
//Due Date: 9/22/17
//Filename: kAllenAssembler.cpp
//INPUT FILENAME: kallen3.asm
//OUTPUT FILENAME: -
//DESCRIPTION:
//This is an x86 assembler simulation written in C++
//To use it, 
//1. You can write an assembly code file (.asm) using basic x86 syntax.
//2. Place the .asm file in the same directory as this assembler
//3. Change the ASM_FILE_NAME (line 24) to a string that is your .asm filename
//4. Compile and run this file.
//5. You can change the .asm file without recompiling this assembler
//**************************************************

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <ctype.h>

#include "assembler.h"

using namespace std;
char ASM_FILE_NAME[] = "kallen6.asm";			// input filename, .asm

const int MAX = 150;  					//size of simulators memory
const int COL = 7;    					//number of columns for output

Memory memory[MAX] = {0};				//Array of size MAX, simulates memory of computer
int address;						//The current address the assembler is looking at

/*********************
 * Function Prototype List
 * See functin definition for usage instructions
 */
void printMemoryDump();					//prints memeory with integers commands
void convertToMachineCode(ifstream &fin);		//Converts .asm commands into machine code values and inserts into memory
int convertToNumber(string line, int &start);		//Converts a string into a number
int whichReg(char regLetter);  				//returns the number of the letter registar
void fillMemory( );					//Populates the memory array
void splitCommand( string &line, string &command );	//Parses .asm commands and extracts individual parts
void runCode( );					//Interprets the machine code values and manipulates the memory
void changeToLowerCase(string &line);			//Converts .asm commands into lowercase
bool isNumber(string string);				//Checks if input string is a number
int stripBrackets(string address);			//Strips brackets from .asm input and returns the number contained
bool isAddress(string string);				//Checks if input string is an address
void buildBotBits(string oper2, int &machineCode);      //Builds the bottom bits of the 2 operand instructions
void setFlag(int reg, int botBits, int &address);	//Hangles the logic for the compare machine code
int doMath( int arg1, int arg2, int operation);	        //Handles the logic fo the OR, AND, ADD, and SUB machine code
void jumpBuilder(string commArr[], int &machineCode, int &address);	//Builds jump commands for the assembly of the code
void doJump( int botBits, int &address);		//Handles the logic for running Jump Code
void functionBuilder(ifstream &fin, int numParams);     //Builds the funtion in memory
void registerDump();					//Dumps the registers, flag, and return address onto the stack
void registerRestore();					//Restores the state after a function

int main( )
{
	cout << "Initial Memory State" << endl;
	printMemoryDump( );				//Print initial memory state (should be all 0's)
	fillMemory( );					//Assemble code and fill memory
	cout << "^^^^^^^^^^^^^^^^^^^^^^^^^ MEMORY AFTER ASSEMBLY ^^^^^^^^^^^^^^^^^^^^^" << endl << endl;
	printMemoryDump( );				
	cout << "^^^^^^^^^^^^^^^^^^^^^^^^^ OUTPUT OF CODE ^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << endl << endl;
	runCode( );
	cout << "^^^^^^^^^^^^^^^^^^^^^^^^^ MEMORY AFTER RUNCODE ^^^^^^^^^^^^^^^^^^^^^^" << endl << endl;
	printMemoryDump( );
	cout << endl;
	system( "pause" );				//Needed for Windows Machines
	return 0;
}
/****************************************
 * int Registers::getReg(int i)
 * Description:
 * Takes the machine code value of a register
 * Returns the value of that register
 */
int Registers::getReg(int i)
{
	if(i > 3 || i < 0)
	{
		cout << "Error. No register with index " << i << ". Aborting." << endl;
	}
	switch(i)
	{
		case(0): return AX;
		case(1): return BX;
		case(2): return CX;
		case(3): return DX;
	}
	return 0;
}
/******************************
 * void Registers::setReg(int input, int reg)
 * Description:
 * Sets the specified register to the provided value
 * Parameters:
 * int input - the value to be set into the register
 * int reg - the index (0-3) of the register to be set
 */
void Registers::setReg(int input, int reg)
{
	if(reg > 3 || reg < 0)
	{
		cout << "Error. No register with inder " << reg << ". Aborting." << endl;
	}
	switch(reg)
	{
		case(0): AX = input;
			 break;
		case(1): BX = input;
			 break;
		case(2): CX = input;
			 break;
		case(3): DX = input;
			 break;
		case(4): EX = input;
			 break;
	}
}
/******************************
* void Registers::pushStack(reg)
* Description:
* Builds the stack using the stack pointer
* Paramters:
* reg - the register name to be put into the stack
*/
void Registers::pushStack( int val)
{
	EX--;
	memory[EX] = val;
}

/******************************
 * void Registers::popStack();
 * Description:
 * Decrements EX and clears what it was pointing to
 * Parameters:
 * None
 */
void Registers::popStack()
{
	memory[EX] = 0x00;
	EX++;
}
			
/************************************************************/
/*fillMemory						    */
/*changes the code to machine code and places the
 * commands into the memory. */
void fillMemory( )
{
	address = 0;
	ifstream fin;
	fin.open( ASM_FILE_NAME );
	if (fin.fail( ))
	{
		cout << "Error, file didn't open\n";
		system( "pause" );
		exit( 1 );
	}

	for (int i = 0; i< MAX && !fin.fail( ); i++)
	{
		convertToMachineCode( fin );
	}
	
}

/**************************************************/
/* splitCommand
 * Description:
 * Parses the line of assembler code
 * Parameters:
 * line - MODIFIED, is the line to be parsed, it is returned 1 command shorter
 * command - MODIFIED, is the command that is returned, clipped off the end
 */
void splitCommand( string &line, string &command )
{
	int space = line.find( ' ' );				//the position of the first space in the command
	if(space != -1)
	{
		command = line.substr( 0, space );
		line = line.substr(space + 1);
	} else {
		command = line.substr ( 0 );
		line = "";
	}
}
/***************************************************/
/* convertToMachineCode
 * Description:
 * processes a single line of asm code and converts it to machine values
 * Parameters:
 * fin - MODIFIED, is the asm file to be processed
 */
void convertToMachineCode( ifstream &fin )
{
	string line;  						//full line from .asm file
	string commArr[3];					//array to hold command and assign indeces
	string command; 					//the asm commmand
	string oper1, oper2;  					//the two operands could be empty
	int machineCode = 0;					//start building the machine code
	getline( fin, line, '\n' );
	changeToLowerCase( line );

	int i = 0;						//i is the number of arguments found
	while( line.length() > 0 && i < 3)
	{
		splitCommand( line, command );
		commArr[i] = command; 
		i++;
	} 

	command = commArr[0];					//set the command to the first argument of the array
	if(i == 0)						//if i is 0, the command was empty. Step address
	{
		address++;
	}
	if(i == 1)						//if i is 1, it is a no operand command (or a value)
	{
		if(isNumber(commArr[0])) {			//if the first command is a number, it's just a value to put in memory
			memory[address] = stoi(commArr[0]);
			address++;
		}
	}
	
	if(i > 1)						//if i was greater than 1, there were two parts to the asm command
	{
		oper1 = commArr[1];
	}
	if(i > 2)						//if i was greater than 2, there were three parts to the asm command
	{
		oper2 = commArr[2];
	}
	if (command[0] == 'h')  				//halt
	{
		memory[address] = HALT;
		address++;
	}
	if (command[0] == 'm' && !isAddress(commArr[1]))  				//move
	{
		machineCode = MOVREG;
		machineCode += (whichReg( oper1[0] ) << 3);
		buildBotBits(oper2, machineCode);
		address++;
	}
	if (command[0] == 'm' && isAddress(commArr[1]))	 		// The second arg is an address 
	{
		machineCode = MOVMEM;
		machineCode += (whichReg( oper2[0] ) << 3);
		machineCode += ADDRESS;
		memory[address] = machineCode;
		address++;
		memory[address] = stripBrackets(commArr[1]);
		address++;
	}
	if (command == "add")	 					//add
	{
		machineCode = ADD;
		machineCode += (whichReg ( oper1[0] )) << 3;
		buildBotBits(oper2, machineCode);
		address++;
	}
	if (command == "sub")	 					//subtract
	{
		machineCode = SUB;
		machineCode += (whichReg ( oper1[0] )) << 3;
		buildBotBits(oper2, machineCode);
		address++;
	}
	if (command == "and")	 					//logical and
	{
		machineCode = AND;
		machineCode += (whichReg ( oper1[0] )) << 3;
		buildBotBits(oper2, machineCode);
		address++;
	}
	if (command == "or")	 					//logical or
	{
		machineCode = OR;
		machineCode += (whichReg ( oper1[0] )) << 3;
		buildBotBits(oper2, machineCode);
		address++;
	}
	if (command[0] == 'c')						//compare
	{
		machineCode = COMPARE;
		machineCode += (whichReg ( oper1[0] )) << 3;
		buildBotBits(oper2, machineCode);
		address++;
	}
	if (command[0] == 'j')						//jump instructions
	{
		machineCode = JUMP_INST;
		jumpBuilder(commArr, machineCode, address);
		address++;
	}
	if (command == "put")
	{
		machineCode = PUT;
		memory[address] = machineCode;
		address++;
	}
	if (command == "get")
	{
		machineCode = GET;
		memory[address] = machineCode;
		address++;
	}
	if (command == "fun")
	{
		machineCode = FUN;
		memory[address] = machineCode;
		address++;
		machineCode = stripBrackets(oper1);
		memory[address] = machineCode;
		address++;
		int numParams = stoi(oper2);
		machineCode = numParams;
		memory[address] = machineCode;
		address++;
		functionBuilder(fin, numParams);
		
	}
	if (command == "ret")
	{
		machineCode = RET;
		memory[address] = machineCode;
		address++;
		machineCode = stripBrackets(oper1);
		memory[address] = machineCode;
		address++;
	}

}
/****************************************
 * functionBuilder
 * Description: Builds the machine code for functions
 * Parameters:
 * numParams - the number of parameters in the function
 * fin - the input file stream from the asm file
 * Return - n/a. Modifies memory[] by inserting machinecode
 * for the function
 */
void functionBuilder(ifstream &fin, int numParams)
{
	string line;
	int functionCode = 0x00;
	for(int i = 0; i < numParams; i++)
	{
		getline(fin, line, '\n');
		changeToLowerCase(line);
		buildBotBits(line, functionCode);
		address++;
		functionCode = 0x00;
	}
}
/****************************************
 * buildBotBits
 * Description: Adds the bottom bits of the 2 operand instructions
 * Paramters:
 * oper2 - the second operator from the command Array
 * int machineCode - the value being built 
 * int addrss - the address pointer
 *
 */
void buildBotBits(string oper2, int &machineCode)
{
		if(isNumber(oper2))
		{
			machineCode += CONSTANT;
			memory[address] = machineCode;
			address++;
			memory[address] = stoi(oper2);
		}
		else if(isAddress(oper2))
		{
			machineCode += ADDRESS;
			memory[address] = machineCode;
			address++;
			memory[address] = stripBrackets(oper2);
		}
		else
		{
			machineCode += whichReg(oper2[0]);
			memory[address] = machineCode;
		}
}
/******************************
 * jumpBuilder
 * Description: Processes the type of jump and builds the correct 
 * 		memory values to input into memory
 * Parameters:
 * string command - the name of the jump in the asm file
 * int address - the address pointer
 * int machineCode - the machine code entry being built
 */
void jumpBuilder(string commArr[], int &machineCode, int &address)
{
	string command = commArr[0];
	int targetAddress = stripBrackets(commArr[1]);

		if(command == "je")
		{
			machineCode += JUMP_EQUAL;
		}
		if(command == "jne")
		{
			machineCode += JUMP_NOT_EQUAL;
		}
		if(command == "jb")
		{
			machineCode += JUMP_BELOW;
		}
		if(command == "jbe")
		{
			machineCode += JUMP_BELOW_EQ;
		}
		if(command == "ja")
		{
			machineCode += JUMP_ABOVE;
		}
		if(command == "jae")
		{
			machineCode += JUMP_ABOVE_EQ;
		}
		if(command == "jmp")
		{
			machineCode += JUMP;
		}

	memory[address] = machineCode;
	address++;
	memory[address] = targetAddress; 
}
/*********************************
 * stripBrackets
 * Description:
 * Removes the brackets from memory addresses
 * Parameters:
 * address - the string to be stipped of brackets
 * Returns:
 * A number extracted from the brackets
 */
int stripBrackets(string address)
{
	string temp;						//a string to hold the chars between the brackets
	int i = 0;

	for(i = 0; i < address.length(); i++) {
		if(address[i] == '[' || address[i] == ']' ) {
			i++;
		}
		temp += address[i];
	}
	return stoi(temp);
}	
	

/************************************************************
 * whichReg
 * Description:
 * Converts a string in the .asm file into the machine code for the register
 * Parameters:
 * regLetter - the first letter of the .asm string for the register
 * Returns:
 * An integer representing the appropriate register's machine code
 */
int whichReg(char regLetter)
{
	if (regLetter == 'a')
	{
		return AXREG;
	}
	else if (regLetter == 'b')
	{
		return BXREG;
	}
	else if (regLetter == 'c')
	{
		return CXREG;
	}
	else if (regLetter == 'd')
	{
		return DXREG;
	}
	return -1;  //something went wrong if -1 is returned
}

/************************************************************/
/*ConvertToNumber															*/
/*  takes in a line and converts digits to a integer			*/
/*  line - is the string of assembly code to convert			*/
/*  start - MODIFIED, is the location where the line is being coverted, 
		it starts at the beginning of number and it passed 
		back at the next location */
/*  value - MODIFIED, is the integer value of the digits in the code
 *  Returns:
 *  value - integer converted from the string
 */

int convertToNumber(string line, int &start)
{
	char number[16];						//character array to hold number
	bool negative = false;						//flag to check if negative
	int value;							//var to hold the numerical version of string
	int i = 0;
	if (line[start] == '-')
	{
		start++;
		negative = true;
	}
	while (i<16 && line.size() > start&& isdigit(line[start]))
	{
		number[i] = line[start];
		i++;
		start++;
	}
	number[i] = '\0';
	value = atoi(number);
	if (negative == true)
	{
		value = -value;
	}
	return value;
}

/************************************************************
 * printMemoryCommands
 * Description: prints memory with letter commands
 */
void printMemoryCommands()
{
	int i= 0;
	int count;  // which column is currently outputting
	int moveColumn = MAX / COL;
	int index=0;
	while ( i< MAX)
	{
		count = 0;
		while (count < COL && index < MAX)
		{
			index = i + count * moveColumn;
			//As each type of instruction is added you will need to be able to deal with type of command
			//printcommand(index)
		}
		cout<<endl;
			
	}
	cout<<"RA "<< regis.AX << "  BX "<< regis.BX << "  CX "<< regis.CX << "  DX "<< regis.DX<<endl;
	cout<<"\tinstruction "<<regis.instrAddr<<"\tflag "<<regis.flag<<"\n\n";
}


/************************************************************
 * printMemoryDump

 * prints memory by number
 * MAX is the amount of elements in the memory array (Vicki used 100) - Defined globally
 * COL is the number of columns that are to be displayed (Vicki used 7; was originally called COLUMNS) - Defined globally
 */
void printMemoryDump()
{
	int numRows=MAX/COL+1;   					//number of rows that will print
	int carryOver=MAX%COL;   					//number of columns on the bottom row
	int location;   						//the current location being called
	for(int row=0;row<numRows;row++)
	{
		location=row;
		for(int column=0;location<MAX&&column<COL;column++)
		{
			if(!(numRows-1==row&&carryOver-1<column))
			{
				cout<<setw(5)<<location<<"."<<setw(5)<<memory[location];
				location+=(numRows-(carryOver-1<column));
			}
		}
		cout<<endl;
	}

	cout<<endl;
	cout<<"AX: "<<regis.AX<<'\t';
	cout<<"BX: "<<regis.BX<<'\t';
	cout<<"CX: "<<regis.CX<<'\t';
	cout<<"DX: "<<regis.DX<<'\t';
	cout<<endl<<endl;
	cout<<"Instruction: "<<regis.instrAddr<<endl;
	cout<<"Flag: "<<regis.flag;
	
	cout<<endl<<endl;
}


/*********************************
 * changeToLowerCase
 * Description:
 * Converts a string to lowercase
 * Parameters:
 * &line - MODIFIED, the string to be converted to lowercase
 */
void changeToLowerCase(string &line)
{
	
	int index=0;
	while (index < line.size())
	{
		line[index] = tolower(line[index]);
		index++;
	}

}

/*********************************
 * isAddress
 * Description:
 * Checks if a string is an address and returns true if it is
 * Paramters:
 * string - the string to be checked
 * Returns:
 * bool - true/false
 */
bool isAddress(string string)
{
	return string[0] == '[';
}

/*********************************
 * isNumber
 * Description:
 * Checks if a string is a number and returns true if it is
 * Parameters:
 * string - the string to be checked
 * Returns:
 * bool - true/false
 */
bool isNumber(string string)
{
	int i = 0, num;
	for (i = 0; i < string.length(); i++ )
	{
		num = string[i];
		if((num < 48 || num > 57) && num != 45)
			return false;
	}
	return true;
}

/*********************************
 * runCode
 * Description:
 * Interprets the values in memory[] as machine code values
 * 	and alters memory[] accordingly
 */
void runCode( )
{
	address = 0;					//start address counter back at 0
	regis.EX = MAX;					//Set the stack pointer to the end of memory
	int topBits, midBits, botBits;			//variables to hold the pieces of the machine code instructions
	int targetAddress;				//variable to hold addresses
	while(memory[address] != HALT) 			//run until HALT command encountered
	{
		topBits = (memory[address] & 224);	//Extract the bits from the machine code
		midBits = (memory[address] & 24) >> 3;	//TODO Make this a standalone function
		botBits = memory[address] & 7;
		//cout << "DEBUG - Address: " << address << endl;
		//cout << "DEBUG - topBits: " << topBits << endl << "DEBUG - midBits: " << midBits << endl << "DEBUG - botBits: " << botBits << endl;
		//printMemoryDump();
		//int temp;
		//cin >> temp;

		if(topBits ==  MOVREG)
		{
			if(botBits == CONSTANT)
			{
				regis.setReg(memory[address+1], midBits);
				address++;
			}
			else if(botBits == ADDRESS) 	//the command is to move from memory
			{
				targetAddress = memory[address+1];  //the target address is always in the next memory location
				regis.setReg(memory[targetAddress], midBits);
				address++;
			}
			else	 			//The command is to move from a register
			{
				regis.setReg(regis.getReg(botBits), midBits);
			}
			address++;
		}
		if(topBits == MOVMEM)
		{
			if(botBits == ADDRESS) 				//The command is to move into an address
			{
				targetAddress = memory[address + 1];
				memory[targetAddress] = regis.getReg(midBits);
				address++;
			}
			address++;
		}
		if(topBits == ADD)
		{
			int input;
			if(botBits == CONSTANT)
			{
				input = doMath(regis.getReg(midBits), memory[address+1], ADD);
				address++;
			}
			else if(botBits == ADDRESS)
			{
				int targetAddress = memory[address+1];
				input = doMath(regis.getReg(midBits), memory[targetAddress], ADD);
				address++;
			}
			else
			{
				input = doMath(regis.getReg(midBits), regis.getReg(botBits), ADD);
			}
			regis.setReg(input, midBits);
		address++;
		}
		if(topBits == SUB)
		{
			int input;
			if(botBits == CONSTANT)
			{
				input = doMath(regis.getReg(midBits), memory[address+1], SUB);
				address++;
			}
			else if(botBits == ADDRESS)
			{
				int targetAddress = memory[address+1];
				input = doMath(regis.getReg(midBits), memory[targetAddress], SUB);
				address++;
			}
			else
			{
				input = doMath(regis.getReg(midBits), regis.getReg(botBits), SUB);
			}
			regis.setReg(input, midBits);
		address++;
		}
		if(topBits == OR)
		{
			int input;
			if(botBits == CONSTANT)
			{
				input = doMath(regis.getReg(midBits), memory[address+1], OR);
				address++;
			}
			else if(botBits == ADDRESS)
			{
				int targetAddress = memory[address+1];
				input = doMath(regis.getReg(midBits), memory[targetAddress], OR);
				address++;
			}
			else
			{
				input = doMath(regis.getReg(midBits), regis.getReg(botBits), OR);
			}
			regis.setReg(input, midBits);
		address++;
		}
		if(topBits == AND)
		{
			int input;
			if(botBits == CONSTANT)
			{
				input = doMath(regis.getReg(midBits), memory[address+1], AND);
				address++;
			}
			else if(botBits == ADDRESS)
			{
				int targetAddress = memory[address+1];
				input = doMath(regis.getReg(midBits), memory[targetAddress], AND);
				address++;
			}
			else
			{
				input = doMath(regis.getReg(midBits), regis.getReg(botBits), AND);
			}
			regis.setReg(input, midBits);
			address++;
		}
		if(topBits == COMPARE)
		{
			setFlag(regis.getReg(midBits), botBits, address);
			address++;
		}
		if(topBits == SPECIAL)
		{
			if(midBits == ZERO_OPS)
			{
				if(botBits == PUT)
				{
					cout << "CONSOLE OUTPUT: Contents of AX - " << regis.AX << endl;
				}
				if(botBits == GET)
				{
					cout << endl;
					cout << "CONSOLE INPUT: The program is requesting user input - : "; 
					cin >> regis.AX;
					cout << endl;
				}
				if(botBits == FUN)
				{
					address++;
					int targetAddress = memory[address];
					address++;
					int numParam = memory[address];
					address++;
					for(int i = 0; i < numParam; i++)
					{
						if(memory[address] == CONSTANT) 
						{
							address++;
							memory[targetAddress - (1 + i)] = memory[address];
						}
						else if (memory[address] == ADDRESS)
						{
							address++;
							memory[targetAddress - (1 + i)] = memory[address];
						}
						else
						{
							memory[targetAddress - (1 + i)] = memory[address];
						}
						address++;
					}
					registerDump();
					address = targetAddress;
					continue;			//Stop the loop and pass control to the function
				}
				if(botBits == RET)
				{
					memory[memory[address+1]] = regis.AX;
					registerRestore();
					continue;
				}
				address++;
			}
			if(midBits == JUMP_INST >> 3)
			{
				doJump(botBits, address);
			}
		}
	}
}
				
/******************************
 * Run Code Utility Function
 */

/******************************
 * registerDump()
 * Description:
 * Dumps the registers into the stack and sets the return address
 * Parameters:
 * None
 */
void registerDump()
{
	regis.pushStack(regis.AX);
	regis.pushStack(regis.BX);
	regis.pushStack(regis.CX);
	regis.pushStack(regis.DX);
	regis.pushStack(regis.flag);
	regis.pushStack(address);
}
/******************************
 * registerRestore()
 * Description:
 * Restores the register state after a function is done running
 * Parameters:
 * None
 */
void registerRestore()
{
	address = memory[regis.EX];
	regis.popStack();
	regis.flag = memory[regis.EX];
	regis.popStack();
	regis.DX = memory[regis.EX];
	regis.popStack();
	regis.CX = memory[regis.EX];
	regis.popStack();
	regis.BX = memory[regis.EX];
	regis.popStack();
	regis.AX = memory[regis.EX];
	regis.popStack();
}

/******************************
 * void setFlag
 * Description:
 * Hangles the logic for the compare function machine code
 * Parameters:
 * int reg - the register being compared, the first argument
 * int botBits - the bottom bits of the machine code command
 * int &address - the address pointer
 */

void setFlag(int reg, int botBits, int &address)
{
	if(botBits == CONSTANT)
	{
		if(reg > memory[address+1])
		{
			regis.flag = 1;
		}
		else if(reg < memory[address+1])
		{
			regis.flag = -1;
		}
		else
		{
			regis.flag = 0;
		}
		address++;
	}
	else if(botBits == ADDRESS)
	{
		int targetAddress = memory[address+1];
		if(reg > memory[targetAddress])
		{
			regis.flag = 1;
		}
		else if(reg < memory[targetAddress])
		{
			regis.flag = -1;
		}
		else
		{
			regis.flag = 0;
		}
		address++;
	}
	else
	{
		if(reg > regis.getReg(botBits) )
		{
			regis.flag = 1;
		}
		else if(reg < regis.getReg(botBits) )
		{
			regis.flag = -1;
		}
		else
		{
			regis.flag = 0;
		}
	}
}

/******************************
 * doJump
 * Description:
 * Handles the logic for all jump commands
 * Parameters:
 * int botBits - the bits containing the specific jump command
 * int address - the address pointer
 */
void doJump( int botBits, int &address)
{
	int targetAddress = memory[address+1];

	switch(botBits){
		case(JUMP_EQUAL):
			if(regis.flag == 0)
			{
				address = targetAddress;
			}
			else
			{
				address += 2;
			}
			break;
		case(JUMP_NOT_EQUAL):
			if(regis.flag != 0)
			{
				address = targetAddress;
			}
			else
			{
				address += 2;
			}
			break;
		case(JUMP_BELOW):
			if(regis.flag < 0)
			{
				address = targetAddress;
			}
			else
			{
				address += 2;
			}
			break;
		case(JUMP_BELOW_EQ):
			if(regis.flag <= 0)
			{
				address = targetAddress;
			}
			else
			{
				address += 2;
			}
			break;
		case(JUMP_ABOVE):
			if(regis.flag > 0)
			{
				address = targetAddress;
			}
			else
			{
				address += 2;
			}
			break;
		case(JUMP_ABOVE_EQ):
			if(regis.flag >= 0)
			{
				address = targetAddress;
			}
			else
			{
				address += 2;
			}
			break;
		case(JUMP):
			address = targetAddress;
			break;
		default:
			address += 2;
	}
}


/******************************
 * doMath
 * Description:
 * Handles the logic for the OR, AND, SUB, and ADD code commands
 * Parameters:
 * int arg1 - the first argument to receive the result of the operation
 * int arg2 - the second argument
 * int operation - the operation to perform
 */
int doMath( int arg1, int arg2, int operation)
{
	int result;				// the final result to be returned
	if(operation == OR)
	{
		result = arg1 | arg2;
	}
	if(operation == AND)
	{
		result = arg1 & arg2;
	}
	if(operation == ADD)
	{
		result = arg1 + arg2;
	}
	if(operation == SUB)
	{
		result = arg1 - arg2;
	}
	return result;
}
	

					
/*****************************
 * Problems:
 * no problems
 */

/****************************
 * TODO s
 * - Functions always return what is in the AX register
 * - At the end of the function, RET [xxxx]
 * - A void function will still take an address for RET (but you don't have to use it)
 * - Add the ability to put constants into memory addresses: MOV [xxxx] CONST
 * - Condense some of the Assembler code into functions
 * - Convert the bit extraction into a standalone function
 * - Add labels handling
 * - Add comments handling
 * + Convert all the register switch statements into a function or class method
 */
