#include <sched.h>
#include <sys/sysinfo.h>

#include "observers.hpp"
#include "utils.hpp"

pinning_observer::pinning_observer(oneapi::tbb::task_arena &arena):
	oneapi::tbb::task_scheduler_observer(arena),
	nprocs(get_nprocs()),
	counter(0)
{
	observe(true);
}

void
pinning_observer::on_scheduler_entry(bool is_worker)
{
	cpu_set_t *mask = CPU_ALLOC(nprocs);
	auto mask_size = CPU_ALLOC_SIZE(nprocs);
	CPU_ZERO_S(mask_size, mask);
	CPU_SET_S(counter++ % nprocs, mask_size, mask);
	if (sched_setaffinity(0, mask_size, mask))
		die("sched_setaffinity: " << std::strerror(errno));
}
