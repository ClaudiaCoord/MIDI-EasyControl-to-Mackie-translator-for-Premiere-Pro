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
		namespace regex = std;
		using AWS = asio::ip::tcp::socket;
		using namespace std::string_view_literals;

		class Names {
		public:
			static constexpr std::string_view Upgrade = "Upgrade"sv;
			static constexpr std::string_view Connection = "Connection"sv;
			static constexpr std::string_view WebSocket = "websocket"sv;
			static constexpr std::string_view WebSocketKey = "Sec-WebSocket-Key"sv;
			static constexpr std::string_view WebSocketAccept = "Sec-WebSocket-Accept"sv;
			static constexpr std::string_view MessageMasked = "message from client not masked"sv;
			static constexpr std::string_view MessageBig = "message too big"sv;

			static constexpr std::string_view HTTP11 = "HTTP/1.1 "sv;
			static constexpr std::string_view HTTP101 = "101 Web Socket Protocol Handshake\r\n"sv;
			static constexpr std::string_view HTTP200 = "200 OK"sv;
			static constexpr std::string_view UA = "Default 1.0"sv;

			static constexpr std::string_view Date = "Date: "sv;
			static constexpr std::string_view Server = "Server: "sv;
			static constexpr std::string_view LastModified = "Last-Modified: "sv;
			static constexpr std::string_view ContentLength = "Content-Length: "sv;
			static constexpr std::string_view ContentType = "Content-Type: "sv;
			static constexpr std::string_view ConnectionClosed = "Connection: Closed"sv;

			static constexpr std::string_view TextHtml = "text/html"sv;
			static constexpr std::string_view NewLine = "\r\n"sv;
		};

		template <class socket_type>
		class SocketServer;

		template <class socket_type>
		class SocketServerBase {
		public:
			class InMessage : public std::istream {
				friend class SocketServerBase<socket_type>;

			public:
				unsigned char fin_rsv_opcode{ 0 };
				std::size_t size() {
					return length;
				}
				std::string string() {
					return std::string(asio::buffers_begin(streambuf.data()), asio::buffers_end(streambuf.data()));
				}

			private:
				InMessage() : std::istream(&streambuf), length(0) {}
				InMessage(unsigned char fin_rsv_opcode, std::size_t length) : std::istream(&streambuf), fin_rsv_opcode(fin_rsv_opcode), length(length) {}
				std::size_t length;
				asio::streambuf streambuf;
			};
			class OutMessage : public std::ostream {
				friend class SocketServerBase<socket_type>;
				asio::streambuf streambuf{};

			public:
				OutMessage() : std::ostream(&streambuf) {}
				OutMessage(std::size_t capacity) : std::ostream(&streambuf) {
					streambuf.prepare(capacity);
				}
				std::size_t size() const {
					return streambuf.size();
				}
			};
			class Connection : public std::enable_shared_from_this<Connection> {
				friend class SocketServerBase<socket_type>;
				friend class SocketServer<socket_type>;

			public:
				Connection(std::unique_ptr<socket_type>&& socket_)
					: socket(std::move(socket_)), read_write_strand(get_executor(socket->lowest_layer())), timeoutidle(0), close_sent(false) {}

				std::string method{}, path{}, query_string{}, http_version{};
				CaseInsensitiveMultimap header{};
				regex::smatch path_match{};

			private:
				template <typename... Args>
				Connection(std::shared_ptr<ScopeRunner> handler_runner_, long timeoutidle, Args &&...args) noexcept
					: handler_runner(std::move(handler_runner_)), socket(new socket_type(std::forward<Args>(args)...)), read_write_strand(get_executor(socket->lowest_layer())), timeoutidle(timeoutidle), close_sent(false) {}

				std::mutex lock_timer_{};
				std::mutex lock_send_queue_{};

				std::unique_ptr<socket_type> socket{};
				std::unique_ptr<asio::steady_timer> timer{};
				std::shared_ptr<ScopeRunner> handler_runner{};
				std::shared_ptr<InMessage> fragmented_in_message{};

				asio::ip::tcp::endpoint endpoint{};
				asio::streambuf streambuf{};
				strand read_write_strand{};

				std::atomic<bool> close_sent{ false };
				long timeoutidle{ 0 };

				void set_timeout(long seconds = -1) {
					if (seconds == -1)
						seconds = timeoutidle;

					std::unique_lock<std::mutex> lock_(lock_timer_);

					if (seconds == 0) {
						timer = nullptr;
						return;
					}

					timer = make_steady_timer(*socket, std::chrono::seconds(seconds));
					std::weak_ptr<Connection> connection_weak(this->shared_from_this());
					timer->async_wait([connection_weak](const error_code& ec) {
						if (!ec) {
							if (auto connection = connection_weak.lock())
								connection->close();
						}
					});
				}
				void cancel_timeout() {
					std::unique_lock<std::mutex> lock_(lock_timer_);
					if (timer) {
						try {
							timer->cancel();
						} catch (...) {
						}
					}
				}

				class OutData {
				public:
					OutData(std::shared_ptr<OutMessage> out_header_, std::shared_ptr<OutMessage> out_message_,
						std::function<void(const error_code&)>&& callback_) noexcept
						: out_header(std::move(out_header_)), out_message(std::move(out_message_)), callback(std::move(callback_)) {}
					std::shared_ptr<OutMessage> out_header{};
					std::shared_ptr<OutMessage> out_message{};
					std::function<void(const error_code&)> callback = [](auto a) {};
				};

				std::list<OutData> send_queue_{};

				void send_from_queue() {
					std::array<asio::const_buffer, 2> buffers{ send_queue_.begin()->out_header->streambuf.data(), send_queue_.begin()->out_message->streambuf.data() };
					auto self = this->shared_from_this();
					set_timeout();
					post(read_write_strand, [self, buffers] {
						auto lock = self->handler_runner->continue_lock();
						if (!lock)
							return;

						asio::async_write(*self->socket, buffers, [self](const error_code& ec, std::size_t) {
							self->set_timeout();
							auto lock = self->handler_runner->continue_lock();
							if (!lock)
								return;
							{
								std::unique_lock<std::mutex> lock_(self->lock_send_queue_);

								if (!ec) {
									auto it = self->send_queue_.begin();
									auto callback = std::move(it->callback);
									self->send_queue_.erase(it);
									if (self->send_queue_.size() > 0)
										self->send_from_queue();

									lock_.unlock();
									if (callback)
										callback(ec);
								}
								else {
									std::list<std::function<void(const error_code&)>> callbacks{};
									for (auto& out_data : self->send_queue_) {
										if (out_data.callback)
											callbacks.emplace_back(std::move(out_data.callback));
									}
									self->send_queue_.clear();

									lock_.unlock();
									for (auto& callback : callbacks)
										callback(ec);
								}
							}
						});
					});
				}

			public:
				void send(std::shared_ptr<OutMessage> out_message, std::function<void(const error_code&)> callback = nullptr, unsigned char fin_rsv_opcode = 129) {
					std::size_t length = out_message->size();

					auto out_header = std::make_shared<OutMessage>(10);

					out_header->put(static_cast<char>(fin_rsv_opcode));
					if (length >= 126) {
						std::size_t num_bytes;
						if (length > 0xffff) {
							num_bytes = 8;
							out_header->put(127);
						}
						else {
							num_bytes = 2;
							out_header->put(126);
						}

						for (std::size_t c = num_bytes - 1; c != static_cast<std::size_t>(-1); c--)
							out_header->put((static_cast<unsigned long long>(length) >> (8 * c)) % 256);
					}
					else
						out_header->put(static_cast<char>(length));

					std::unique_lock<std::mutex> lock_(lock_send_queue_);
					send_queue_.emplace_back(std::move(out_header), std::move(out_message), std::move(callback));
					if (send_queue_.size() == 1)
						send_from_queue();
				}
				void send(std::string_view out_message_str, std::function<void(const error_code&)> callback = nullptr, unsigned char fin_rsv_opcode = 129) {
					auto out_message = std::make_shared<OutMessage>();
					out_message->write(out_message_str.data(), static_cast<std::streamsize>(out_message_str.size()));
					send(out_message, std::move(callback), fin_rsv_opcode);
				}
				void send_close(int status, const std::string& reason = "", std::function<void(const error_code&)> callback = nullptr) {
					if (close_sent)
						return;
					close_sent = true;

					auto send_stream = std::make_shared<OutMessage>();

					send_stream->put(status >> 8);
					send_stream->put(status % 256);
					*send_stream << reason;

					send(std::move(send_stream), std::move(callback), 136);
				}
				void close() {
					error_code ec;
					socket->lowest_layer().shutdown(asio::ip::tcp::socket::shutdown_both, ec);
					socket->lowest_layer().cancel(ec);
				}

				const asio::ip::tcp::endpoint& remote_endpoint() const {
					return endpoint;
				}
				asio::ip::tcp::endpoint local_endpoint() const {
					try {
						if (auto connection = this->connection.lock())
							return connection->socket->lowest_layer().local_endpoint();
					} catch (...) {}
					return asio::ip::tcp::endpoint();
				}
				FLAG_DEPRECATED std::string remote_endpoint_address() const {
					try {
						return endpoint.address().to_string();
					} catch (...) {}
					return std::string();
				}
				FLAG_DEPRECATED unsigned short remote_endpoint_port() const {
					try {
						return endpoint.port();
					} catch (...) {}
					return 0;
				}
			};
			class Endpoint {
				friend class SocketServerBase<socket_type>;

			private:
				std::mutex lock_connections_{};
				std::unordered_set<std::shared_ptr<Connection>> connections{};

			public:
				std::function<StatusCode(std::shared_ptr<Connection>, CaseInsensitiveMultimap&)> on_handshake;
				std::function<void(std::shared_ptr<Connection>)> on_open;
				std::function<void(std::shared_ptr<Connection>, std::shared_ptr<InMessage>)> on_message;
				std::function<void(std::shared_ptr<Connection>, int, const std::string&)> on_close;
				std::function<void(std::shared_ptr<Connection>, const error_code&)> on_error;
				std::function<void(std::shared_ptr<Connection>)> on_ping;
				std::function<void(std::shared_ptr<Connection>)> on_pong;

				std::unordered_set<std::shared_ptr<Connection>> get_connections() {
					std::unique_lock<std::mutex> lock(lock_connections_);
					auto copy = connections;
					return copy;
				}
			};
			class Config {
				friend class SocketServerBase<socket_type>;

			private:
				Config(unsigned short port) {
					cnf.port = port;
					cnf.server_ua = Names::UA;
					content_modified = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
				}

			public:
				REMOTE::RemoteConfig<std::string> cnf{};

				std::size_t thread_pool_size{ 1 };
				std::size_t max_message_size = (std::numeric_limits<std::size_t>::max)();
				std::chrono::local_time<std::chrono::system_clock::duration> content_modified{};
				CaseInsensitiveMultimap header{};
			};
			Config config{};

		private:
			class regex_orderable : public regex::regex {
			public:
				std::string str{};
				regex_orderable(const char* regex_cstr) : regex::regex(regex_cstr), str(regex_cstr) {}
				regex_orderable(const std::string& regex_str) : regex::regex(regex_str), str(regex_str) {}
				bool operator<(const regex_orderable& rhs) const {
					return str < rhs.str;
				}
			};
			asio::ip::tcp::endpoint get_endpoint_(bool b, unsigned short port) {
				return asio::ip::tcp::endpoint(b ? asio::ip::tcp::v6() : asio::ip::tcp::v4(), port);
			}

		public:
			std::map<regex_orderable, Endpoint> endpoint{};
			std::function<const std::string()> get_content = []() { return std::string(); };

			const bool IsRun() const {
				return is_run.load();
			}
			void start(const std::function<void(unsigned short)>& callback = nullptr) {
				if (!config.cnf.enable) {
					to_log::Get() << (log_string() << L"Start disabled in configuration, abort..");
					return;
				}
				if (config.cnf.empty()) {
					to_log::Get() << (log_string() << L"Start not posibled, configuration is empty, abort..");
					return;
				}
				std::unique_lock<std::mutex> lock(lock_start_stop_);

				if (!io_service) {
					io_service = std::make_shared<io_context>();
					internal_io_service = true;
				}
				if (!acceptor)
					acceptor = std::unique_ptr<asio::ip::tcp::acceptor>(new asio::ip::tcp::acceptor(*io_service));

				asio::ip::tcp::endpoint endpoint{};
				if (!config.cnf.host.empty())
					endpoint = asio::ip::tcp::endpoint(make_address(config.cnf.host), config.cnf.port);
				else
					endpoint = get_endpoint_(config.cnf.isipv6, config.cnf.port);

				try {
					acceptor->open(endpoint.protocol());
				} catch (const system_error& error) {
					if (error.code() == asio::error::address_family_not_supported && config.cnf.host.empty() && config.cnf.isipv6) {
						to_log::Get() << (log_string() << L"IPV6 network not supported by system, trying IPV4...");
						endpoint = get_endpoint_(false, config.cnf.port);
						acceptor->open(endpoint.protocol());
					}
					else throw;
				}
				acceptor->set_option(asio::socket_base::reuse_address(config.cnf.isreuseaddr));

				#if defined(TCP_FASTOPEN)
				if (config.cnf.isfastsocket) {
					const int qlen = 5;
					error_code ec{};
					acceptor->set_option(asio::detail::socket_option::integer<IPPROTO_TCP, TCP_FASTOPEN>(qlen), ec);
				}
				#endif
				acceptor->bind(endpoint);

				after_bind();

				auto port = acceptor->local_endpoint().port();

				acceptor->listen();
				accept();

				if (internal_io_service && io_service->stopped())
					restart(*io_service);

				if (callback)
					post(*io_service, [callback, port] {
					callback(port);
				});

				if (internal_io_service) {
					threads.clear();
					for (std::size_t c = 1; c < config.thread_pool_size; c++) {
						threads.emplace_back([this]() {
							this->io_service->run();
						});
					}

					lock.unlock();

					if (config.thread_pool_size > 0)
						io_service->run();

					lock.lock();

					for (auto& t : threads)
						t.join();
				}
				is_run.store(true);
			}
			void stop() {
				std::lock_guard<std::mutex> lock_(lock_start_stop_);
				is_run.store(false);

				if (acceptor) {
					error_code ec;
					acceptor->close(ec);

					for (auto& pair : endpoint) {
						std::unique_lock<std::mutex> lock(pair.second.lock_connections_);
						for (auto& connection : pair.second.connections)
							connection->close();
						pair.second.connections.clear();
					}

					if (internal_io_service)
						io_service->stop();
				}
			}
			void stop_accept() {
				if (acceptor) {
					error_code ec{};
					acceptor->close(ec);
				}
			}

			virtual ~SocketServerBase() = default;

			std::unordered_set<std::shared_ptr<Connection>> get_connections() {
				std::unordered_set<std::shared_ptr<Connection>> all_connections;
				for (auto& e : endpoint) {
					std::unique_lock<std::mutex> lock(e.second.lock_connections_);
					all_connections.insert(e.second.connections.begin(), e.second.connections.end());
				}
				return all_connections;
			}
			void upgrade(const std::shared_ptr<Connection>& connection) {
				connection->handler_runner = handler_runner;
				connection->timeoutidle = config.cnf.timeoutidle;
				write_handshake(connection);
			}
			std::shared_ptr<io_context> io_service{};

		protected:
			std::mutex lock_send_queue_{};
			std::mutex lock_start_stop_{};
			bool internal_io_service{ false };
			std::atomic<bool> is_run{ false };

			std::unique_ptr<asio::ip::tcp::acceptor> acceptor{};
			std::vector<std::thread> threads{};

			std::shared_ptr<ScopeRunner> handler_runner{};

			SocketServerBase(unsigned short port) : config(port), handler_runner(new ScopeRunner()) {}

			virtual void after_bind() {}
			virtual void accept() = 0;

			void write_static_content(const std::shared_ptr<Connection>& connection) {
				try {
					auto streambuf = std::make_shared<asio::streambuf>();
					std::ostream oss(streambuf.get());
					const std::string s = get_content();

					oss << Names::HTTP11 << Names::HTTP200 << Names::NewLine;
					oss << Names::Server << config.cnf.server_ua.c_str() << Names::NewLine;
					oss << Names::Date
						<< std::format("{:%a, %d %b %Y %H:%M:%OS GMT}", std::chrono::current_zone()->to_local(std::chrono::system_clock::now()))
						<< Names::NewLine;
					oss << Names::LastModified
						<< std::format("{:%a, %d %b %Y %H:%M:%OS GMT}", config.content_modified)
						<< Names::NewLine;
					oss << Names::ContentLength << s.length() << Names::NewLine;
					oss << Names::ContentType << Names::TextHtml << Names::NewLine;
					oss << Names::ConnectionClosed << Names::NewLine << Names::NewLine;
					oss << s.c_str();

					connection->set_timeout(config.cnf.timeoutreq);
					asio::async_write(*connection->socket, *streambuf, [this, connection, streambuf](const error_code& ec, std::size_t) {
						connection->cancel_timeout();
					}
					);
				} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			}
			void read_handshake(const std::shared_ptr<Connection>& connection) {
				connection->set_timeout(config.cnf.timeoutreq);
				asio::async_read_until(*connection->socket, connection->streambuf, "\r\n\r\n", [this, connection](const error_code& ec, std::size_t) {
					connection->cancel_timeout();
					auto lock = connection->handler_runner->continue_lock();
					if (!lock)
						return;
					if (!ec) {
						std::istream istream(&connection->streambuf);
						if (RequestMessage::parse(istream, connection->method, connection->path, connection->query_string, connection->http_version, connection->header))
							write_handshake(connection);
					}
				}
				);
			}
			void write_handshake(const std::shared_ptr<Connection>& connection) {
				if (connection && connection->path._Equal("/") && connection->method._Equal("GET")) {
					write_static_content(connection);
					return;
				}
				for (auto& regex_endpoint : endpoint) {
					regex::smatch path_match{};
					if (regex::regex_match(connection->path, path_match, regex_endpoint.first)) {
						auto streambuf = std::make_shared<asio::streambuf>();
						std::ostream oss(streambuf.get());

						StatusCode status = StatusCode::information_switching_protocols;
						auto key_it = connection->header.find(Names::WebSocketKey.data());
						if (key_it == connection->header.end())
							status = StatusCode::client_error_upgrade_required;
						else {
							CaseInsensitiveMultimap response_header = config.header;
							response_header.emplace(Names::Upgrade, Names::WebSocket);
							response_header.emplace(Names::Connection, Names::Upgrade);

							try {
								response_header.emplace(Names::WebSocketAccept, WS::AKEY::sec_websocket(key_it->second));
							} catch (...) {
							}

							try {
								connection->endpoint = connection->socket->lowest_layer().remote_endpoint();
							} catch (...) {
							}

							if (regex_endpoint.second.on_handshake)
								status = regex_endpoint.second.on_handshake(connection, response_header);

							if (status == StatusCode::information_switching_protocols) {
								oss << Names::HTTP11 << Names::HTTP101;
								for (auto& header_field : response_header)
									oss << header_field.first << ": " << header_field.second << Names::NewLine;
								oss << Names::NewLine;
							}
						}
						if (status != StatusCode::information_switching_protocols)
							oss << Names::HTTP11 << WS::status_code(status) << Names::NewLine << Names::NewLine;

						connection->path_match = std::move(path_match);
						connection->set_timeout(config.cnf.timeoutreq);
						asio::async_write(*connection->socket, *streambuf, [this, connection, streambuf, &regex_endpoint, status](const error_code& ec, std::size_t) {
							connection->cancel_timeout();
							auto lock = connection->handler_runner->continue_lock();
							if (!lock)
								return;
							if (status != StatusCode::information_switching_protocols)
								return;

							if (!ec) {
								connection_open(connection, regex_endpoint.second);
								read_message(connection, regex_endpoint.second);
							}
							else connection_error(connection, regex_endpoint.second, ec);
						}
						);
						return;
					}
				}
			}
			void read_message(const std::shared_ptr<Connection>& connection, Endpoint& endpoint) const {
				connection->set_timeout();
				post(connection->read_write_strand, [this, connection, &endpoint] {
					auto lock = connection->handler_runner->continue_lock();
					if (!lock)
						return;
					asio::async_read(*connection->socket, connection->streambuf, asio::transfer_exactly(2), bind_executor(connection->read_write_strand, [this, connection, &endpoint](const error_code& ec, std::size_t bytes_transferred) {
						connection->cancel_timeout();
						auto lock = connection->handler_runner->continue_lock();
						if (!lock)
							return;
						if (!ec) {
							if (bytes_transferred == 0) {
								read_message(connection, endpoint);
								return;
							}
							std::istream istream(&connection->streambuf);

							std::array<unsigned char, 2> first_bytes;
							istream.read((char*)&first_bytes[0], 2);

							unsigned char fin_rsv_opcode = first_bytes[0];

							if (first_bytes[1] < 128) {
								const std::string reason(Names::MessageMasked);
								connection->send_close(1002, reason);
								connection_close(connection, endpoint, 1002, reason);
								return;
							}

							std::size_t length = (first_bytes[1] & 127);

							if (length == 126) {
								connection->set_timeout();
								asio::async_read(*connection->socket, connection->streambuf, asio::transfer_exactly(2), bind_executor(connection->read_write_strand, [this, connection, &endpoint, fin_rsv_opcode](const error_code& ec, std::size_t) {
									connection->cancel_timeout();
									auto lock = connection->handler_runner->continue_lock();
									if (!lock)
										return;
									if (!ec) {
										std::istream istream(&connection->streambuf);

										std::array<unsigned char, 2> length_bytes;
										istream.read((char*)&length_bytes[0], 2);

										std::size_t length = 0;
										std::size_t num_bytes = 2;
										for (std::size_t c = 0; c < num_bytes; c++)
											length += static_cast<std::size_t>(length_bytes[c]) << (8 * (num_bytes - 1 - c));

										read_message_content(connection, length, endpoint, fin_rsv_opcode);
									}
									else
										connection_error(connection, endpoint, ec);
								}));
							}
							else if (length == 127) {
								connection->set_timeout();
								asio::async_read(*connection->socket, connection->streambuf, asio::transfer_exactly(8), bind_executor(connection->read_write_strand, [this, connection, &endpoint, fin_rsv_opcode](const error_code& ec, std::size_t) {
									connection->cancel_timeout();
									auto lock = connection->handler_runner->continue_lock();
									if (!lock)
										return;
									if (!ec) {
										std::istream istream(&connection->streambuf);

										std::array<unsigned char, 8> length_bytes;
										istream.read((char*)&length_bytes[0], 8);

										std::size_t length = 0;
										std::size_t num_bytes = 8;
										for (std::size_t c = 0; c < num_bytes; c++)
											length += static_cast<std::size_t>(length_bytes[c]) << (8 * (num_bytes - 1 - c));

										read_message_content(connection, length, endpoint, fin_rsv_opcode);
									}
									else
										connection_error(connection, endpoint, ec);
								}));
							}
							else
								read_message_content(connection, length, endpoint, fin_rsv_opcode);
						}
						else
							connection_error(connection, endpoint, ec);
					}));
				});
			}
			void read_message_content(const std::shared_ptr<Connection>& connection, std::size_t length, Endpoint& endpoint, unsigned char fin_rsv_opcode) const {
				if (length + (connection->fragmented_in_message ? connection->fragmented_in_message->length : 0) > config.max_message_size) {
					connection_error(connection, endpoint, make_error_code::make_error_code(errc::message_size));
					const int status = 1009;
					const std::string reason = Names::MessageBig.data();
					connection->send_close(status, reason);
					connection_close(connection, endpoint, status, reason);
					return;
				}
				connection->set_timeout();
				asio::async_read(*connection->socket, connection->streambuf, asio::transfer_exactly(4 + length), [this, connection, length, &endpoint, fin_rsv_opcode](const error_code& ec, std::size_t) {
					connection->cancel_timeout();
					auto lock = connection->handler_runner->continue_lock();
					if (!lock)
						return;
					if (!ec) {
						std::istream istream(&connection->streambuf);

						std::array<unsigned char, 4> mask;
						istream.read((char*)&mask[0], 4);

						std::shared_ptr<InMessage> in_message;

						if ((fin_rsv_opcode & 0x80) == 0 || (fin_rsv_opcode & 0x0f) == 0) {
							if (!connection->fragmented_in_message) {
								connection->fragmented_in_message = std::shared_ptr<InMessage>(new InMessage(fin_rsv_opcode, length));
								connection->fragmented_in_message->fin_rsv_opcode |= 0x80;
							}
							else
								connection->fragmented_in_message->length += length;
							in_message = connection->fragmented_in_message;
						}
						else
							in_message = std::shared_ptr<InMessage>(new InMessage(fin_rsv_opcode, length));
						std::ostream ostream(&in_message->streambuf);
						for (std::size_t c = 0; c < length; c++)
							ostream.put(istream.get() ^ mask[c % 4]);

						if ((fin_rsv_opcode & 0x0f) == 8) {
							int status = 0;
							if (length >= 2) {
								unsigned char byte1 = in_message->get();
								unsigned char byte2 = in_message->get();
								status = (static_cast<int>(byte1) << 8) + byte2;
							}

							auto reason = in_message->string();
							connection->send_close(status, reason);
							this->connection_close(connection, endpoint, status, reason);
						}
						else if ((fin_rsv_opcode & 0x0f) == 9) {
							auto out_message = std::make_shared<OutMessage>();
							*out_message << in_message->string();
							connection->send(out_message, nullptr, fin_rsv_opcode + 1);

							if (endpoint.on_ping)
								endpoint.on_ping(connection);

							this->read_message(connection, endpoint);
						}
						else if ((fin_rsv_opcode & 0x0f) == 10) {
							if (endpoint.on_pong)
								endpoint.on_pong(connection);

							this->read_message(connection, endpoint);
						}
						else if ((fin_rsv_opcode & 0x80) == 0) {
							this->read_message(connection, endpoint);
						}
						else {
							if (endpoint.on_message)
								endpoint.on_message(connection, in_message);

							connection->fragmented_in_message = nullptr;
							this->read_message(connection, endpoint);
						}
					}
					else
						this->connection_error(connection, endpoint, ec);
				});
			}
			void connection_open(const std::shared_ptr<Connection>& connection, Endpoint& endpoint) const {
				{
					std::unique_lock<std::mutex> lock_(endpoint.lock_connections_);
					endpoint.connections.insert(connection);
				}

				if (endpoint.on_open)
					endpoint.on_open(connection);
			}
			void connection_close(const std::shared_ptr<Connection>& connection, Endpoint& endpoint, int status, const std::string& reason) const {
				{
					std::unique_lock<std::mutex> lock(endpoint.lock_connections_);
					endpoint.connections.erase(connection);
				}

				if (endpoint.on_close)
					endpoint.on_close(connection, status, reason);
			}
			void connection_error(const std::shared_ptr<Connection>& connection, Endpoint& endpoint, const error_code& ec) const {
				{
					std::unique_lock<std::mutex> lock(endpoint.lock_connections_);
					endpoint.connections.erase(connection);
				}

				if (endpoint.on_error && is_run.load())
					endpoint.on_error(connection, ec);
			}
		};

		template <class socket_type>
		class SocketServer : public SocketServerBase<socket_type> {};

		template <>
		class SocketServer<AWS> : public SocketServerBase<AWS> {
		public:
			SocketServer() : SocketServerBase<AWS>(80) {}
			virtual ~SocketServer() = default;

		protected:
			void accept() override {
				std::shared_ptr<Connection> connection(new Connection(handler_runner, config.cnf.timeoutidle, *io_service));

				acceptor->async_accept(*connection->socket, [this, connection](const error_code& ec) {
					auto lock = connection->handler_runner->continue_lock();
					if (!lock)
						return;
					if (ec != error::operation_aborted)
						accept();

					if (!ec) {
						asio::ip::tcp::no_delay option(true);
						connection->socket->set_option(option);
						read_handshake(connection);
					}
				}
				);
			}
		};
	}
}