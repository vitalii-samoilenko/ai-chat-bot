#ifndef AI_CHAT_STUBS_IMPL_LINUX_CONSOLE_IPP
#define AI_CHAT_STUBS_IMPL_LINUX_CONSOLE_IPP

#ifdef __unix__

#include <stdexcept>

#include <fcntl.h>
#include <unistd.h>

#include "ai/chat/stubs/impl/console.ipp"

int g_stub_console_in_pipe[]{ -1, -1 };
int g_stub_console_out_pipe[]{ -1, -1 };
int g_stub_console_in_bkp{ -1 };
int g_stub_console_out_bkp{ -1 };

::ai::chat::stubs::console::console(
) {
	pipe2(g_stub_console_in_pipe, O_NONBLOCK);
	pipe2(g_stub_console_out_pipe, O_NONBLOCK);
	g_stub_console_in_bkp = dup(STDIN_FILENO);
	g_stub_console_out_bkp = dup(STDOUT_FILENO);
	if (g_stub_console_in_bkp == -1
		|| g_stub_console_out_bkp == -1
		|| dup2(g_stub_console_in_pipe[0], STDIN_FILENO) == -1
		|| dup2(g_stub_console_out_pipe[1], STDOUT_FILENO) == -1) {
		dup2(g_stub_console_out_bkp, STDOUT_FILENO);
		dup2(g_stub_console_in_bkp, STDIN_FILENO);
		close(g_stub_console_out_bkp);
		close(g_stub_console_in_bkp);
		close(g_stub_console_out_pipe[0]);
		close(g_stub_console_out_pipe[1]);
		close(g_stub_console_in_pipe[0]);
		close(g_stub_console_in_pipe[1]);
		throw ::std::runtime_error{
			"console stub failed"
		};
	}
};

::ai::chat::stubs::console::~console(
) {
	char _out[512];
	ssize_t n;
	do {
		n = ::read(g_stub_console_out_pipe[0],
			_out, sizeof _out
		);
		if (0 < n)
			::write(g_stub_console_out_bkp,
				_out, n
			);
	} while (0 < n);
	dup2(g_stub_console_out_bkp, STDOUT_FILENO);
	dup2(g_stub_console_in_bkp, STDIN_FILENO);
	close(g_stub_console_out_bkp);
	close(g_stub_console_in_bkp);
	close(g_stub_console_out_pipe[0]);
	close(g_stub_console_out_pipe[1]);
	close(g_stub_console_in_pipe[0]);
	close(g_stub_console_in_pipe[1]);
};

void
::ai::chat::stubs::console::write(
	::std::string_view input
) const {
	if (::write(g_stub_console_in_pipe[1],
			input.data(), sizeof(char) * input.length()
		) == -1)
		throw ::std::runtime_error{
			"console stub write failed"
		};
};
::std::string_view
ai::chat::stubs::console::read(
) const {
	static char _out[512];
	static char _[256];
	ssize_t n{
		::read(g_stub_console_out_pipe[0],
			_out, sizeof _out
		)
	};
	if (n == -1)
		throw ::std::runtime_error{
			"console stub read failed"
		};
	::std::string_view output{
		_out, static_cast<size_t>(n)
	};
	do {
		n = ::read(g_stub_console_out_pipe[0],
			_, sizeof _
		);
	} while (0 < n);
	return output;
};

#endif

#endif
