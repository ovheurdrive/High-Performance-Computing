CC=mpicc
CFLAGS=-Wall -g
LDFLAGS=

EXEC= main
TEST= test 
LIBS= $(wildcard lib/*.c)
LIBS_OBJ= $(LIBS:.c=.o)
LIBS_HDR= $(LIBS:.c=.h)

all: $(EXEC) $(TEST)

main: src/main.o $(LIBS_OBJ)
	@[ -d bin  ] || mkdir bin
	$(CC) -o bin/$@ $^ $(LDFLAGS)

main.o: $(LIBS_HDR)

test: test/test.o $(LIBS_OBJ)
	@[ -d bin  ] || mkdir bin
	$(CC) -o bin/$@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean

clean:
	rm -rf lib/*.o
	rm -rf src/*.o
	rm -rf tests/*.o