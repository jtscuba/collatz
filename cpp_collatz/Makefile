CC=clang++

SOURCE= collatz.cpp

CFLAGS= -O3 --debug -std=c++14 -Wall -Werror -Wextra -pedantic -pthread -march=native -ffast-math
LDFLAGS=

all: collatz_optimized

collatz_optimized: $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o collatz $(LDFLAGS)

collatz_naive: naive_collatz.cpp
	$(CC) $(CFLAGS) naive_collatz.cpp -o collatz_naive $(LDFLAGS)

collatz_aprox_cache: aprox_cache_collatz.cpp
	$(CC) $(CFLAGS) approx_cache_collatz.cpp -o collatz_naive $(LDFLAGS)

clean:
	rm collatz
