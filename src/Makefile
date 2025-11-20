.PHONY: all clean

SRC := hash.c htable.c log.c prime_po2s.c str.c

OBJ := $(patsubst %.c,%.o,$(SRC))

all: $(OBJ)

clean:
	$(RM) *.o $(BIN)

hash.o: hash.h
htable.o: htable.h prime_po2s.h
log.o: log.h str.h
prime_po2s.o: prime_po2s.h
str.o: str.h
