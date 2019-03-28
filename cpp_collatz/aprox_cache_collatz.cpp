#include "stdint.h"
#include <iostream>
using namespace std;

void collatz(int64_t i) {
	if ((i & (i-1)) == 0) {
		cout << i << "\n";
	}

	while (true) {
		if (i == 1) {
			return;
		}

		if (i % 2 == 0) {
			i = i / 2;
		} else {
			i = 3*i + 1;
		}
	}
}


int main() {
	for (int i = 1; i < 0xFFFF; i++) {
		collatz(i);
	}
}
