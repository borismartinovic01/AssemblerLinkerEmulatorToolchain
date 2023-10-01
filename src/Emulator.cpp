#include "../inc/Emulator.hpp"

void Emulator::emulate(){

	while(1){

        int currInstruction = read4Bytes(programState.registers[15]);
        programState.registers[15] += 4;
        executeInstruction(currInstruction);

    }
}

void Emulator::executeInstruction(int currInstruction){

	int oc = getOc(currInstruction);
	int mode = getMod(currInstruction);
	
	cout << "OC: " << oc << "\tMODE: " << mode <<"\n";
	
	switch(oc){
		case 0:
			cout << "HALT\n";
			haltInstruction(currInstruction);
			break;
		case 1:
			cout << "INT\n";
			intInstruction(currInstruction);
			break;
		case 2:
			if(mode == 0){
				cout << "CallImmed\n";
				callImmedInstruction(currInstruction);
			}
			else{
				cout << "CallMem\n";
				callMemInstruction(currInstruction);
			}
			break;
		case 3:{
			switch (mode){
				case 0:
					cout << "JmpImmed\n";
					jmpImmedInstruction(currInstruction);
					break;
				case 1:
					cout << "beqImmed\n";
					beqImmedInstruction(currInstruction);
					break;
				case 2:
					cout << "bneImmed\n";
					bneImmedInstruction(currInstruction);
					break;
				case 3:
					cout << "bgtImmed\n";
					bgtImmedInstruction(currInstruction);
					break;
				case 8:
					cout << "jmpMem\n";
					jmpMemInstruction(currInstruction);
					break;
				case 9:
					cout << "beqMem\n";
					beqMemInstruction(currInstruction);
					break;
				case 10:
					cout << "bneMem\n";
					bneMemInstruction(currInstruction);
					break;
				case 11:
					cout << "bgtMem\n";
					bgtMemInstruction(currInstruction);
					break;
			}
			break;
		}
		case 4:
			cout << "xchg\n";
			xchgInstruction(currInstruction);
			break;
		case 5:{
			switch(mode){
				case 0:
					cout << "add\n";
					addInstruction(currInstruction);
					break;
				case 1:
					cout << "sub\n";
					subInstruction(currInstruction);
					break;
				case 2:
					cout << "mul\n";
					mulInstruction(currInstruction);
					break;
				case 3:
					cout << "div\n";
					divInstruction(currInstruction);
					break;
			}
			break;
		}
		case 6:{
			switch(mode){
				case 0:
				cout << "not\n";
				notInstruction(currInstruction);
				break;
			case 1:
				cout << "and\n";
				andInstruction(currInstruction);
				break;
			case 2:
				cout << "or\n";
				orInstruction(currInstruction);
				break;
			case 3:
				cout << "xor\n";
				xorInstruction(currInstruction);
				break;
			}
			break;
		}
		case 7:{
			switch(mode){
				case 0:
					cout << "shl\n";
					shlInstruction(currInstruction);
					break;
				case 1:
					cout << "shr\n";
					shrInstruction(currInstruction);
					break;
			}
			break;
		}
		case 8:{
			switch(mode){
				case 0:
					cout << "stRegind\n";
					stRegindInstruction(currInstruction);
					break;
				case 1:
					cout << "push\n";
					pushInstruction(currInstruction);
					break;
				case 2: 
					cout << "stMem\n";
					stMemInstruction(currInstruction);
					break;
			}
			break;
		}
		case 9:{
			switch(mode){
				case 0:
					cout << "csrrd\n";
					csrrdInstruction(currInstruction);
					break;
				case 3:
					cout << "pop\n";
					popInstruction(currInstruction);
					break;
				case 4:
					cout << "csrwr\n";
					csrwrInstruction(currInstruction);
					break;
				case 7:
					cout << "popStatus\n";
					popStatusInstruction(currInstruction);
					break;
				case 2:
					cout << "ldMem\n";
					ldMemInstruction(currInstruction);
					break;
				case 1:
					cout << "ldRegdir\n";
					ldRegdirInstruction(currInstruction);
					break;
			}
			break;
		}
		default:
			cout << "Error: Unknown instruction.\n";
			exit(-1);
	}
}

