.PHONY: all src clean

export CFLAGS ?= -Og -g -pedantic -std=c90 -Wall -Werror -Wextra -Wfatal-errors \
			-Wno-error=pedantic -Wno-error=unused-parameter
export LDFLAGS ?= -lm # log.h uses math.h
export LDLIBS ?=

all: src

clean:
	$(MAKE) -C src clean

src:
	$(MAKE) -C src all

src/%:
	$(MAKE) -C src $(patsubst src/%,%,$@)
