all: sqlite3-logging-preload.so

sqlite3-logging-preload.so: sqlite3-logging-preload.c
	gcc -g -Wall -fPIC -shared -o sqlite3-logging-preload.so sqlite3-logging-preload.c

test-bin: test-bin.c
	gcc -g -Wall -o test-bin test-bin.c -lsqlite3

clean:
	rm -f sqlite3-preload.so test-bin

test: test-bin sqlite3-logging-preload.so
	@rm -f test.db
	@LD_PRELOAD=./sqlite3-logging-preload.so ./test-bin
