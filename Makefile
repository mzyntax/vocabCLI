CFLAGS =  -Wall -Wextra -Werror -pedantic -g


#Just use this to compile the program

cli : cli.c servicer.c storagelib.c
	mkdir -p build
	gcc $(CFLAGS) cli.c cards.c servicer.c storagelib.c logger.c flashcard_queue/queue.c flashcard_system/spaced_repetition.c -o build/cli -lm











#| ############################# Ignore Below ############################# |#






























test: tests/test.c
	mkdir -p build
	gcc $(CFLAGS) tests/test.c -o build/test -lm

ptest: tests/pointer_logic.c
	mkdir -p build
	gcc $(CFLAGS) tests/pointer_logic.c -o build/ptest -lm

qtest: tests/queue_test.c
	mkdir -p build
	gcc $(CFLAGS) tests/queue_test.c flashcard_queue/queue.c cards.c -o build/qtest -lm

mtest: tests/memory_allocation.c
	mkdir -p build
	gcc $(CFLAGS) tests/memory_allocation.c -o build/mtest -lm

ftest: tests/formula_test.c
	mkdir -p build
	gcc $(CFLAGS) tests/formula_test.c -o build/ftest -lm

tester: tests/tester.c
	mkdir -p build
	gcc $(CFLAGS) tests/tester.c -o build/tester -lm