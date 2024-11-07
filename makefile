
CC = g++ -g -O0

CCFLAGS = --ansi ${INC} -std=c++20 -W ${DEP} ${CC_OPTS}

FLEX = flex
FLEX_OPTS = -Pinstant_

BISON = bison
BISON_OPTS = -t -pinstant_
INC = -I. -I./lib/fmt/include/
DEP = -L./lib/fmt -lfmt 

OBJS = Absyn.o Buffer.o Lexer.o Parser.o Printer.o LLVMGenerator.o JasminGenerator.o InscJVM.o Error.o
OBJS_FILES = build/Absyn.o  build/Buffer.o build/Lexer.o build/Parser.o build/Printer.o build/LLVMGenerator.o build/JasminGenerator.o ./build/Error.o
.PHONY : clean distclean

all: CreateDirectory InscJVM InscLLVM
	
CreateDirectory:
	@mkdir -p build

clean :
	rm -f build/*.o src/TestInstant src/Instant.aux src/Instant.log src/Instant.pdf src/Instant.dvi src/Instant.ps src/Instant GeneratedFiles/* \
	rm -rf build insc_jvm insc_llvm

cleanTests :
	rm -f Tests/NewTestInputs/*.j Tests/NewTestInputs/*.ll Tests/NewTestInputs/*.bc \
	rm ./*.class \
	rm -rf ./Tests/NewTestOutputs \
	rm ./Tests/NewTestInputs/*.class

distclean : clean
	rm -f Absyn.h Absyn.c Bison.h Buffer.h Buffer.c Instant.l Lexer.c Instant.y Parser.h Parser.c Printer.c Printer.h Skeleton.c Skeleton.h Makefile Instant.tex

InscJVM: ${OBJS} InscJVM.o
	@echo "Linking TestInstant..."
	${CC} ${CCFLAGS} ${OBJS_FILES} build/InscJVM.o -o insc_jvm

InscLLVM: ${OBJS} InscLLVM.o
	${CC} ${CCFLAGS} ${OBJS_FILES} build/InscLLVM.o -o insc_llvm


Absyn.o : src/Absyn.c src/Absyn.h
	${CC} ${CCFLAGS} -c src/Absyn.c -o build/Absyn.o

Buffer.o : src/Buffer.c src/Buffer.h
	${CC} ${CCFLAGS} -c src/Buffer.c -o build/Buffer.o

Lexer.c : src/Instant.l
	${FLEX} ${FLEX_OPTS} -osrc/Lexer.c src/Instant.l

Parser.c Bison.h : src/Instant.y
	${BISON} ${BISON_OPTS} src/Instant.y -o src/Parser.c 

Lexer.o : CCFLAGS+=-Wno-sign-conversion

Lexer.o : src/Lexer.c src/Bison.h
	${CC} ${CCFLAGS} -c src/Lexer.c -o build/Lexer.o

Parser.o : src/Parser.c src/Absyn.h src/Bison.h
	${CC} ${CCFLAGS} -c src/Parser.c -o build/Parser.o

Printer.o : src/Printer.c src/Printer.h src/Absyn.h
	${CC} ${CCFLAGS} -c src/Printer.c -o build/Printer.o

LLVMGenerator.o : src/LLVMGenerator.cpp src/LLVMGenerator.h
	${CC} ${CCFLAGS} -c src/LLVMGenerator.cpp -o build/LLVMGenerator.o

JasminGenerator.o : src/JasminGenerator.cpp src/JasminGenerator.h
	${CC} ${CCFLAGS} -c src/JasminGenerator.cpp -o build/JasminGenerator.o

InscLLVM.o : src/InscLLVM.cpp src/Parser.h src/Printer.h src/Absyn.h
	${CC} ${CCFLAGS} -c src/InscLLVM.cpp -o build/InscLLVM.o

InscJVM.o : src/InscJVM.cpp src/Parser.h src/Printer.h src/Absyn.h
	${CC} ${CCFLAGS} -c src/InscJVM.cpp -o build/InscJVM.o

Error.o : src/Error.cpp 
	${CC} ${CCFLAGS} -c src/Error.cpp -o build/Error.o