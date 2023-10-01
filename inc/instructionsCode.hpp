#ifndef _INSTRUCTIONSCODE_HPP
#define _INSTRUCTIONSCODE_HPP

const int haltCode = 0;
const int intCode = 1 << 28;
const int callImmedCode = 2 << 28;
const int callMemCode = 2 << 28 | 1 << 24;
const int jmpImmedCode = 3 << 28;
const int beqImmedCode = 3 << 28 | 1 << 24;
const int bneImmedCode = 3 << 28 | 2 << 24;
const int bgtImmedCode = 3 << 28 | 3 << 24;
const int jmpMemCode = 3 << 28 | 8 << 24;
const int beqMemCode = 3 << 28 | 9 << 24;
const int bneMemCode = 3 << 28 | 10 << 24;
const int bgtMemCode = 3 << 28 | 11 << 24;
const int xchgCode = 4 << 28;
const int addCode = 5 << 28;
const int subCode = 5 << 28 | 1 << 24;
const int mulCode = 5 << 28 | 2 << 24;
const int divCode = 5 << 28 | 3 << 24;
const int notCode = 6 << 28;
const int andCode = 6 << 28 | 1 << 24;
const int orCode = 6 << 28 | 2 << 24;
const int xorCode = 6 << 28 | 3 << 24;
const int shlCode = 7 << 28;
const int shrCode = 7 << 28 | 1 << 24; 
const int pushCode = 8 << 28 | 1 << 24;
const int csrrdCode = 9 << 28;
const int popCode = 9 << 28 | 3 << 24;
const int csrwrCode = 9 << 28 | 4 << 24;
const int popStatusCode = 9 << 28 | 7 << 24;
const int ldMemCode = 9 << 28 | 2 << 24;
const int ldRegdirCode = 9 << 28 | 1 << 24;
const int stRegindCode = 8 << 28;
const int stMemCode = 8 << 28 | 2 << 24;

#endif
