#include "../inc/Assembler.hpp"

void Assembler::assembly(){
	
	mpHex[0] = "0";
    mpHex[1] = "1";
    mpHex[2] = "2";
    mpHex[3] = "3";
    mpHex[4] = "4";
    mpHex[5] = "5";
    mpHex[6] = "6";
    mpHex[7] = "7";
    mpHex[8] = "8";
    mpHex[9] = "9";
    mpHex[10] = "a";
    mpHex[11] = "b";
    mpHex[12] = "c";
    mpHex[13] = "d";
    mpHex[14] = "e";
    mpHex[15] = "f";
	
	if(currentPass == 3) return; //assembling already done
	
	initSymbolTable();
	
	//first pass
	
	for (Line& line: lines) {
        
		if(currentPass > 1) break;
        
      if (line.label != nullptr) {
        	
        string label = line.label;
        addLabel(label);
        free(line.label);
        
      }
      assemblyLine(line);
  }
  
  outputSectionHeader();
  outputSymbolTable();
  
  currSectionIdx = -1;
  currentPass = 2; //second pass
    
  for (Line& line: lines) {
		
		if(currentPass > 2) break;
    assemblyLine(line);  
  }

  currentPass = 3;
}

void Assembler::assemblyLine(Line line){
	switch (line.type) {
        case LIGNORE:
            break;
        case LDIRECTIVE:
        	switch(line.data.dir.type){
        		case GLOBAL:
        			globalDirective(line);
        			break;
        		case EXTERN:
							
        			externDirective(line);
        			break;
        		case SECTION:
        			sectionDirective(line);
        			break;
        		case WORD:
							wordDirective(line);
							break;
        		case SKIP:
        			skipDirective(line);
        			break;
        		case END:
        			endDirective();
							break;
					}        
          break;
        case LINSTRUCTION:
          switch(line.data.ins.type){
            case HALT:
							{
								cout << "Instruction: HALT\n";
								outputInstructionCode(haltCode);
            		break;
							}
            case INT:
            	{				cout << "Instruction: INT\n";
								outputInstructionCode(intCode);
								int displ = 4;
								int code2 = popStatusCode;
								code2 |= ((uint8_t)0) << 20; //status register index = 0
            					code2 |= ((uint8_t)14) << 16;
            					code2 |= displ;
								outputInstructionCode(code2);
            		break;
							}
						case LD:
							{
								ldInstruction(line);
								break;
							}
						case ST:
							{
								stInstruction(line);
								break;
							}
            case IRET:
            	{				cout << "Instruction: IRET\n";
								int code = popCode;
            		code |= ((uint8_t)15) << 20;
            		code |= ((uint8_t)14) << 16;
            		int displ = 4;
            		code |= displ;
            		outputInstructionCode(code);

            		break;
							}
            case CALL:
            	{
								cout << "Instruction: CALL\n";
								int code = callMemCode;
            		code |= ((uint8_t)line.data.ins.regD) << 20;
								jmpInstruction(line, code);
								
            		break;
							}
            case RET:
            	{				cout << "Instruction: RET\n";
								int code = popCode;
            		code |= ((uint8_t)line.data.ins.regD) << 20;
            		code |= ((uint8_t)line.data.ins.regS) << 16;
            		int displ = 4;
            		code |= displ;
            		outputInstructionCode(code);
					cout << "FINISHED RET\n";
            		break;
							}
            case JMP:
            	{
								cout << "Instruction: JMP\n";
            		int code = jmpMemCode;
            		code |= ((uint8_t)line.data.ins.regD) << 20;
            		jmpInstruction(line, code);
            		
            		break;
							}
            case BEQ:
            	{
					cout << "Instruction: BEQ\n";
            		int code = beqMemCode;
            		code |= ((uint8_t)15) << 20;
            		code |= ((uint8_t)line.data.ins.regD) << 16;
            		code |= ((uint8_t)line.data.ins.regS) << 12;
            		jmpInstruction(line, code);

            		break;
							}
            case BNE:
            	{
					cout << "Instruction: BNE\n";
            		int code = bneMemCode;
            		code |= ((uint8_t)15) << 20;
            		code |= ((uint8_t)line.data.ins.regD) << 16;
            		code |= ((uint8_t)line.data.ins.regS) << 12;
            		jmpInstruction(line, code);
								
            		break;
							}
            case BGT:
            	{
								cout << "Instruction: BGT\n";
            		int code = bgtMemCode;
            		code |= ((uint8_t)15) << 20;
            		code |= ((uint8_t)line.data.ins.regD) << 16;
            		code |= ((uint8_t)line.data.ins.regS) << 12;
            		jmpInstruction(line, code);
								
            		break;
							}
            case PUSH:
            	{				cout << "Instruction: PUSH\n";
								int code = pushCode;
            		code |= ((uint8_t)line.data.ins.regS) << 20;
            		code |= ((uint8_t)line.data.ins.regD) << 12;
            		int displ = -4 & 0x00000fff;
            		code |= displ;
            		outputInstructionCode(code);
            		break;
							}
            case POP:
            	{				cout << "Instruction: POP\n";
								int code = popCode;
            		code |= ((uint8_t)line.data.ins.regD) << 20;
            		code |= ((uint8_t)line.data.ins.regS) << 16;
            		int displ = 4;
            		code |= displ;
            		outputInstructionCode(code);
            		break;
							}
            case XCHG:
            	{				cout << "Instruction: XCHG\n";
								int code = xchgCode;
            		code |= ((uint8_t)line.data.ins.regS) << 16;
            		code |= ((uint8_t)line.data.ins.regD) << 12;
            		outputInstructionCode(code);
            		break;
							}
            case ADD:
            	{				cout << "Instruction: ADD\n";
								int code = addCode;
					code |= ((uint8_t)line.data.ins.regS) << 20;
            		code |= ((uint8_t)line.data.ins.regS) << 16;
            		code |= ((uint8_t)line.data.ins.regD) << 12;
            		outputInstructionCode(code);
            		break;
							}
            case SUB:
            	{				cout << "Instruction: SUB\n";
								int code = subCode;
					code |= ((uint8_t)line.data.ins.regS) << 20;
            		code |= ((uint8_t)line.data.ins.regS) << 16;
            		code |= ((uint8_t)line.data.ins.regD) << 12;
            		outputInstructionCode(code);
            		break;
							}
            case MUL:
            	{				cout << "Instruction: MUL\n";
								int code = mulCode;
					code |= ((uint8_t)line.data.ins.regS) << 20;
            		code |= ((uint8_t)line.data.ins.regS) << 16;
            		code |= ((uint8_t)line.data.ins.regD) << 12;
            		outputInstructionCode(code);
            		break;
							}
            case DIV:
            	{				cout << "Instruction: DIV\n";
								int code = divCode;
					code |= ((uint8_t)line.data.ins.regS) << 20;
            		code |= ((uint8_t)line.data.ins.regS) << 16;
            		code |= ((uint8_t)line.data.ins.regD) << 12;
            		outputInstructionCode(code);
            		break;
							}
            case NOT:
            	{				cout << "Instruction: NOT\n";
								int code = notCode;
            		code |= ((uint8_t)line.data.ins.regD) << 20;
            		code |= ((uint8_t)line.data.ins.regD) << 16;
            		outputInstructionCode(code);
            		break;
							}
            case AND:		
            	{				cout << "Instruction: AND\n";
								int code = andCode;
            		code |= ((uint8_t)line.data.ins.regD) << 20;
            		code |= ((uint8_t)line.data.ins.regD) << 16;
            		code |= ((uint8_t)line.data.ins.regS) << 12;
            		outputInstructionCode(code);
            		break;
							}
            case OR:
            	{				cout << "Instruction: OR\n";
								int code = orCode;
            		code |= ((uint8_t)line.data.ins.regD) << 20;
            		code |= ((uint8_t)line.data.ins.regD) << 16;
            		code |= ((uint8_t)line.data.ins.regS) << 12;
            		outputInstructionCode(code);
            		break;
							}
            case XOR:
            	{				cout << "Instruction: XOR\n";
								int code = xorCode;
            		code |= ((uint8_t)line.data.ins.regD) << 20;
            		code |= ((uint8_t)line.data.ins.regD) << 16;
            		code |= ((uint8_t)line.data.ins.regS) << 12;
            		outputInstructionCode(code);
            		break;
							}
            case SHL:
            	{				cout << "Instruction: SHL\n";
								int code = shlCode;
            		code |= ((uint8_t)line.data.ins.regD) << 20;
            		code |= ((uint8_t)line.data.ins.regD) << 16;
            		code |= ((uint8_t)line.data.ins.regS) << 12;
            		outputInstructionCode(code);
            		break;
							}
            case SHR:
            	{				cout << "Instruction: SHR\n";
								int code = shrCode;
            		code |= ((uint8_t)line.data.ins.regD) << 20;
            		code |= ((uint8_t)line.data.ins.regD) << 16;
            		code |= ((uint8_t)line.data.ins.regS) << 12;
            		outputInstructionCode(code);
            		break;
							}
            case CSRRD:
            	{				cout << "Instruction: CSRRD\n";
								int code = csrrdCode;
            		code |= ((uint8_t)line.data.ins.regD) << 20;
            		code |= ((uint8_t)line.data.ins.regS) << 16;
            		outputInstructionCode(code);
            		break;
							}
            case CSRWR:
            	{				cout << "Instruction: CSRWR\n";
								int code = csrwrCode;
            		code |= ((uint8_t)line.data.ins.regD) << 20;
            		code |= ((uint8_t)line.data.ins.regS) << 16;
            		outputInstructionCode(code);
            		break;
							}
					}
          break;
    }
}

