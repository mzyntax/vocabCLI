CFLAGS =  -Wall -Wextra -Werror -pedantic -g

tester: tester.c servicer.c storagelib.c
	mkdir -p build
	gcc $(CFLAGS) tester.c servicer.c storagelib.c logger.c -o build/tester -lm

test: test.c
	mkdir -p build
	gcc $(CFLAGS) test.c -o build/test -lm

cli : cli.c servicer.c storagelib.c
	mkdir -p build
	gcc $(CFLAGS) cli.c servicer.c storagelib.c logger.c -o build/cli -lm
