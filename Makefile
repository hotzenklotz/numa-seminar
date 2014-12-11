CC := gcc-4.8
CFLAGS := -O3 -std=c99

rwlock.so: rwlock.c
	$(CC) -Wall -fPIC -Iinclude -shared -o rwlock.so rwlock.c

run: rwlock.so
	LD_PRELOAD=`pwd`/rwlock.so kccachetest wicked -th 32 -capsiz 2000000 100000

clean:
	@rm -f test rwlock.so *.o