void Assembler::outputInstructionCode(int code){
	if(currentPass == 1){
		
		sectionTable[currSectionIdx].length += 4;
		return;
	}
	uint8_t byte1 =  code & 0x000000ff;
	uint8_t byte2 = (code & 0x0000ff00) >> 8;
	uint8_t byte3 = (code & 0x00ff0000) >> 16;
	uint8_t byte4 = (code & 0xff000000) >> 24;
	
  sectionTable[currSectionIdx].data.push_back(byte4);
  sectionTable[currSectionIdx].data.push_back(byte3);
  sectionTable[currSectionIdx].data.push_back(byte2);
  sectionTable[currSectionIdx].data.push_back(byte1);
}

void Assembler::outputIntValue(int val){
	uint8_t byte1 =  val & 0x000000ff;
	uint8_t byte2 = (val & 0x0000ff00) >> 8;
	uint8_t byte3 = (val & 0x00ff0000) >> 16;
	uint8_t byte4 = (val & 0xff000000) >> 24;
  sectionTable[currSectionIdx].data.push_back(byte4);
  sectionTable[currSectionIdx].data.push_back(byte3);
  sectionTable[currSectionIdx].data.push_back(byte2);
  sectionTable[currSectionIdx].data.push_back(byte1);
}

void Assembler::initSymbolTable(){
	string currSymbol = "undefined";
	Symbol symbol(0, 0, 0, 0, 0, 0, currSymbol);
	symbolTable.push_back(symbol);
	symbolTableSize += 5 * sizeof(int) + 2 * sizeof(uint8_t) + currSymbol.size();
}

