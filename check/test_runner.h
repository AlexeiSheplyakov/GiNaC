#ifndef GINAC_CHECK_TEST_RUNNER_H
#define GINAC_CHECK_TEST_RUNNER_H
#include <iostream>
#include <cstdlib>
#include <string>
#include <ctime>
#include "timer.h"

template<typename T> static void 
run_benchmark(T& benchmark,
	      const unsigned ntests = 10,
	      const std::clock_t t_annoying = 15*CLOCKS_PER_SEC,
	      const std::clock_t t_min = CLOCKS_PER_SEC/100)
{
	const std::clock_t start(std::clock());
	std::clock_t elapsed(start);

	timer P36;
	unsigned n = 0;
	double t = 0;
	bool go_on = true;
	do {
		++n;
		P36.start();
		benchmark.run();
		t += P36.read();
		go_on = benchmark.check();
		if (!go_on)
			break;
		elapsed = std::clock() - start;
		if (elapsed > t_annoying)
			break;
	} while (n <= ntests || elapsed < t_min);
	t /= n;
	benchmark.print_result(t);
}

// By default long-running timings are disabled (to not annoy the user).
// If the GINAC_RUN_EXPENSIVE_TIMINGS environment variable is set to "1",
// some of them (which are supposed to be relatively fast) will be enabled.
// If GINAC_RUN_EXPENSIVE_TIMINGS is set to "2", all timings are enabled.
static int run_expensive_timings_p()
{
	static int value = 0;
	static int cc = 0;
	static const std::string env_name("GINAC_RUN_EXPENSIVE_TIMINGS");
	if (cc++ == 0) {
		char* envvar = std::getenv(env_name.c_str());
		if (envvar != NULL) {
			value = std::atoi(envvar);
			if (value < 0 || value > 2)
				value = 0;
		}
		if (value) {
			std::cerr << "WARNING: "
				<< "long-running timings are ENABLED."
				<< std::endl
				<< "Unset the \"" << env_name << "\" "
				<< "environment variable skip them."
				<< std::endl;
		}
	}
	return value;
}

#endif // GINAC_CHECK_TEST_RUNNER_H

