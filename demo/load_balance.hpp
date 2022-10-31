/* Measure load balancing for TBB.
 */

#pragma once

#include <ostream>
#include <cstdint>
#include <vector>

/* Currently, only tracks TBB threads. It seems only in rare cases when not
 * pinning on very small workloads do TBB threads not map 1:1 (or at least
 * very closely) with hardware threads.
 */
struct load_balance {
	int allowed;
	std::vector<std::uint64_t> tbb/*, cpu*/;

	load_balance(int allowed, int total_hardware);
	void take_measurement();
};

/* Put the minimum, standard deviation, and maximum separated by tabs */
std::ostream& operator<<(std::ostream& str, const load_balance& lb);