void Assembler::addLabel(string label){
	//int num, int value, int size, uint8_t type, uint8_t bind, int ndx, string name
	
	for(Symbol& symbol: symbolTable){
		if(symbol.name == label){
			bool symbolExists = true;
			if(symbol.bind == 1 && symbol.ndx == 0){
				symbol.ndx = currSectionIdx + 1;
				symbol.value = sectionTable[currSectionIdx].length;
				return;
			}
			
			cout << "Error: Multiple symbol definition" << "\n";
			exit(-1);
		}
	}
			
	
	Symbol symbol(symbolTable.size(), sectionTable[currSectionIdx].length, 0, 0, 0, currSectionIdx + 1, label);
	symbolTable.push_back(symbol);
	symbolTableSize += 5 * sizeof(int) + 2 * sizeof(uint8_t) + label.size();
}

void Assembler::jmpInstruction(Line line, int code){
	
	if(currentPass == 1){
		sectionTable[currSectionIdx].length += 4 * 3;
		return;
	}

	if(line.data.ins.operand.swrd == NULL){
		int displ = 4 & 0x00000fff;
    code |= displ;
    outputInstructionCode(code);
		
		int jmpCode = jmpImmedCode;
		jmpCode |= ((uint8_t)line.data.ins.regD) << 20;
		int jmpDispl =  4 & 0x00000fff;
		jmpCode |= jmpDispl;
		outputInstructionCode(jmpCode);

		outputIntValue(line.data.ins.operand.literal);

    return;
	}
	
	string currSymbol = line.data.ins.operand.swrd;
	
	free(line.data.ins.operand.swrd);
	
	bool symbolExists = false;
	
	for(Symbol& symbol: symbolTable){
		if(symbol.name == currSymbol){
			symbolExists = true;
			
			int displ = 4 & 0x00000fff;
    	code |= displ;
    	outputInstructionCode(code);

			int jmpCode = jmpImmedCode;
			jmpCode |= ((uint8_t)15) << 20;
			int jmpDispl =  4 & 0x00000fff;
			jmpCode |= jmpDispl;
			outputInstructionCode(jmpCode);

			if(currSectionIdx == symbol.ndx && currSectionIdx != 0){ //if section is the same no need for relocation record
				outputIntValue(symbol.value - sectionTable[currSectionIdx].data.size());
			}
			else{
				
				outputIntValue(0);
				//add relocation record
					
				bool relocationTableExists = false;
					
				RelocationRecord newRecord;
					
				if(bindTypeToString(symbol.bind) == "GLOB"){
					newRecord.offset = sectionTable[currSectionIdx].data.size() - 4;
					newRecord.type = 2;
					newRecord.symbol = symbol.num;
					newRecord.addend = 0;
				}
				else{
					newRecord.offset = sectionTable[currSectionIdx].data.size() - 4;
					newRecord.type = 2;
					newRecord.symbol = symbol.ndx;
					newRecord.addend = symbol.value - 4;
				}
					
					
				for(int i = 0; i < relocationTables.size(); i++){
					if(relocationTables[i].first == ".rela" + sectionTable[currSectionIdx].name){
					relocationTableExists = true;
					relocationTables[i].second.push_back(newRecord);
						break;
					}
				}
					
				if(relocationTableExists == false){
					vector<RelocationRecord> vc;
					vc.push_back(newRecord);
					relocationTables.push_back(make_pair(".rela" + sectionTable[currSectionIdx].name, vc));
				}
				
			}
			
			break;
		}
	}
	if(symbolExists == false){
		cout << "Error: Symbol undefined\n";
		exit(-1);
	}

}

void Assembler::ldInstruction(Line line){
	switch(line.data.ins.operand.type){
		case IMMED:
			cout << "Instruction: LD IMMED\n";
			ldImmed(line);
			break;
		case MEM:
			cout << "Instruction: LD MEM\n";
			ldMem(line);
			break;
		case REGDIR:
			cout << "Instruction: LD REGDIR\n";
			ldRegdir(line);
			break;
		case REGIND:
			cout << "Instruction: LD REGIND\n";
			ldRegind(line);
			break;
		case REGINDPOM:
			cout << "Instruction: LD REGINDPOM\n";
			ldRegindpom(line);
			break;
	}
	
}

