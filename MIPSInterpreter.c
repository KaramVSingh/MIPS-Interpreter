#include <stdio.h>
#include <stdlib.h>

// testing a commit
//define an instruction structure:
typedef struct instructions {
	char* operation;
	char type;
	char* rs;
	char* rt;
	char* rd;
	char* label;

	int constant;
} instruct_t;

//this is the register file which will hold all of the data
int registerFile[32];

//this will contain the HI and LO registers:
int specRegisters[2];

//gets the size of the entire file
int getSize(FILE *fp) {
	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	rewind(fp);
	return size;
}

//calculates the exponent
int power(int base, int power) {
	int i = 0;
	int hold = base;

	for(i = 0; i < power - 1; i++) {
		hold = hold*base;
	}
	return hold;
}

//This function takes in a character array and a literal and compares them, returning
//1 if the values are the same and 0 if the values are different
int equals(char* wordA, char* literal) {
	int i = 0;
	int charEqual = 1;
	int ASize = arraySize(wordA);
	int BSize = arraySize(literal);

	if(ASize == BSize) {
		for(i = 0; i < ASize; i++) {
			if(wordA[i] != literal[i]) {
				charEqual = 0;
			}
		}
		return charEqual;
	} else {
		return 0;
	}
}


//gets the size of the entire array. This can be used to find the length of the string
int arraySize(char* word) {
	int i = 0;
	while(1) {
		if(word[i] == '\0') {
			return i;
		}
		i++;
	}
}

//this function will convert the word to lower case to avoid
//case sensitivity in programming
char* convertCase(char *word) {
	int i = 0;

	while(word[i] != '\0') {
		if(word[i] > 'a' - 1 && word[i] < 'z' + 1) {
			word[i] = word[i] - 32;
		}
		i++;
	}
	return word;
}

//this function will be used to get each word until the next space:
char* getNextWord(FILE *fp) {
	char *currentWord = malloc(sizeof(char) * 1);
	char currentLetter;
	int notTerminated = 1;
	int sizeOfWord = 0;

	//junk letter to help with extra spaces and enters
	currentWord[0] = '@';
	while(notTerminated) {
		if((currentLetter = getc(fp)) != ' ' && currentLetter != '\n' && currentLetter != '\r') {
			sizeOfWord++;
			currentWord = (char*)realloc(currentWord, sizeof(char) * sizeOfWord);
			currentWord[sizeOfWord - 1] = currentLetter;
			if(equals(convertCase(currentWord), "END")) {
				break;
			}
		} else {
			notTerminated = 0;
		}
	}

	return currentWord;
}

//This function will be used to get the first half of a compound word with parentheses
int getFirstSubstring(char* string) {
	int notTerminated = 1;
	int i = 0;

	char* output = (char*)malloc(1*sizeof(char));

	while(notTerminated) {
		if(string[i] != '(') {
			output = (char*)realloc(output, sizeof(char) * (i + 1));
			output[i] = string[i];
			i++;
		} else {
			notTerminated = 0;
		}
	}
	return atoi(output);
}

//This function will be used to get the second half of a compound word with parentheses
char* getNextSubstring(char* string) {
	int notTerminated = 1;
	int collecting = 0;
	int i = 0;
	int sizeOfWord = 1;

	char hold = NULL;
	char* output = (char*)malloc(1 * sizeof(char));

	while(notTerminated) {

		if(string[i] == ')') {
			notTerminated = 0;
		}
		if(collecting && notTerminated) {
			output = (char*)realloc(output, sizeof(char) * sizeOfWord);
			hold = string[i];
			output[sizeOfWord - 1] = hold;
			sizeOfWord++;
		}
		if(string[i] == '(') {
			collecting = 1;
		}
		
		i++;
	}

	return output;
}

//This function will remove the last character of an array
char* removeChar(char* string) {
	int i = 0;
	char *output = (char*)malloc(sizeof(char) * (arraySize(string) - 1));

	if(arraySize(string) > 1) {
		for(i = 0; i < arraySize(string) - 1; i++) {
			output[i] = string[i];
		}
		return output;
	} else {
		return string;
	}

}

