CC=g++
CFLAGS=-c -std=gnu++0x -Wall -g
LDFLAGS=-lreadline

SOURCES=main.cpp \
	processor.cpp \
	device.cpp \
	processor_exception.cpp \
	asm/lexer.cpp \
	asm/parser.cpp \
	asm/parse_error.cpp \
	asm/symbol_table.cpp \
	asm/symbol_error.cpp \
	asm/symbol_analyser.cpp \
	asm/error_location.cpp \
	asm/code_generator.cpp \
	asm/type_error.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=dcpu16

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o asm/*.o dcpu16