void Assembler::stInstruction(Line line){
	switch(line.data.ins.operand.type){
		case MEM:
			cout << "Instruction: ST MEM\n";
			stMem(line);
			break;
		case REGIND:
			cout << "Instruction: ST REGIND\n";
			stRegind(line);
			break;
		case REGINDPOM:
			cout << "Instruction: ST REGINDPOM\n";
			stRegindpom(line);
			break;
	}
}

void Assembler::ldImmed(Line line){
	if(currentPass == 1){
		sectionTable[currSectionIdx].length += 4 * 3;
		return;
	}
	if(line.data.ins.operand.swrd == nullptr){
		int code = ldMemCode;
		code |= ((uint8_t)line.data.ins.regD) << 20;
		code |= ((uint8_t)15) << 16;
		int displ = 4 & 0x00000fff;
    code |= displ;
    outputInstructionCode(code);
		
		int jmpCode = jmpImmedCode;
		jmpCode |= ((uint8_t)15) << 20;
		int jmpDispl =  4 & 0x00000fff;
		jmpCode |= jmpDispl;
		outputInstructionCode(jmpCode);

		outputIntValue(line.data.ins.operand.literal);

    return;
	}
	
	string currSymbol = line.data.ins.operand.swrd;
	
	free(line.data.ins.operand.swrd);
	
	bool symbolExists = false;
	int code = ldMemCode;

	for(Symbol& symbol: symbolTable){
		if(symbol.name == currSymbol){
			symbolExists = true;
			
			code |= ((uint8_t)line.data.ins.regD) << 20;
			code |= ((uint8_t)15) << 16;
			int displ = 4 & 0x00000fff;
    	code |= displ;
    	outputInstructionCode(code);

			int jmpCode = jmpImmedCode;
			jmpCode |= ((uint8_t)15) << 20;
			int jmpDispl =  4 & 0x00000fff;
			jmpCode |= jmpDispl;
			outputInstructionCode(jmpCode);

			cout << currSectionIdx << "\n";
			if(currSectionIdx == symbol.ndx && currSectionIdx != 0){ //if section is the same no need for relocation record
				outputIntValue(symbol.value - sectionTable[currSectionIdx].data.size());
			}
			else{
				
				outputIntValue(0);
				//add relocation record
					
				bool relocationTableExists = false;
					
				RelocationRecord newRecord;
					
				if(bindTypeToString(symbol.bind) == "GLOB"){
					newRecord.offset = sectionTable[currSectionIdx].data.size() - 4;
					newRecord.type = 1;
					newRecord.symbol = symbol.num;
					newRecord.addend = 0;
				}
				else{
					newRecord.offset = sectionTable[currSectionIdx].data.size();
					newRecord.type = 1;
					newRecord.symbol = symbol.ndx;
					newRecord.addend = symbol.value;
				}
					
					
				for(int i = 0; i < relocationTables.size(); i++){
					if(relocationTables[i].first == ".rela" + sectionTable[currSectionIdx].name){
					relocationTableExists = true;
					relocationTables[i].second.push_back(newRecord);
						break;
					}
				}
					
				if(relocationTableExists == false){
					vector<RelocationRecord> vc;
					vc.push_back(newRecord);
					relocationTables.push_back(make_pair(".rela" + sectionTable[currSectionIdx].name, vc));
				}
				
			}
			
			break;
		}
	}
	if(symbolExists == false){
		cout << "Error: Symbol undefined\n";
		exit(-1);
	}

}