//this function will be used for the JR command and will jump to a certain word number
void JUMPwordNumber(int target, FILE *fp) {
	rewind(fp);
	int i = 0;

	for(i = 0; i < target; i++) {
		getc(fp);
	}

	return;
}

//this function will be used to jump to the located label in the program when it is branched or jumps
void JUMP(char *label, FILE *fp) {
	rewind(fp);

	int notTerminated = 1;
	char *currentWord;

	while(notTerminated) {
		currentWord = getNextWord(fp);
		if(equals(removeChar(currentWord), label)) {
			notTerminated = 0;
		}
	}

	return;
}

//this function will innitialize and format the instruction as a struct:
instruct_t formatInstruction(char *operation, instruct_t instruction, FILE* fp) {
	instruction.operation = operation;
	char *hold = (char*)malloc(1 * sizeof(char));

	//All R TYPE instructions
	if(instruction.type == 'R') {
		if(equals(operation, "DIV") || equals(operation, "MULT")) {
			instruction.rd = "\0";
			instruction.rs = convertCase(getNextWord(fp));
			instruction.rt = convertCase(getNextWord(fp));
			instruction.constant = 0;
		} else {
			instruction.rd = convertCase(getNextWord(fp));
			instruction.rs = convertCase(getNextWord(fp));
			instruction.rt = convertCase(getNextWord(fp));
			instruction.constant = 0;
		}
	}

	//All I TYPE instuctions
	if(instruction.type == 'I') {
		if(equals(operation, "SLL")) {
			instruction.rd = convertCase(getNextWord(fp));
			instruction.rs = "\0";
			instruction.rt = convertCase(getNextWord(fp));
			instruction.constant = atoi(getNextWord(fp));
		} else if(equals(operation, "SRL")) {
			instruction.rd = convertCase(getNextWord(fp));
			instruction.rs = "\0";
			instruction.rt = convertCase(getNextWord(fp));
			instruction.constant = atoi(getNextWord(fp));
		} else if(equals(operation, "SRA")) {
			instruction.rd = convertCase(getNextWord(fp));
			instruction.rs = "\0";
			instruction.rt = convertCase(getNextWord(fp));
			instruction.constant = atoi(getNextWord(fp));
		} else if(equals(operation, "ADDI")) {
			instruction.rd = "\0";
			instruction.rt = convertCase(getNextWord(fp));
			instruction.rs = convertCase(getNextWord(fp));
			instruction.constant = atoi(getNextWord(fp));
		} else if(equals(operation, "SUBI")) {
			instruction.rd = "\0";
			instruction.rt = convertCase(getNextWord(fp));
			instruction.rs = convertCase(getNextWord(fp));
			instruction.constant = atoi(getNextWord(fp));
		} else if(equals(operation, "ANDI")) {
			instruction.rd = "\0";
			instruction.rt = convertCase(getNextWord(fp));
			instruction.rs = convertCase(getNextWord(fp));
			instruction.constant = atoi(getNextWord(fp));
		} else if(equals(operation, "ORI")) {
			instruction.rd = "\0";
			instruction.rt = convertCase(getNextWord(fp));
			instruction.rs = convertCase(getNextWord(fp));
			instruction.constant = atoi(getNextWord(fp));
		} else if(equals(operation, "SLTI")) {
			instruction.rd = "\0";
			instruction.rt = convertCase(getNextWord(fp));
			instruction.rs = convertCase(getNextWord(fp));
			instruction.constant = atoi(getNextWord(fp));
		} else if(equals(operation, "XORI")) {
			instruction.rd = "\0";
			instruction.rt = convertCase(getNextWord(fp));
			instruction.rs = convertCase(getNextWord(fp));
			instruction.constant = atoi(getNextWord(fp));
		} else if(equals(operation, "SW")) {
			instruction.rd = '\0';
			instruction.rt = convertCase(getNextWord(fp));

			hold = convertCase(getNextWord(fp));
			instruction.constant = getFirstSubstring(hold);
			instruction.rs = getNextSubstring(hold);
		} else if(equals(operation, "LW")) {
			instruction.rd = '\0';
			instruction.rt = convertCase(getNextWord(fp));

			hold = convertCase(getNextWord(fp));
			instruction.constant = getFirstSubstring(hold);
			instruction.rs = getNextSubstring(hold);
		} else if(equals(operation, "BNE")) {
			instruction.rd = '\0';
			instruction.rs = convertCase(getNextWord(fp));
			instruction.rt = convertCase(getNextWord(fp));
			instruction.constant = 0;
			instruction.label = convertCase(getNextWord(fp));
		} else if(equals(operation, "BEQ")) {
			instruction.rd = '\0';
			instruction.rs = convertCase(getNextWord(fp));
			instruction.rt = convertCase(getNextWord(fp));
			instruction.constant = 0;
			instruction.label = convertCase(getNextWord(fp));
		}
	}

	//All J TYPE instructions
	if(instruction.type == 'J') {
		instruction.label = convertCase(getNextWord(fp));
		instruction.rd = '\0';
		instruction.rs = '\0';
		instruction.rt = '\0';
		instruction.constant = 0;
	}

	return instruction;
}