void Emulator::haltInstruction(int currInstruction){
	
	cout << "Emulated processor executed halt instruction\n";
	cout << "Emulated processor state:\n";
	
	for(int i = 0; i < programState.registers.size(); i++){
		printf("r%d=0x%08X", i, (unsigned int)programState.registers[i]);
		// cout << "r" << i << "=0x" << hex << programState.registers[i];
		if((i + 1) % 4 == 0) printf("\n");
		else printf("\t");
	}
	
	exit(0);
}
void Emulator::intInstruction(int currInstruction){
	programState.registers[14] -= 4;
	write4Bytes(programState.registers[14], programState.csr[0]);
	
	programState.registers[14] -= 4;
	write4Bytes(programState.registers[14], programState.registers[15]);
	
	programState.csr[2] = 4;
	programState.csr[0] &= (~0x1);
	
	programState.registers[15] = programState.csr[1]; 
}
void Emulator::callImmedInstruction(int currInstruction){
	programState.registers[14] -= 4;
	write4Bytes(programState.registers[14], programState.registers[15]);
	
	programState.registers[15] = programState.registers[getRega(currInstruction)] + programState.registers[getRegb(currInstruction)] + getDispl(currInstruction);
}
void Emulator::callMemInstruction(int currInstruction){
	programState.registers[14] -= 4;
	write4Bytes(programState.registers[14], programState.registers[15]);
	
	programState.registers[15] = read4Bytes(programState.registers[getRega(currInstruction)] + programState.registers[getRegb(currInstruction)] + getDispl(currInstruction));
}
void Emulator::jmpImmedInstruction(int currInstruction){
	programState.registers[15] = programState.registers[getRega(currInstruction)] + getDispl(currInstruction);
}
void Emulator::beqImmedInstruction(int currInstruction){
	if(programState.registers[getRegb(currInstruction)] == programState.registers[getRegc(currInstruction)]){
		programState.registers[15] = programState.registers[getRega(currInstruction)] + getDispl(currInstruction);
	}
}
void Emulator::bneImmedInstruction(int currInstruction){
	if(programState.registers[getRegb(currInstruction)] != programState.registers[getRegc(currInstruction)]){
		programState.registers[15] = programState.registers[getRega(currInstruction)] + getDispl(currInstruction);
	}
}
void Emulator::bgtImmedInstruction(int currInstruction){
	if(programState.registers[getRegb(currInstruction)] > programState.registers[getRegc(currInstruction)]){
		programState.registers[15] = programState.registers[getRega(currInstruction)] + getDispl(currInstruction);
	}
}
void Emulator::jmpMemInstruction(int currInstruction){
	programState.registers[15] = read4Bytes(programState.registers[getRega(currInstruction)] + getDispl(currInstruction)) + 4;
}
void Emulator::beqMemInstruction(int currInstruction){
	if(programState.registers[getRegb(currInstruction)] == programState.registers[getRegc(currInstruction)]){
		programState.registers[15] = read4Bytes(programState.registers[getRega(currInstruction)] + getDispl(currInstruction)) + 4;
	}
}
void Emulator::bneMemInstruction(int currInstruction){
	if(programState.registers[getRegb(currInstruction)] != programState.registers[getRegc(currInstruction)]){
		programState.registers[15] = read4Bytes(programState.registers[getRega(currInstruction)] + getDispl(currInstruction));
	}
}
void Emulator::bgtMemInstruction(int currInstruction){
	if(programState.registers[getRegb(currInstruction)] > programState.registers[getRegc(currInstruction)]){
		programState.registers[15] = read4Bytes(programState.registers[getRega(currInstruction)] + getDispl(currInstruction));
	}
}
void Emulator::xchgInstruction(int currInstruction){
	int temp = programState.registers[getRegb(currInstruction)];
	programState.registers[getRegb(currInstruction)] = programState.registers[getRegc(currInstruction)];
	programState.registers[getRegc(currInstruction)] = temp;
}
void Emulator::addInstruction(int currInstruction){
	programState.registers[getRega(currInstruction)] = programState.registers[getRegb(currInstruction)] + programState.registers[getRegc(currInstruction)];
}
void Emulator::subInstruction(int currInstruction){
	programState.registers[getRega(currInstruction)] = programState.registers[getRegb(currInstruction)] - programState.registers[getRegc(currInstruction)];
}
void Emulator::mulInstruction(int currInstruction){
	programState.registers[getRega(currInstruction)] = programState.registers[getRegb(currInstruction)] * programState.registers[getRegc(currInstruction)];
}
void Emulator::divInstruction(int currInstruction){
	programState.registers[getRega(currInstruction)] = programState.registers[getRegb(currInstruction)] / programState.registers[getRegc(currInstruction)];
}
void Emulator::notInstruction(int currInstruction){
	programState.registers[getRega(currInstruction)] = ~(programState.registers[getRegb(currInstruction)]);
}
void Emulator::andInstruction(int currInstruction){
	programState.registers[getRega(currInstruction)] = programState.registers[getRegb(currInstruction)] & programState.registers[getRegc(currInstruction)];
}
void Emulator::orInstruction(int currInstruction){
	programState.registers[getRega(currInstruction)] = programState.registers[getRegb(currInstruction)] | programState.registers[getRegc(currInstruction)];
}
void Emulator::xorInstruction(int currInstruction){
	programState.registers[getRega(currInstruction)] = programState.registers[getRegb(currInstruction)] ^ programState.registers[getRegc(currInstruction)];
}
void Emulator::shlInstruction(int currInstruction){
	programState.registers[getRega(currInstruction)] = programState.registers[getRegb(currInstruction)] << programState.registers[getRegc(currInstruction)];
}
void Emulator::shrInstruction(int currInstruction){
	programState.registers[getRega(currInstruction)] = programState.registers[getRegb(currInstruction)] >> programState.registers[getRegc(currInstruction)];
}
void Emulator::pushInstruction(int currInstruction){
	programState.registers[getRega(currInstruction)] = programState.registers[getRega(currInstruction)] - 4;
	write4Bytes(programState.registers[getRega(currInstruction)], programState.registers[getRegc(currInstruction)]);
}
void Emulator::csrrdInstruction(int currInstruction){
	programState.registers[getRega(currInstruction)] = programState.csr[getRegb(currInstruction)];
}
void Emulator::popInstruction(int currInstruction){
	programState.registers[getRega(currInstruction)] = read4Bytes(programState.registers[getRegb(currInstruction)]);
	programState.registers[getRegb(currInstruction)] += 4;
}
void Emulator::csrwrInstruction(int currInstruction){
	programState.csr[getRega(currInstruction)] = programState.registers[getRegb(currInstruction)];
}
void Emulator::popStatusInstruction(int currInstruction){
	programState.csr[getRega(currInstruction)] = read4Bytes(programState.registers[getRegb(currInstruction)]);
	programState.registers[getRegb(currInstruction)] += 4;
}
void Emulator::ldMemInstruction(int currInstruction){

	programState.registers[getRega(currInstruction)] = read4Bytes(programState.registers[getRegb(currInstruction)] + 
															  programState.registers[getRegc(currInstruction)] +
															  getDispl(currInstruction));
				
}
void Emulator::ldRegdirInstruction(int currInstruction){
	programState.registers[getRega(currInstruction)] = programState.registers[getRegb(currInstruction)] + getDispl(currInstruction);
}
void Emulator::stRegindInstruction(int currInstruction){
	write4Bytes(programState.registers[getRega(currInstruction)] + 
			programState.registers[getRegb(currInstruction)] +
			getDispl(currInstruction), programState.registers[getRegc(currInstruction)]);
}

