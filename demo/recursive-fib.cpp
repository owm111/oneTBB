/* TBB parallel recursive Fibonacci number calculator which measures throughput
 *
 * usage: ./recursive-fib [n]
 *
 * If the n argument is given, each test will be ran n times instead of once.
 *
 * Reads lines from standard input with the following format:
 *
 * 	<n> <nthread>
 *
 * The nth Fibonacci number will be computed using nthread threads.
 *
 * Results are written to standard output with the following format:
 *
 * 	<n> <fib_number> <nthread> <jobs> <total_time> <tasks/sec> <lb>
 *
 * Where fib_number is the nth Fibonacci number and jobs is the amount of
 * parallel threads created. lb is information related to load balancing, which
 * contains the minimum, the standard deviation from the average, and the
 * maximum tasks per TBB thread.
 */

#include <oneapi/tbb.h>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <memory>
/*#include <sched.h>*/
#include <vector>
#include <sstream>

#include "load_balance.hpp"
#include "observers.hpp"
#include "utils.hpp"

std::uint64_t threads_created(std::uint64_t n);
std::uint64_t parallel_fib(std::uint64_t n);
std::uint64_t parallel_fib_lb(std::uint64_t n, load_balance& lb);

const char *progname = "recursive-fib";
std::uint64_t
parallel_fib(std::uint64_t n)
{
	if (n < 2)
		return n;

	std::uint64_t x, y;
	oneapi::tbb::parallel_invoke(
			[&]{x = parallel_fib(n - 1);},
			[&]{y = parallel_fib(n - 2);});
	return x + y;
}

std::uint64_t
parallel_fib_lb(std::uint64_t n, load_balance& lb)
{
	if (n < 2)
		return n;

	lb.take_measurement();
	std::uint64_t x, y;
	oneapi::tbb::parallel_invoke(
			[&]{x = parallel_fib_lb(n - 1, lb);},
			[&]{y = parallel_fib_lb(n - 2, lb);});
	return x + y;
}

/* Recursively computes the number of threads created by parallel_fib(n) */
/* TODO: there should be an O(1) way to compute this */
std::uint64_t
threads_created(std::uint64_t n)
{
	if (n < 2)
		return 0;
	return 2 + threads_created(n - 1) + threads_created(n - 2);
}

int
main(int argc, char *argv[])
{
	progname = argv[0];
	int tests = 1;
	for (int i = 1; i < argc; ++i) {
		char *end;
		tests = std::strtoul(argv[i], &end, 10);
		if (*end != '\0') {
			die("argument must be a valid integer");
		} else if (tests < 1) {
			die("argument must be greater than 0");
		}

	}

	std::uint64_t fib_num;
	unsigned nthread;
	while (std::cin >> fib_num >> nthread) {
		oneapi::tbb::task_arena arena(nthread);
		pinning_observer observer(arena);
		for (int i = 0; i < tests; ++i) {
			auto start_time = std::chrono::high_resolution_clock::now();
			auto result = parallel_fib(fib_num);
			auto end_time = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> d = end_time - start_time;
			double total_time = d.count();
			double jobs = threads_created(fib_num);
			double throughput = jobs / total_time;
			int nslots =
				oneapi::tbb::this_task_arena::max_concurrency();
			load_balance lb(nthread, nslots);
			parallel_fib_lb(fib_num, lb);
			std::cout << fib_num << "\t" << result << "\t" <<
				nthread << "\t" << jobs << "\t" << total_time
				<< "\t" << throughput << "\t" << lb <<
				std::endl;

		}
	}

	if (!std::cin.eof() && std::cin.fail()) {
		die("error reading from stdin");
	}

	std::exit(EXIT_SUCCESS);
}