//this function will convert the register names into integers
int convertToInt(char* registerName) {
	if(registerName[1] == 'Z' || registerName[1] == '0') {
		return 0;
	} else if(equals(registerName, "$AT")) {
		return 1;
	} else if(registerName[1] == 'V') {
		return (registerName[2] - '0' + 2);
	} else if(registerName[1] == 'A') {
		return (registerName[2] - '0' + 4);
	} else if(registerName[1] == 'T') {
		if(registerName[2] - '0' < 8) {
			return (registerName[2] - '0' + 8);
		} else {
			return (registerName[2] - '0' + 16);
		}
	} else if(registerName[1] == 'S') {
		if(registerName[2] == 'P') {
			return 29;
		} else {
			return (registerName[2] - '0' + 16);
		}
	} else if(registerName[1] == 'K') {
		return (registerName[2] - '0' + 26);
	} else if(registerName[1] == 'G') {
		return 28;
	} else if(registerName[1] == 'F') {
		return 30;
	} else if(registerName[1] == 'R') {
		return 31;
	}
	return 0;
}

//this function will execue all R type instructions in the program:
void executeRType(char* operation, instruct_t instruction, FILE* fp) {

	instruction.type = 'R';
	instruction = formatInstruction(operation, instruction, fp);

	//decodes and executes ADD instruction
	if(equals(operation, "ADD")) {

		registerFile[convertToInt(instruction.rd)] = registerFile[convertToInt(instruction.rs)] + registerFile[convertToInt(instruction.rt)];
	} else if(equals(operation, "SUB")) {

		registerFile[convertToInt(instruction.rd)] = registerFile[convertToInt(instruction.rs)] - registerFile[convertToInt(instruction.rt)];
	} else if(equals(operation, "AND")) {

		registerFile[convertToInt(instruction.rd)] = registerFile[convertToInt(instruction.rs)] & registerFile[convertToInt(instruction.rt)];
	} else if(equals(operation, "OR")) {

		registerFile[convertToInt(instruction.rd)] = registerFile[convertToInt(instruction.rs)] | registerFile[convertToInt(instruction.rt)];
	} else if(equals(operation, "SLT")) {

		if(registerFile[convertToInt(instruction.rs)] < registerFile[convertToInt(instruction.rt)]) {
			registerFile[convertToInt(instruction.rd)] = 1;
		} else {
			registerFile[convertToInt(instruction.rd)] = 0;
		}
	} else if(equals(operation, "XOR")) {

		registerFile[convertToInt(instruction.rd)] = registerFile[convertToInt(instruction.rs)] ^ registerFile[convertToInt(instruction.rt)];
	} else if(equals(operation, "DIV")) {

		specRegisters[0] = registerFile[convertToInt(instruction.rs)] / registerFile[convertToInt(instruction.rt)];
		specRegisters[1] = registerFile[convertToInt(instruction.rs)] % registerFile[convertToInt(instruction.rt)];
	} else if(equals(operation, "MULT")) {

		specRegisters[0] = (registerFile[convertToInt(instruction.rs)] * registerFile[convertToInt(instruction.rt)]) & (0xFFFFFFFF00000000);
		specRegisters[1] = (registerFile[convertToInt(instruction.rs)] * registerFile[convertToInt(instruction.rt)]) & (0x00000000FFFFFFFF);
	}
}

