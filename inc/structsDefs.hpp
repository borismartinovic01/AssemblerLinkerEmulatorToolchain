#ifndef _STRUCTSDEFS_HPP
#define _STRUCTSDEFS_HPP

#include "typesDefs.hpp"
#include <cstdint>

struct SymbolListElement{
	TSymbol type;
    union {
        int literal;
        char* swrd;
    } data;
};

struct SymbolsList{
	int length;
	SymbolListElement* symbolsList;
};



struct Operand {
    TOperand type;
    int reg;
    int literal;
    char* swrd;
};



struct Instruction{
	TInstruction type;
    Operand operand;
    int regS;
    int regD;
};

struct Directive{
	TDirective type;
	SymbolsList symbolsList;
	char* swrd;
    int literal;
};

struct Line{
    TLine type;
    char* label;
    union {
        Directive dir;
        Instruction ins;
    }data;
};

#endif
