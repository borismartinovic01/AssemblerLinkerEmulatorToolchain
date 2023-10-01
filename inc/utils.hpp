#ifndef _UTILS_HPP
#define _UTILS_HPP

#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>
using namespace std;

struct Section{
	string name;
	int length = 0;
	vector<uint8_t> data;
	Section(string name):name(name){}
};

struct SectionHeaderElem{
	int num;
	int address;
	int size;
	int currSize;
	string name;
};
	
struct Symbol{
	int num;
	int value;
	int size;
	uint8_t type; // 0 - NOTYP; 1 - SCTN
	uint8_t bind; // 0 - LOC; 1 - GLOB
	int ndx; // -1 for undefined
	string name;
	Symbol(int num, int value, int size, uint8_t type, uint8_t bind, int ndx, string name)
		:num(num), value(value), size(size), type(type), bind(bind), ndx(ndx), name(name){}	
};
	
struct RelocationRecord{
	int offset;
	uint8_t type; //0 - R_X86_64_32, 1 - R_X86_64_32S, 2 - R_X86_64_PC32
	int symbol;
	int addend;
};

inline string symbolTypeToString(uint8_t type){
	if(type == 0) return "NOTYP";
	return "SCTN";
}

inline string bindTypeToString(uint8_t type){
	if(type == 0) return "LOC";
	return "GLOB";
}

inline string relocationTypeToString(uint8_t type){
	if(type == 0) return "R_X86_64_32";
	if(type == 1) return "R_X86_64_32S";
	return "R_X86_64_PC32";
}

#endif