void Assembler::ldMem(Line line){

	if(currentPass == 1){
		sectionTable[currSectionIdx].length += 4 * 4;
		return;
	}

	if(line.data.ins.operand.swrd == nullptr){
		int code = ldMemCode;
		code |= ((uint8_t)line.data.ins.regD) << 20;
		code |= ((uint8_t)15) << 16;
		int displ = 4 & 0x00000fff;
    code |= displ;
    outputInstructionCode(code);
		
		int jmpCode = jmpImmedCode;
		jmpCode |= ((uint8_t)15) << 20;
		int jmpDispl =  4 & 0x00000fff;
		jmpCode |= jmpDispl;
		outputInstructionCode(jmpCode);

		outputIntValue(line.data.ins.operand.literal);
		//end of first part

		//second part

		code = ldMemCode;
		code |= ((uint8_t)line.data.ins.regD) << 20;
		code |= ((uint8_t)line.data.ins.regD) << 16;
    outputInstructionCode(code);
	cout << " " << code << "\n";

    return;
	}

	string currSymbol = line.data.ins.operand.swrd;
	
	free(line.data.ins.operand.swrd);
	
	bool symbolExists = false;
	int code = ldMemCode;

	for(Symbol& symbol: symbolTable){
		if(symbol.name == currSymbol){
			symbolExists = true;
			
			code |= ((uint8_t)line.data.ins.regD) << 20;
			code |= ((uint8_t)15) << 16;
			int displ = 4 & 0x00000fff;
    	code |= displ;
    	outputInstructionCode(code);

			int jmpCode = jmpImmedCode;
			jmpCode |= ((uint8_t)15) << 20;
			int jmpDispl =  4 & 0x00000fff;
			jmpCode |= jmpDispl;
			outputInstructionCode(jmpCode);

			if(currSectionIdx == symbol.ndx && currSectionIdx != 0){ //if section is the same no need for relocation record
				outputIntValue(symbol.value - sectionTable[currSectionIdx].data.size());
			}
			else{
				
				outputIntValue(0);
				//add relocation record
					
				bool relocationTableExists = false;
					
				RelocationRecord newRecord;
					
				if(bindTypeToString(symbol.bind) == "GLOB"){
					newRecord.offset = sectionTable[currSectionIdx].data.size() - 4;
					newRecord.type = 1;
					newRecord.symbol = symbol.num;
					newRecord.addend = 0;
				}
				else{
					newRecord.offset = sectionTable[currSectionIdx].data.size();
					newRecord.type = 1;
					newRecord.symbol = symbol.ndx;
					newRecord.addend = symbol.value;
				}
					
					
				for(int i = 0; i < relocationTables.size(); i++){
					if(relocationTables[i].first == ".rela" + sectionTable[currSectionIdx].name){
					relocationTableExists = true;
					relocationTables[i].second.push_back(newRecord);
						break;
					}
				}
					
				if(relocationTableExists == false){
					vector<RelocationRecord> vc;
					vc.push_back(newRecord);
					relocationTables.push_back(make_pair(".rela" + sectionTable[currSectionIdx].name, vc));
				}
				
			}
			
			break;
		}
	}
	if(symbolExists == false){
		cout << "Error: Symbol undefined\n";
		exit(-1);
	}

	//second part

	code = ldMemCode;
	code |= ((uint8_t)line.data.ins.regD) << 20;
	code |= ((uint8_t)line.data.ins.regD) << 16;
  outputInstructionCode(code);
}

void Assembler::ldRegdir(Line line){
	int code = ldRegdirCode;
	code |= ((uint8_t)line.data.ins.regD) << 20;
	code |= ((uint8_t)line.data.ins.operand.reg) << 16;
  outputInstructionCode(code);
}

void Assembler::ldRegind(Line line){
	int code = ldMemCode;
	code |= ((uint8_t)line.data.ins.regD) << 20;
	code |= ((uint8_t)line.data.ins.operand.reg) << 16;
  outputInstructionCode(code);
}

void Assembler::ldRegindpom(Line line){

	if(currentPass == 1){
		sectionTable[currSectionIdx].length += 4;
		return;
	}

	if(line.data.ins.operand.swrd == nullptr){

		if(line.data.ins.operand.literal > 2047 || line.data.ins.operand.literal < -2048){
			cout << "Error: Literal can't be represented with 12 bits\n";
			exit(-1);
		}

		int code = ldMemCode;
		code |= ((uint8_t)line.data.ins.regD) << 20;
		code |= ((uint8_t)line.data.ins.operand.reg) << 16;
		code |= ((uint8_t)line.data.ins.operand.literal);
    outputInstructionCode(code);

    	return;
	}

	string currSymbol = line.data.ins.operand.swrd;
	
	free(line.data.ins.operand.swrd);
	
	bool symbolExists = false;

	for(Symbol& symbol: symbolTable){
		if(symbol.name == currSymbol){
			symbolExists = true;
			
			if(currSectionIdx != symbol.ndx){ //if section is the same no need for relocation record
				cout << "Error: Symbol final value can't be determined\n";
				exit(-1);
				
			}

			if(symbol.value - sectionTable[currSectionIdx].data.size() > 2047 || symbol.value - sectionTable[currSectionIdx].data.size() < -2048){
				cout << "Error: Symbol can't be represented with 12 bits\n";
				exit(-1);
			}

			int code = ldMemCode;
			code |= ((uint8_t)line.data.ins.regD) << 20;
			code |= ((uint8_t)line.data.ins.operand.reg) << 16;
			code |= ((uint8_t)(symbol.value - sectionTable[currSectionIdx].data.size()));
    	outputInstructionCode(code);
			
			break;
		}
	}
	if(symbolExists == false){
		cout << "Error: Symbol undefined\n";
		exit(-1);
	}
}

