bison: misc/machine.ypp
	bison -d misc/machine.ypp

flex: misc/machine.l machine.tab.hpp
	flex misc/machine.l

assembler: lex.yy.c machine.tab.cpp machine.tab.hpp src/Assembler.cpp src/sourceas.cpp
	g++ -Iinc -I. -std=c++17 machine.tab.cpp lex.yy.c src/Assembler.cpp src/sourceas.cpp -o assembler

linker: src/Linker.cpp src/sourceld.cpp
	g++ -Iinc -I. -std=c++17 src/Linker.cpp src/sourceld.cpp -o linker

emulator: src/Emulator.cpp src/sourceem.cpp
	g++ -Iinc -I. -std=c++17 src/Emulator.cpp src/sourceem.cpp -o emulator

build: bison flex assembler linker emulator

clean:
	rm -f assembler
	rm -f linker
	rm -f emulator
	rm -f lex.yy.c
	rm -f machine.tab.cpp
	rm -f machine.tab.hpp
	