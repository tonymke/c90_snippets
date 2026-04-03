.PHONY: all src clean check valgrind

export CFLAGS ?= -Og -g -pedantic -std=c90 -Wall -Werror -Wextra -Wfatal-errors \
			-Wno-error=pedantic -Wno-error=unused-parameter
export LDFLAGS ?= -lm # log.h uses math.h
export LDLIBS ?=

all: src

clean:
	$(MAKE) -C src clean
	$(MAKE) -C tests clean

check:
	$(MAKE) -C tests check

valgrind:
	$(MAKE) -C tests valgrind

src:
	$(MAKE) -C src all

src/%:
	$(MAKE) -C src $(patsubst src/%,%,$@)

tests/%:
	$(MAKE) -C tests $(patsubst tests/%,%,$@)
