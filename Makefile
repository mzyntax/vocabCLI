CFLAGS = -Wall

tester: tester.c servicer.c storagelib.c
	mkdir -p build
	gcc $(CFLAGS) tester.c servicer.c storagelib.c -o build/tester -lm

cli : cli.c servicer.c storagelib.c
	mkdir -p build
	gcc $(CFLAGS) cli.c servicer.c storagelib.c -o build/cli -lm
