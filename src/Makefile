.PHONY: all clean

SRC := hash.c log.c prime_po2s.c str.c

OBJ := $(patsubst %.c,%.o,$(SRC))

all: $(OBJ)

clean:
	$(RM) *.o $(BIN)

hash.o: hash.c hash.h
log.o: log.c log.h str.h
prime_po2s.o: prime_po2s.c prime_po2s.h
str.o: str.c str.h