void Assembler::stMem(Line line){

	if(currentPass == 1){
		sectionTable[currSectionIdx].length += 3 * 4;
		return;
	}

	if(line.data.ins.operand.swrd == nullptr){

		int code = stMemCode;
		code |= ((uint8_t)15) << 20;
		code |= ((uint8_t)line.data.ins.regD) << 12;
		int displ = 4 & 0x00000fff;
    	code |= displ;
    outputInstructionCode(code);

int jmpCode = jmpImmedCode;
		jmpCode |= ((uint8_t)15) << 20;
		int jmpDispl =  4 & 0x00000fff;
		jmpCode |= jmpDispl;
		outputInstructionCode(jmpCode);

		outputIntValue(line.data.ins.operand.literal);

    return;
	}

	string currSymbol = line.data.ins.operand.swrd;
	
	free(line.data.ins.operand.swrd);
	
	bool symbolExists = false;

	for(Symbol& symbol: symbolTable){
		if(symbol.name == currSymbol){
			symbolExists = true;

				int code = stMemCode;
		code |= ((uint8_t)15) << 20;
		code |= ((uint8_t)line.data.ins.regD) << 12;
		int displ = 4 & 0x00000fff;
    	code |= displ;
    outputInstructionCode(code);

int jmpCode = jmpImmedCode;
		jmpCode |= ((uint8_t)15) << 20;
		int jmpDispl =  4 & 0x00000fff;
		jmpCode |= jmpDispl;
		outputInstructionCode(jmpCode);

			if(currSectionIdx == symbol.ndx && currSectionIdx != 0){ //if section is the same no need for relocation record
				outputIntValue(symbol.value - sectionTable[currSectionIdx].data.size());
			}
			else{
				
				outputIntValue(0);
				// add relocation record
					
				bool relocationTableExists = false;
					
				RelocationRecord newRecord;
					
				if(bindTypeToString(symbol.bind) == "GLOB"){
					newRecord.offset = sectionTable[currSectionIdx].data.size() - 4;
					newRecord.type = 1;
					newRecord.symbol = symbol.num;
					newRecord.addend = 0;
				}
				else{
					newRecord.offset = sectionTable[currSectionIdx].data.size();
					newRecord.type = 1;
					newRecord.symbol = symbol.ndx;
					newRecord.addend = symbol.value;
				}
					
					
				for(int i = 0; i < relocationTables.size(); i++){
					if(relocationTables[i].first == ".rela" + sectionTable[currSectionIdx].name){
					relocationTableExists = true;
					relocationTables[i].second.push_back(newRecord);
						break;
					}
				}
					
				if(relocationTableExists == false){
					vector<RelocationRecord> vc;
					vc.push_back(newRecord);
					relocationTables.push_back(make_pair(".rela" + sectionTable[currSectionIdx].name, vc));
				}
				
			}
			
			break;
		}
	}
	if(symbolExists == false){
		cout << "Error: Symbol undefined\n";
		exit(-1);
	}
}

void Assembler::stRegind(Line line){
	int code = stRegindCode;
	code |= ((uint8_t)line.data.ins.operand.reg) << 20;
	code |= ((uint8_t)line.data.ins.regD) << 12;
  outputInstructionCode(code);
}

void Assembler::stRegindpom(Line line){

	if(currentPass == 1){
		sectionTable[currSectionIdx].length += 4;
		return;
	}

	if(line.data.ins.operand.swrd == nullptr){

		if(line.data.ins.operand.literal > 2047 || line.data.ins.operand.literal < -2048){
			cout << "Error: Literal can't be represented with 12 bits\n";
			exit(-1);
		}

		int code = stRegindCode;
		code |= ((uint8_t)line.data.ins.operand.reg) << 20;
		// code |= ((uint8_t)tmpReg) << 16;
		code |= ((uint8_t)line.data.ins.regD) << 12;
		code |= ((uint8_t)(line.data.ins.operand.literal));
    outputInstructionCode(code);

    return;
	}

	string currSymbol = line.data.ins.operand.swrd;
	cout << "Free st regindpom\n";
	free(line.data.ins.operand.swrd);
	
	bool symbolExists = false;
	int code = ldMemCode;

	for(Symbol& symbol: symbolTable){
		if(symbol.name == currSymbol){
			symbolExists = true;
			
			if(currSectionIdx != symbol.ndx){ //if section is the same no need for relocation record
				cout << "Error: Symbol final value can't be determined\n";
				exit(-1);
				
			}

			if(symbol.value - sectionTable[currSectionIdx].data.size() > 2047 || symbol.value - sectionTable[currSectionIdx].data.size() < -2048){
				cout << "Error: Symbol can't be represented with 12 bits\n";
				exit(-1);
			}

			int code = stRegindCode;
			code |= ((uint8_t)line.data.ins.operand.reg) << 20;
			// code |= ((uint8_t)tmpReg) << 16;
			code |= ((uint8_t)line.data.ins.regD) << 12;
			code |= ((uint8_t)(symbol.value - sectionTable[currSectionIdx].data.size()));
    	outputInstructionCode(code);
			
			break;
		}
	}
	if(symbolExists == false){
		cout << "Error: Symbol undefined\n";
		exit(-1);
	}
}

void Assembler::getSymbols(SymbolsList& symbolsList, vector<string>& vc){
	for(int i = 0; i < symbolsList.length; i++){
		vc.push_back(symbolsList.symbolsList[i].data.swrd);
		if(currentPass == 2) free(symbolsList.symbolsList[i].data.swrd);
	}
	if(currentPass == 2) free(symbolsList.symbolsList);
	
}

void Assembler::getSymbolsAndLiterals(SymbolsList& symbolsList, vector<string>& vc, vector<bool>& isSymbol){
	for(int i = 0; i < symbolsList.length; i++){
		
		if(symbolsList.symbolsList[i].type == SSWRD){
			vc.push_back(symbolsList.symbolsList[i].data.swrd);
			isSymbol.push_back(true);
			if(currentPass == 2) free(symbolsList.symbolsList[i].data.swrd);
		}
		else{
			vc.push_back(to_string(symbolsList.symbolsList[i].data.literal));
			isSymbol.push_back(false);
		}
	}
	if(currentPass == 2) free(symbolsList.symbolsList);
}

