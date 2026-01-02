.PHONY: all check clean FORCE

CFLAGS += -I../src

TESTS := test_hash

all: $(TESTS)

check: $(TESTS)
	@echo "Running tests..."
	@for test in $(TESTS); do \
		echo "Running $$test..."; \
		./$$test || exit 1; \
	done
	@echo "All tests passed!"

clean:
	$(RM) $(TESTS) *.o

test_hash: test_hash.o ../src/hash.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

test_hash.o: test.h ../src/hash.h

../src/%.o: FORCE
	$(MAKE) -C ../src $*.o

FORCE:
