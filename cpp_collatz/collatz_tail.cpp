#include <iostream>
#include <memory>
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include <thread>
#include <mutex>

using namespace std;
const size_t TABLE_SIZE = 1024*1024*1024*4L;
const uint64_t LARGEST = 1024*1024*1024*4L;
int16_t table[TABLE_SIZE];
mutex m1;
int MAX_STEPS;
int MAX_INT;

inline int tailCollatz(const __int128 n, const int k) {
	if(n==1) return k;
	if(n < TABLE_SIZE && table[n] != - 1) return k+table[n];
	if(!(n & 1)) return tailCollatz(n >> 1, k+1);
	return tailCollatz((3*n+1) >> 1, k+2);
}

inline int Collatz(const uint64_t n) {
	//cout << "hi" << endl;
	int tmp = tailCollatz(n, 0);
	if(n < TABLE_SIZE) table[n] = tmp;
	return tmp;
}

inline void helper(int init, int last) {
	int maxSteps = 0;
	int maxInt = 0;
	for(int i = init; i <= last; i++) {
		int n = Collatz(i);
		if(n > maxSteps) {
			maxSteps = n;
			maxInt = i;
		}
	}
	m1.lock();
	if(maxSteps > MAX_STEPS) {
		MAX_STEPS = maxSteps;
		MAX_INT = maxInt;
	}
	m1.unlock();

}

int main() {
	memset(table, -1, TABLE_SIZE*2);
	thread t1(helper, 1, LARGEST/2);
	thread t2(helper, LARGEST/2 + 1, LARGEST);
	//thread t3(helper, LARGEST/2 + 1, 3*(LARGEST/4));
	//thread t4(helper, 3*(LARGEST/4) + 1, LARGEST);
	t1.join();
	t2.join();
	//t3.join();
	//t4.join();

	cout << "max steps is "<< MAX_STEPS << "at int " << MAX_INT << endl;
}
