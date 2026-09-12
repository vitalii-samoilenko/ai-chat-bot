#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>

#include "etelemetry.hpp"

class Spinner {
private:
	size_t _min;
	size_t _delta;
	ET_DECLARE_LOGGER();
	ET_DECLARE_TRACER();
	ET_DECLARE_METER();
	ET_DECLARE_COUNTER(spins);
	ET_DECLARE_GAUGE(tick);

public:
	Spinner(
		size_t min,
		size_t max)
	: _min{ min }
	, _delta{ max - min }
	a_ET_INIT_LOGGER("spinner")
	a_ET_INIT_TRACER("spinner")
	a_ET_INIT_METER("spinner")
	a_ET_INIT_COUNTER(spins, "spinner_spins")
	a_ET_INIT_GAUGE(tick, "spinner_tick") {

	};

	void Spin() {
		ET_START_SPAN((*this), spin, "spinner_spin");
		ET_START_SUBSPAN((*this), spin, pre_spin, "spinner_pre_spin");
		ET_LOG_INFO((*this), pre_spin, "Starting spin...");
		size_t timeout{ _min + ::std::rand() % _delta };
		ET_ADD_COUNTER((*this), spins, 1, {
			ET_TAG("type", "started")
		});
		ET_RECORD_GAUGE((*this), tick, timeout, {
			ET_TAG("type", "expected")
		});
		ET_STOP_SPAN(pre_spin);
		auto begin = ::std::chrono::high_resolution_clock::now();
		::std::this_thread::sleep_for(::std::chrono::milliseconds{ timeout });
		auto end = ::std::chrono::high_resolution_clock::now();
		ET_START_SUBSPAN((*this), spin, post_spin, "spinner_post_spin");
		auto duration = ::std::chrono::duration_cast<::std::chrono::milliseconds>(end - begin);
		ET_RECORD_GAUGE((*this), tick, duration.count(), {
			ET_TAG("type", "actual")
		});
		ET_ADD_COUNTER((*this), spins, 1, {
			ET_TAG("type", "finished")
		});
		ET_LOG_INFO((*this), post_spin, "Finishing spin...");
		ET_STOP_SPAN(post_spin);
		ET_STOP_SPAN(spin);
	};
};

ET_SYSTEM_CALLBACKS();

int main(int argc, char const **argv) {
	ET_INIT("localhost:8081", "ai_chat_hosts_console");
	for (Spinner spinner{ 500, 2000 };;) {
		spinner.Spin();
	}
	return 0;
};
