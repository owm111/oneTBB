#include <oneapi/tbb/task_arena.h>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <numeric>

#include <iostream>

#include "load_balance.hpp"

struct statistics {
	std::uint64_t min, max;
	double avg, dev;
};

static statistics calc_statistics(const std::vector<std::uint64_t>& v);

static statistics
calc_statistics(const std::vector<std::uint64_t>& v)
{
	statistics res;
	double count = v.size();
	res.min = *std::min_element(v.begin(), v.end());
	res.max = *std::max_element(v.begin(), v.end());
	res.avg = (double)std::accumulate(v.begin(), v.end(), 0) / (double)count;
	auto dev_lambda = [res] (double acc, std::uint64_t x) {
		double delta = (double)x - res.avg;
		return acc + (delta * delta);
	};
	res.dev = std::sqrt(std::accumulate(v.begin(), v.end(), (double)0.0, dev_lambda)
			/ count);
	return res;
}

load_balance::load_balance(int allowed, int slots)
	: allowed(allowed), tbb(slots, 0)/*, cpu(slots, 0)*/
{
	/* nothing else */
}

void
load_balance::take_measurement()
{
	tbb[oneapi::tbb::this_task_arena::current_thread_index()]++;
	/*cpu[sched_getcpu()]++;*/
}

std::ostream&
operator<<(std::ostream& str, const load_balance& lb)
{
	statistics stats;
	std::vector<std::uint64_t> v;
	auto it = std::back_inserter(v);

	std::remove_copy(lb.tbb.begin(), lb.tbb.end(), it, 0);
	std::fill_n(it, lb.allowed - std::distance(v.begin(), v.end()), 0);
	stats = calc_statistics(v);
	str << stats.min << "\t" << stats.dev << "\t" << stats.max;

	/*
	str << "\t";

	std::remove_copy(lb.cpu.begin(), lb.cpu.end(), it, 0);
	std::fill_n(it, lb.allowed - std::distance(v.begin(), v.end()), 0);
	stats = calc_statistics(v);
	str << stats.min << "\t" << stats.dev << "\t" << stats.max;
	*/

	return str;
}