//this function will execue all I type instructions in the program:
void executeIType(char* operation, instruct_t instruction, FILE* fp, int* memory) {

	instruction.type = 'I';
	instruction = formatInstruction(operation, instruction, fp);

	//decodes and executes SLL instruction
	if(equals(operation, "SLL")) {

		registerFile[convertToInt(instruction.rd)] = registerFile[convertToInt(instruction.rt)] << instruction.constant;
	} else if(equals(operation, "SRL")) {

		registerFile[convertToInt(instruction.rd)] = registerFile[convertToInt(instruction.rt)] / power(2, instruction.constant);
	} else if(equals(operation, "SRA")) {

		registerFile[convertToInt(instruction.rd)] = registerFile[convertToInt(instruction.rt)] >> instruction.constant;
	} else if(equals(operation, "ADDI")) {

		registerFile[convertToInt(instruction.rt)] = registerFile[convertToInt(instruction.rs)] + instruction.constant;
	} else if(equals(operation, "SUBI")) {

		registerFile[convertToInt(instruction.rt)] = registerFile[convertToInt(instruction.rs)] - instruction.constant;
	} else if(equals(operation, "ANDI")) {

		registerFile[convertToInt(instruction.rt)] = registerFile[convertToInt(instruction.rs)] & instruction.constant;
	} else if(equals(operation, "ORI")) {

		registerFile[convertToInt(instruction.rt)] = registerFile[convertToInt(instruction.rs)] | instruction.constant;
	} else if(equals(operation, "SLTI")) {

		if(registerFile[convertToInt(instruction.rs)] < instruction.constant) {
			registerFile[convertToInt(instruction.rt)] = 1;
		} else {
			registerFile[convertToInt(instruction.rt)] = 0;
		}
	} else if(equals(operation, "XORI")) {

		registerFile[convertToInt(instruction.rt)] = registerFile[convertToInt(instruction.rs)] ^ instruction.constant;
	} else if(equals(operation, "SW")) {
		//this stores the value in rt into the memory adress at rs + instruction constant
		memory[-registerFile[convertToInt(instruction.rs)]/4 - instruction.constant/4 - 1] = registerFile[convertToInt(instruction.rt)];
	} else if(equals(operation, "LW")) {
		//this loads the value in rs + instruction constant into rt
		registerFile[convertToInt(instruction.rt)] = memory[-registerFile[convertToInt(instruction.rs)]/4 - instruction.constant/4 - 1];
	} else if(equals(operation, "BNE")) {
		
		if(registerFile[convertToInt(instruction.rs)] != registerFile[convertToInt(instruction.rt)]) {
			JUMP(instruction.label, fp);
		}
	} else if(equals(operation, "BEQ")) {
		
		if(registerFile[convertToInt(instruction.rs)] == registerFile[convertToInt(instruction.rt)]) {
			JUMP(instruction.label, fp);
		}
	}
}

//this function will execue all J type instructions in the program:
void executeJType(char* operation, instruct_t instruction, FILE* fp) {

	instruction.type = 'J';
	instruction = formatInstruction(operation, instruction, fp);

	//decodes and executes J instruction
	if(equals(operation, "J")) {

		JUMP(instruction.label, fp);
	} else if(equals(operation, "JAL")) {

		//first we need to store the word number in the $RA register, the nwe need to jump
		registerFile[convertToInt("$RA")] = ftell(fp);
		JUMP(instruction.label, fp);
	} else if(equals(operation, "JR")) {

		//we need to jump to what is stored in JAL $RA
		JUMPwordNumber(registerFile[convertToInt("$RA")], fp);
		registerFile[convertToInt("$RA")] = 0;
	} else if(equals(operation, "MFHI")) {

		registerFile[convertToInt(instruction.label)] = specRegisters[0];
	} else if(equals(operation, "MFLO")) {

		registerFile[convertToInt(instruction.label)] = specRegisters[1];
	}
}

//this is a custom instruction made to test code. It simply prints what is in the specified register
void PRINT(FILE *fp) {
	char *registerName = getNextWord(fp);
	printf("%d\n", registerFile[convertToInt(registerName)]);
}

