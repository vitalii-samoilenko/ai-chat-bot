#include <chrono>
#include <cstdlib>
#include <cstring>
#include <span>
#include <string_view>
#include <thread>
#include <tuple>

#include "etelemetry.hpp"

#include "ai/chat/repositories/pipe.hpp"
#include "ai/chat/repositories/console.hpp"

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

class PipeConfig {
private:


public:
	PipeConfig() = default;
	PipeConfig(PipeConfig const &) = delete;
	PipeConfig(PipeConfig &&) = default;

	~PipeConfig() = default;

	PipeConfig &operator=(PipeConfig const &) = delete;
	PipeConfig &operator=(PipeConfig &&) = default;

	inline bool get_enabled() const {
		return true;
	};
};
class ThreadConfig {
private:
	PipeConfig const _pipeConfig;

public:
	ThreadConfig() = default;
	ThreadConfig(ThreadConfig const &) = delete;
	ThreadConfig(ThreadConfig &&) = default;

	~ThreadConfig() = default;

	ThreadConfig &operator=(ThreadConfig const &) = delete;
	ThreadConfig &operator=(ThreadConfig &&) = default;

	inline PipeConfig const &get_pipe() const {
		return _pipeConfig;
	};
};
class ConsoleConfig {
private:


public:
	ConsoleConfig() = default;
	ConsoleConfig(ConsoleConfig const &) = delete;
	ConsoleConfig(ConsoleConfig &&) = default;

	~ConsoleConfig() = default;

	ConsoleConfig &operator=(ConsoleConfig const &) = delete;
	ConsoleConfig &operator=(ConsoleConfig &&) = default;

	inline bool get_enabled() const {
		return true;
	};
	inline ::std::string_view get_name() const {
		return ::std::string_view{ "John Doe" };
	};
};
class ParticipantConfig {
private:
	ConsoleConfig const _consoleConfig;

public:
	ParticipantConfig() = default;
	ParticipantConfig(ParticipantConfig const &) = delete;
	ParticipantConfig(ParticipantConfig &&) = default;

	~ParticipantConfig() = default;

	ParticipantConfig &operator=(ParticipantConfig const &) = delete;
	ParticipantConfig &operator=(ParticipantConfig &&) = default;

	inline ConsoleConfig const &get_console() const {
		return _consoleConfig;
	};
};
class GlobalConfig {
private:
	ThreadConfig const _threadConfig;
	ParticipantConfig const _participantConfig;

public:
	GlobalConfig() = default;
	GlobalConfig(GlobalConfig const &) = delete;
	GlobalConfig(GlobalConfig &&) = default;

	~GlobalConfig() = default;

	GlobalConfig &operator=(GlobalConfig const &) = delete;
	GlobalConfig &operator=(GlobalConfig &&) = default;

	inline ThreadConfig const &get_thread() const {
		return _threadConfig;
	};
	inline ParticipantConfig const &get_participant() const {
		return _participantConfig;
	};
};

class CommunicationCluster {
private:
	GlobalConfig const _globalConfig;
	::std::tuple<
		::ai::chat::repositories::pipe<
			GlobalConfig,
			CommunicationCluster
		>
	> mutable _threadCluster;
	::std::tuple<
		::ai::chat::repositories::console<
			GlobalConfig,
			CommunicationCluster
		>
	> mutable _participantCluster;

public:
	inline CommunicationCluster(
	) : _globalConfig{}
		, _threadCluster{
			::ai::chat::repositories::pipe<
				GlobalConfig,
				CommunicationCluster
			>{ "pipe", _globalConfig, *this}
	} , _participantCluster{
			::ai::chat::repositories::console<
				GlobalConfig,
				CommunicationCluster
			>{ "console", _globalConfig, *this}
	} {
		auto console = ::std::get<0>(_participantCluster)
			.find("console", "John Doe");
		auto channel = ::std::get<0>(_threadCluster)
			.find("pipe", "Topic");
		console->join("pipe", "Topic");
		channel->accept("console", "John Doe");
	};
	CommunicationCluster(CommunicationCluster const &) = delete;
	CommunicationCluster(CommunicationCluster &&) = delete;

	~CommunicationCluster() = default;

	CommunicationCluster &operator=(CommunicationCluster const &) = delete;
	CommunicationCluster &operator=(CommunicationCluster &&) = delete;

	inline void operator()() const {
		auto console = ::std::get<0>(_participantCluster)
			.find("console", "John Doe");
		(*console)();
	};

	inline void push(
		::std::string_view partition,
		::std::string_view name,
		::std::string_view content,
		::std::span<
			::std::tuple<
				::std::string_view,
				::std::string_view
			>
		> tags
	) const {
		::std::apply([&](auto &...threadCluster)->void {
			([&]()->bool {
				auto channel = threadCluster.find(
					partition,
					name
				);
				if (channel == threadCluster.end())
					return false;
				channel->push(
					content,
					tags
				);
				return true;
			}() || ...);
		}, _threadCluster);
	};
	inline void notify(
		::std::string_view partition,
		::std::string_view name,
		long long timestamp,
		::std::string_view content,
		::std::span<
			::std::tuple<
				::std::string_view,
				::std::string_view
			>
		> tags
	) const {
		::std::apply([&](auto &...participantCluster)->void {
			([&]()->bool {
				auto recepient = participantCluster.find(
					partition,
					name
				);
				if (recepient == participantCluster.end())
					return false;
				recepient->notify(
					timestamp,
					content,
					tags
				);
				return true;
			}() || ...);
		}, _participantCluster);
	};
};

ET_SYSTEM_CALLBACKS();

int main(int argc, char const **argv) {
	char const *collector{ "localhost:8081" };
	{
		char const **current{ nullptr };
		for (--argc, ++argv; argc; --argc, ++argv) {
			if (::std::strcmp("--collector", *argv) == 0) {
				current = &collector;
			} else if(current) {
				*current = *argv;
				current = nullptr;
			}
		}
	}
	ET_INIT(collector, "ai_chat_hosts_console");
	CommunicationCluster cluster{};
	for (Spinner spinner{ 500, 2000 };;) {
		spinner.Spin();
		cluster();
	}
	return 0;
};
