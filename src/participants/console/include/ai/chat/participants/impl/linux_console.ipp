#ifndef AI_CHAT_PARTICIPANTS_IMPL_LINUX_CONSOLE_IPP
#define AI_CHAT_PARTICIPANTS_IMPL_LINUX_CONSOLE_IPP

#ifdef __unix__

#include <stdexcept>

#include <sys/epoll.h>
#include <unistd.h>

int g_console_count{ 0 };
int g_console_poll{ -1 };

void
console_open(
) {
	if (g_console_count) {
		++g_console_count;
		return;
	}
	if (g_console_poll != -1)
		throw ::std::runtime_error{
			"console open failed (1)"
		};
	int poll{ epoll_create(1) };
	if (poll == -1)
		throw ::std::runtime_error{
			"console open failed (2)"
		};
	epoll_event e{ EPOLLIN };
	if (epoll_ctl(poll,
			EPOLL_CTL_ADD,
			STDIN_FILENO, &e
		) == -1) {
			close(poll);
			throw ::std::runtime_error{
				"console open failed (3)"
			};
		}
	g_console_count = 1;
	g_console_poll = poll;
};
bool
console_try_read(
	::std::string_view *in
) {
	epoll_event e{};
	if (epoll_wait(g_console_poll,
			&e, 1,
			0) != 1)
		return false;
	static char _in[512];
	static char _[256];
	ssize_t n{
		read(STDIN_FILENO,
			_in, sizeof _in
		)
	};
	if (n == -1
		|| n == 0)
		return false;
	if (_in[n - 1] == '\n') {
		*in = ::std::string_view{
			_in, static_cast<size_t>(n - 1)
		};
	} else {
		*in = ::std::string_view{
			_in, static_cast<size_t>(n)
		};
		do {
			n = read(STDIN_FILENO,
				_, sizeof _
			);
			if (n == -1
				|| n == 0)
				break;
		} while (_[n - 1] != '\n');
	}
	return true;
};
void
console_write(
	::std::string_view out
) {
	ssize_t n{
		write(STDOUT_FILENO,
			out.data(), sizeof(char) * out.length()
		)
	};
	if (n == -1)
		throw ::std::runtime_error{
			"console write failed (1)"
		};
};
void
console_close(
) {
	if (1 < g_console_count) {
		--g_console_count;
		return;
	}
	g_console_count = 0;
	if (close(g_console_poll) == -1)
		return;
	g_console_poll = -1;
};

#endif

#endif
