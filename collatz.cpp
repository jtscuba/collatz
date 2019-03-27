#include <iostream>
#include <memory>
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include <thread>
#include <mutex>
#include <vector>

using namespace std;

using sizeType = uint64_t;

const uint64_t max_32 = ((1L << 32) -1);

int max_steps = 0;
int64_t max_int = 0;
const int64_t TABLE_SIZE = max_32;
int16_t table[TABLE_SIZE];

mutex mlock;

vector<vector<uint64_t>> value_buffer(4);

inline int collatz(int64_t n, int thread_id)
{
	int steps = 0;
	value_buffer[thread_id].clear();

	while (true)
	{
		if (n < TABLE_SIZE and table[n] != -1)
		{
			steps = steps + table[n];
			break;
		}
		value_buffer[thread_id].push_back(n);
        if (n == 1) break;
		if ((n & 1) == 0)
		{
			n = n >> 1;
			steps++;
		}
		else
		{
			n = (3 * n + 1);
			steps+=1;
		}

	}

	for (auto x : value_buffer[thread_id])
	{
		// fill in the table
		if (x < TABLE_SIZE) {
			// mlock.lock();
			// cout << "writing " << steps << " to location " << x << endl;
			// mlock.unlock();
			table[x] = steps; // only write to table, could write each value once in each thread okay though, same value written
		}
	    steps--;
	}

	if(value_buffer.empty())
	{
		return table[n];
	}
	else
	{
		return table[value_buffer[thread_id][0]];
	}
}

inline void helper(int64_t start, int64_t end, int thread_id)
{
	int local_max_steps = 0;
	uint64_t local_max_int = 0;
	for (int64_t i = start; i <= end; i++)
	{
		if ((i & (i-1)) == 0)
		{
			// mlock.lock();
			// cout << "max chain length of " << local_max_steps << " caused by " << local_max_int << endl;
			// mlock.unlock();
		}
		int res = collatz(i, thread_id);

		if (res > local_max_steps)
		{
			local_max_steps = res;
			local_max_int = i;
		}
	}

	mlock.lock();
	if (local_max_steps > max_steps)
	{
		max_steps = local_max_steps;
		max_int   = local_max_int;
	}
	mlock.unlock();
}

int main()
{

	// experimental upper bound
	for (auto& v : value_buffer)
	{
		v.reserve(5000);
	}
	// initalize table efficiently
	memset(table, -1, TABLE_SIZE*2);

	thread t1(helper, 1, max_32, 0);
	thread t2(helper, max_32/4 +1, max_32/2, 1);
	thread t3(helper, max_32/2 + 1, 3 * (max_32 / 4), 2);
	thread t4(helper, (3 * (max_32 / 4)) + 1, max_32, 3);

	t1.join(); t2.join(); t3.join(); t4.join();

	cout << "max chain length of " << max_steps << " caused by " << max_int << endl;
	return 0;
}