void Emulator::stMemInstruction(int currInstruction){
	write4Bytes(read4Bytes(programState.registers[getRega(currInstruction)] + 
			programState.registers[getRegb(currInstruction)] +
			getDispl(currInstruction)), programState.registers[getRegc(currInstruction)]);
}

int Emulator::getOc(int instruction){
	return (instruction >> 28) & 0x0000000f;
}

int Emulator::getMod(int instruction){
	return (instruction >> 24) & 0x0000000f;
}

unsigned int Emulator::getRega(int instruction){
	return (instruction >> 20) & 0x0000000f;
}

unsigned int Emulator::getRegb(int instruction){
	return (instruction >> 16) & 0x0000000f;
}

unsigned int Emulator::getRegc(int instruction){
	return (instruction >> 12) & 0x0000000f;
}

unsigned int Emulator::getDispl(int instruction){
	return instruction & 0x00000fff;
}

int Emulator::read4Bytes(unsigned int address){
	int result = 0;
	result |= memory[address++];
	result |= memory[address++] << 8;
	result |= memory[address++] << 16;
	result |= memory[address++] << 24;
	
	return result;
}
void Emulator::write4Bytes(unsigned int address, int value){
	memory[address++] = (value) & 0x000000ff;
	memory[address++] = (value >> 8) & 0x000000ff;
	memory[address++] = (value >> 16) & 0x000000ff;
	memory[address++] = (value >> 24) & 0x000000ff;
}
