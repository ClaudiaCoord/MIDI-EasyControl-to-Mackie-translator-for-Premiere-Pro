/*
	MIDI-MT is a high level application driver for USB MIDI control surface.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (Remote Controls Plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!

	Original authored by a Ole Christian Eidheim: https://gitlab.com/eidheim
*/

#pragma once

namespace Common {
	namespace WS {
		namespace error = asio::error;
		namespace make_error_code = std;

		using error_code = std::error_code;
		using errc = std::errc;
		using system_error = std::system_error;

		using io_context = asio::io_context;
		using resolver_results = asio::ip::tcp::resolver::results_type;
		using async_connect_endpoint = asio::ip::tcp::endpoint;
		using strand = asio::strand<asio::any_io_executor>;

		template <typename handler_type>
		inline void post(io_context& context, handler_type&& handler) {
			asio::post(context, std::forward<handler_type>(handler));
		}
		inline void restart(io_context& context) {
			context.restart();
		}
		inline asio::ip::address make_address(const std::string& str) {
			return asio::ip::make_address(str);
		}
		template <typename socket_type, typename duration_type>
		inline std::unique_ptr<asio::steady_timer> make_steady_timer(socket_type& socket, std::chrono::duration<duration_type> duration) {
			return std::unique_ptr<asio::steady_timer>(new asio::steady_timer(socket.get_executor(), duration));
		}
		template <typename handler_type>
		inline void async_resolve(asio::ip::tcp::resolver& resolver, const std::pair<std::string, std::string>& host_port, handler_type&& handler) {
			resolver.async_resolve(host_port.first, host_port.second, std::forward<handler_type>(handler));
		}
		inline asio::executor_work_guard<io_context::executor_type> make_work_guard(io_context& context) {
			return asio::make_work_guard(context);
		}
		template <typename socket_type>
		inline asio::basic_socket<asio::ip::tcp>::executor_type get_executor(socket_type& socket) {
			return socket.get_executor();
		}
		template <typename execution_context, typename handler_type>
		inline asio::executor_binder<typename asio::decay<handler_type>::type, typename execution_context::executor_type> bind_executor(strand& strand, handler_type&& handler) {
			return asio::bind_executor(strand, std::forward<handler_type>(handler));
		}
	}
}
