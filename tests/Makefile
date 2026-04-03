.PHONY: all check clean FORCE

CFLAGS += -I../src

TESTS := test_hash test_str test_log

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
test_log: test_log.o ../src/log.o
test_str: test_str.o ../src/str.o
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

test_hash.o: test.h ../src/hash.h test_hash.c
test_str.o: test.h ../src/str.h test_str.c
test_log.o: test.h test_log.c ../src/log.h

../src/%.o: FORCE
	$(MAKE) -C ../src $*.o

FORCE:
