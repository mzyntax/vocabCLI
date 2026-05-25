CFLAGS =  -Wall -Wextra -Werror -pedantic -g

cli : cli.c servicer.c storagelib.c
	mkdir -p build
	gcc $(CFLAGS) cli.c servicer.c storagelib.c logger.c -o build/cli -lm


test: tests/test.c
	mkdir -p build
	gcc $(CFLAGS) tests/test.c -o build/test -lm

ptest: tests/pointer_logic.c
	mkdir -p build
	gcc $(CFLAGS) tests/pointer_logic.c -o build/ptests -lm

ftest: tests/formula_test.c
	mkdir -p build
	gcc $(CFLAGS) tests/formula_test.c -o build/ftest -lm

tester: tests/tester.c servicer.c storagelib.c
	mkdir -p build
	gcc $(CFLAGS) tests/tester.c servicer.c storagelib.c logger.c -o build/tester -lm