//int num, int value, int size, uint8_t type, uint8_t bind, int ndx, string name
void Assembler::globalDirective(Line line){
	
	vector<string> symbolsList;
	getSymbols(line.data.dir.symbolsList, symbolsList);
	
	if(currentPass == 1){
		for(string currSymbol: symbolsList){
			
			bool symbolExists = false;
			
			for(Symbol& symbol: symbolTable){
				if(symbol.name == currSymbol){
					symbolExists = true;
					break;
				}
			}
			
			if(symbolExists == false){
				Symbol symbol(symbolTable.size(), 0, 0, 0, 1, 0, currSymbol);
				symbolTable.push_back(symbol);
				symbolTableSize += 5 * sizeof(int) + 2 * sizeof(uint8_t) + currSymbol.size();
			}
		}
		return;
	}
	
	//second pass
	
	for(string currSymbol: symbolsList){
			
		bool symbolExists = false;
			
		for(Symbol& symbol: symbolTable){
			if(symbol.name == currSymbol){
				if(symbol.ndx == -1){
					cout << "Error: Global symbol undefined\n";
					exit(-1);
				}
			}
		}
	}
}

void Assembler::externDirective(Line line){
	
	vector<string> symbolsList;
	getSymbols(line.data.dir.symbolsList, symbolsList);
	
	if(currentPass == 1){
		for(string currSymbol: symbolsList){
			
			Symbol symbol(symbolTable.size(), 0, 0, 0, 1, 0, currSymbol);
			symbolTable.push_back(symbol);
			symbolTableSize += 5 * sizeof(int) + 2 * sizeof(uint8_t) + currSymbol.size();
		}
		return;
	}
	
}

void Assembler::sectionDirective(Line line){
	
	string sectionName = line.data.dir.swrd;
	cout << "Free section directive\n";
	if(currentPass == 2) free(line.data.dir.swrd);
	
	currSectionIdx++;
	
	if(currentPass == 1){
		Section section(sectionName);
		sectionTable.push_back(section);

		

		int idx = 1;

		for(int i = 1; i < symbolTable.size(); i++){
			if(symbolTable[i].type != 1){
				idx = i;
				break;
			}
		}
		Symbol symbol(idx, 0, 0, 1, 0, sectionTable.size(), sectionName);
		
		symbolTable.insert(symbolTable.begin() + idx, symbol);

		for(int i = 0; i < symbolTable.size(); i++){ //update first column
			symbolTable[i].num = i;
		}

		symbolTableSize += 5 * sizeof(int) + 2 * sizeof(uint8_t) + sectionName.size();
		
		return;
	}

	
}

void Assembler::wordDirective(Line line){
	
	vector<string> symbolsList;
	vector<bool> isSymbol;
	getSymbolsAndLiterals(line.data.dir.symbolsList, symbolsList, isSymbol);
	
	if(currentPass == 1){
		sectionTable[currSectionIdx].length += symbolsList.size() * 4;
		return;
	}
	
	//second pass
	
	for(int i = 0; i < symbolsList.size(); i++){
		int currVal = 0;

		if(isSymbol[i] == false){
			currVal = stoi(symbolsList[i]);
		}
		else {

			bool symbolExists = false;

			for(Symbol& symbol: symbolTable){
				if(symbol.name == symbolsList[i]){
					symbolExists = true;

					//add relocation record
					
					bool relocationTableExists = false;
					
					RelocationRecord newRecord;
					
					if(bindTypeToString(symbol.bind) == "GLOB"){
						newRecord.offset = sectionTable[currSectionIdx].data.size();
						newRecord.type = 0;
						newRecord.symbol = symbol.num;
						newRecord.addend = 0;
					}
					else{
						newRecord.offset = sectionTable[currSectionIdx].data.size();
						newRecord.type = 0;
						newRecord.symbol = symbol.ndx;
						newRecord.addend = symbol.value;
					}
					
					for(int i = 0; i < relocationTables.size(); i++){
						if(relocationTables[i].first == ".rela" + sectionTable[currSectionIdx].name){
							relocationTableExists = true;
							relocationTables[i].second.push_back(newRecord);
							break;
						}
					}
					
					if(relocationTableExists == false){
						vector<RelocationRecord> vc;
						vc.push_back(newRecord);
						relocationTables.push_back(make_pair(".rela" + sectionTable[currSectionIdx].name, vc));
					}
					
					break;
				}
			}
			
			if(symbolExists == false){
				cout << "Error: Symbol undefined\n";
				exit(-1);
			}
		}

		outputIntValue(currVal);
	}
	
}

void Assembler::skipDirective(Line line){

	int bytes = line.data.dir.literal;
	
	if(currentPass == 1){
		sectionTable[currSectionIdx].length += bytes;
		return;
	}

	for(int i = 0; i < bytes; i++){
		sectionTable[currSectionIdx].data.push_back(0);
	}
	
}

