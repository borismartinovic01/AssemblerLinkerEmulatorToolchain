#ifndef _TYPESDEFS_HPP
#define _TYPESDEFS_HPP

enum TSymbol{
	SSWRD,
    SLITERAL
};

enum TOperand {
    IMMED,
    REGDIR,
    REGIND,
    REGINDPOM,
    MEM
};

enum TDirective{
	GLOBAL,
	EXTERN,
	SECTION,
	WORD,
	SKIP,
	END
};

enum TInstruction {
    HALT,
    INT,
    IRET,
    CALL,
    RET,
    JMP,
    BEQ,
    BNE,
    BGT,
    PUSH,
    POP,
    XCHG,
    ADD,
    SUB,
    MUL,
    DIV,
    NOT,
    AND,
    OR,
    XOR,
    SHL,
    SHR,
    LD,
    ST,
    CSRRD,
    CSRWR
};

enum TLine{
	LIGNORE,
	LINSTRUCTION,
	LDIRECTIVE
};

#endif