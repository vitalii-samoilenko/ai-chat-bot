#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>

#include "ai/chat/telemetry.hpp"

class Spinner {
private:
	size_t _min;
	size_t _delta;
	DECLARE_LOGGER();
	DECLARE_TRACER();
	DECLARE_METER();
	DECLARE_COUNTER(spins);
	DECLARE_GAUGE(tick);

public:
	Spinner(
		size_t min,
		size_t max)
	: _min{ min }
	, _delta{ max - min }
	a_INIT_LOGGER("spinner")
	a_INIT_TRACES("spinner")
	a_INIT_METER("spinner")
	a_INIT_COUNTER(spins, "spinner_spins")
	a_INIT_GAUGE(tick, "spinner_tick") {

	};

	void Spin() {
		START_SPAN((*this), spin, "spinner_spin");
		START_SUBSPAN((*this), spin, pre_spin, "spinner_pre_spin");
		LOG_INFO((*this), pre_spin, "Starting spin...");
		size_t timeout{ _min + ::std::rand() % _delta };
		ADD_COUNTER(spins, {
			TAG("type", "started")
		});
		RECORD_GAUGE(tick, timeout, {
			TAG("type", "expected")
		});
		STOP_SPAN(pre_spin);
		auto begin = ::std::chrono::high_resolution_clock::now();
		::std::this_thread::sleep_for(::std::chrono::milliseconds{ timeout });
		auto end = ::std::chrono::high_resolution_clock::now();
		START_SUBSPAN((*this), spin, post_spin, "spinner_post_spin");
		auto duration = ::std::chrono::duration_cast<::std::chrono::milliseconds>(end - begin);
		RECORD_GAUGE(tick, duration.count(), {
			TAG("type", "actual")
		});
		ADD_COUNTER(spins, {
			TAG("type", "finished")
		});
		LOG_INFO((*this), post_spin, "Finishing spin...");
		STOP_SPAN(post_spin);
		STOP_SPAN(spin);
	};
};

SYSTEM_CALLBACKS();

int main(int argc, char const **argv) {
	char const *collector{ "localhost:8081" };
	for (++argv; argv; ++argv) {
		if (strcmp(argv, "--collector"))
			continue;
		collector = ++argv;
		break;
	}
	INIT_TELEMETRY(collector, "ai_chat_hosts_console");
	for (Spinner spinner{ 500, 2000 };;) {
		spinner.Spin();
	}
	return 0;
};
