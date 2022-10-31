/* TBB task_scheduler_observers.
 *
 * normal_observer: synonym for oneapi::tbb::task_scheduler_observer
 * pinning_observer: pins TBB threads to the lowest number CPU thread
 */

#pragma once

#include <atomic>
#include <oneapi/tbb/spin_mutex.h>
#include <oneapi/tbb/task_arena.h>
#include <oneapi/tbb/task_scheduler_observer.h>

typedef oneapi::tbb::task_scheduler_observer normal_observer;

/* Observer that will (do its best to) pin TBB threads to hardware threads with
 * the lowest index. In theory, this will put as many TBB threads on the same
 * NUMA node as possible (see `lscpu | grep NUMA`).
 *
 * This is based off of the implementation given in Chapter 13 of ProTBB.
 */
class pinning_observer : public oneapi::tbb::task_scheduler_observer {
	const unsigned nprocs;
	std::atomic<unsigned> counter;
public:
	pinning_observer(oneapi::tbb::task_arena &arena);
	void on_scheduler_entry(bool is_worker);
};