void Assembler::endDirective(){
	currentPass++;

	if(currentPass == 3){
		assemblingMessage = "SUCCESS";
		outputRelocationTables();
		outputSectionData();
		file.close();
		textFile.close();
	}
}

void Assembler::outputSymbolTable(){
	
	file.write((char*)(&symbolTableSize), sizeof(symbolTableSize));
	textFile << symbolTableSize << "\n";

  for(Symbol& symbol: symbolTable){
  	file.write((char*)(&(symbol.num)), sizeof(symbol.num));
  	file.write((char*)(&(symbol.value)), sizeof(symbol.value));
  	file.write((char*)(&(symbol.size)), sizeof(symbol.size));

		textFile << symbol.num << "\t";
		textFile << symbol.value << "\t";
		textFile << symbol.size << "\t";
  		
  	string typeString = symbolTypeToString(symbol.type);
  	file.write((char*)(&(symbol.type)), sizeof(symbol.type));
  	
		// textFile << typeStringSize << "\t";
		textFile << typeString << "\t";

  	string bindString = bindTypeToString(symbol.bind);
  	file.write((char*)(&(symbol.bind)), sizeof(symbol.bind));
  	
		// textFile << bindStringSize << "\t";
		textFile << bindString << "\t";

  	file.write((char*)(&(symbol.ndx)), sizeof(symbol.ndx));

		textFile << symbol.ndx << "\t";
  		
		int symbolNameSize = symbol.name.size();
  	file.write((char*)(&(symbolNameSize)), sizeof(symbolNameSize));
  	file.write((char*)(&(symbol.name[0])), symbol.name.size());

		// textFile << symbolNameSize << "\t";
		textFile << symbol.name << "\n";
	}
	
}

void Assembler::outputRelocationTables(){

	int relocationTablesSize = relocationTables.size();
	file.write((char*)(&(relocationTablesSize)), sizeof(relocationTablesSize));
	
	textFile << relocationTablesSize << "\n";

	for(int i = 0; i < relocationTables.size(); i++){
		
		int currRelocationTableSize = relocationTables[i].second.size() * (4 + 1 + 4 + 4);
		
		file.write((char*)(&(currRelocationTableSize)), sizeof(currRelocationTableSize));
		
		textFile << currRelocationTableSize << "\n";

		int relocationTableNameSize = relocationTables[i].first.size();
  	file.write((char*)(&(relocationTableNameSize)), sizeof(relocationTableNameSize));
  	file.write((char*)(&(relocationTables[i].first[0])), relocationTables[i].first.size());
		
		// textFile << relocationTableNameSize << "\t";
		textFile << relocationTables[i].first << "\n";

		for(RelocationRecord& rr: relocationTables[i].second){
			
			file.write((char*)(&(rr.offset)), sizeof(rr.offset));
			file.write((char*)(&(rr.type)), sizeof(rr.type));
			file.write((char*)(&(rr.symbol)), sizeof(rr.symbol));
			file.write((char*)(&(rr.addend)), sizeof(rr.addend));

			textFile << rr.offset << "\t";
			textFile << relocationTypeToString(rr.type) << "\t";
			textFile << rr.symbol << "\t";
			textFile << rr.addend << "\n";
		}
	}
}

void Assembler::outputSectionHeader(){
	int pos = ofile.find(".");
	ofileTxt = ofile.substr(0, pos);
	file.open(ofile, std::ios::out | std::ios::binary);
	textFile.open(ofileTxt + ".txt", std::fstream::out);

	int sectionHeaderSize = 0;
	
	for(int i = 0; i < sectionTable.size(); i++){
		sectionHeaderSize += 3 * sizeof(int) + sizeof(int) + sectionTable[i].name.size();
	}
	
	file.write((char*)(&(sectionHeaderSize)), sizeof(sectionHeaderSize));
	textFile << sectionHeaderSize << "\n";
	for(int i = 0; i < sectionTable.size(); i++){
		
		int num = i + 1;
		file.write((char*)(&(num)), sizeof(num));
		textFile << num << "\t";
		
		num = 0; //address field
		file.write((char*)(&(num)), sizeof(num));
		textFile << num << "\t";
		
		int sectionSize = sectionTable[i].length;
		file.write((char*)(&(sectionSize)), sizeof(sectionSize));
		
		textFile << sectionSize << "\t";

		int sectionNameSize = sectionTable[i].name.size();

		file.write((char*)(&(sectionNameSize)), sizeof(sectionNameSize));
  		file.write((char*)(&(sectionTable[i].name[0])), sectionTable[i].name.size());
  		
  		textFile << sectionTable[i].name << "\n";

//		textFile << sectionTable[currSectionIdx - 1].name << "\n";
	}
}

void Assembler::outputSectionData(){
	
	for(int i = 0; i < sectionTable.size(); i++){
		
		int cnt = 0;
		for(uint8_t& curr: sectionTable[i].data){
			file.write((char*)(&curr), sizeof(curr));
			textFile << mpHex[(curr >> 4)];
			textFile << mpHex[(curr & 0x0f)] << " ";
			cnt++;
			if(cnt == 4){
				textFile << "\n";
				cnt = 0;
			}
		}
		
	}
}