//this is the main function for the program
int main(int argc, char *argv[]) {

	printf("\n\n");
	if(argc == 1) {
		printf("Enter: ./a.out <MIPS PROGRAM> to execute the program\n");
		return 0;
	}

	
	//this dynamic array will be used to store permanent memory which can be modified by the user
	int *memory = (int*)malloc(0 * sizeof(int));

	instruct_t instruction;
	int i = 0;

	//initialize array values to zero
	for(i = 0; i < 32; i++) {
		registerFile[i] = 0;
	}

	specRegisters[0] = 0;
	specRegisters[1] = 0;

	int notTerminated = 1;
	char *currentWord;
	FILE *fp = fopen(argv[1], "r");

	//We have opened the file and are now ready to begin decoding it:
	//first we want to find the length of the file to maintain stylistic integrity
	int size = getSize(fp);

	//Once we have the size of the file, we can begin decoding it and applying its effects on the register file
	while(notTerminated) {

		registerFile[0] = 0;

		//We want the memory size to always correspond to where the stack pointer is
		memory = (int*)realloc(memory, sizeof(int)*(-1*registerFile[convertToInt("$SP")]/4));

		if(equals((currentWord = convertCase(getNextWord(fp))), "END")) {
			notTerminated = 0;
		} else if(equals(currentWord, "PRINT")) {
			PRINT(fp);
		} else {

			//in this case, we have gotten the current word and must
			//decode the instruction

			//R TYPE INSTRUCTIONS
			if(equals(currentWord, "ADD")) {
				executeRType(currentWord, instruction, fp);
			} else if(equals(currentWord, "SUB")) {
				executeRType(currentWord, instruction, fp);
			} else if(equals(currentWord, "AND")) {
				executeRType(currentWord, instruction, fp);
			} else if(equals(currentWord, "OR")) {
				executeRType(currentWord, instruction, fp);
			} else if(equals(currentWord, "SLT")) {
				executeRType(currentWord, instruction, fp);
			} else if(equals(currentWord, "XOR")) {
				executeRType(currentWord, instruction, fp);
			} else if(equals(currentWord, "DIV")) {
				executeRType(currentWord, instruction, fp);
			} else if(equals(currentWord, "MULT")) {
				executeRType(currentWord, instruction, fp);
			}

			//I TYPE INSTRUCTIONS
			if(equals(currentWord, "SLL")) {
				executeIType(currentWord, instruction, fp, memory);
			} else if(equals(currentWord, "SRL")) {
				executeIType(currentWord, instruction, fp, memory);
			} else if(equals(currentWord, "SRA")) {
				executeIType(currentWord, instruction, fp, memory);
			} else if(equals(currentWord, "ADDI")) {
				executeIType(currentWord, instruction, fp, memory);
			} else if(equals(currentWord, "SUBI")) {
				executeIType(currentWord, instruction, fp, memory);
			} else if(equals(currentWord, "ANDI")) {
				executeIType(currentWord, instruction, fp, memory);
			} else if(equals(currentWord, "ORI")) {
				executeIType(currentWord, instruction, fp, memory);
			} else if(equals(currentWord, "SLTI")) {
				executeIType(currentWord, instruction, fp, memory);
			} else if(equals(currentWord, "XORI")) {
				executeIType(currentWord, instruction, fp, memory);
			} else if(equals(currentWord, "SW")) {
				executeIType(currentWord, instruction, fp, memory);
			} else if(equals(currentWord, "LW")) {
				executeIType(currentWord, instruction, fp, memory);
			} else if(equals(currentWord, "BNE")) {
				executeIType(currentWord, instruction, fp, memory);
			} else if(equals(currentWord, "BEQ")) {
				executeIType(currentWord, instruction, fp, memory);
			}

			//J TYPE INSTRUCTIONS
			if(equals(currentWord, "J")) {
				executeJType(currentWord, instruction, fp);
			} else if(equals(currentWord, "JAL")) {
				executeJType(currentWord, instruction, fp);
			} else if(equals(currentWord, "JR")) {
				executeJType(currentWord, instruction, fp);
			} else if(equals(currentWord, "MFHI")) {
				executeJType(currentWord, instruction, fp);
			} else if(equals(currentWord, "MFLO")) {
				executeJType(currentWord, instruction, fp);
			}
		}
	}

	printf("\n\n");

	
}