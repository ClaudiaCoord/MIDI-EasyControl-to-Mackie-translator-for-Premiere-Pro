/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (Remote Controls Plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/
#pragma comment(lib, "iphlpapi.lib")

#include "global.h"
#include "..\..\..\Common\rc\resource_version.h"

namespace Common {
	namespace PLUGINS {

		#include "plugininfo.h"

		using namespace Common::IO;
		using namespace std::placeholders;
		using WSS = WS::SocketServer<WS::AWS>;

		class Names {
		public:
			static constexpr std::string_view ENDPOINT = "^/data/?$"sv;
			static constexpr std::string_view EMPTY =
				"<html><head><title>422 - Not acceptable resource (MIDI-MT)</title></head>"
				"<body><h2>Error 422</h2><h3> - Unprocessable content</h3>"
				"<br/>No access to the requested embedded resource!"
				"<br/>" VER_GUI_EN ", " VER_COPYRIGHT "</body></html>"sv;

			static constexpr std::wstring_view SOURCE = L"PLUGINS.RemotePlugin"sv;
			static constexpr std::wstring_view JSON_ACTION = L"action"sv;
			static constexpr std::wstring_view JSON_CONFIG = L"config"sv;
			static constexpr std::wstring_view JSON_CHANGE = L"change"sv;
			static constexpr std::wstring_view JSON_CHANGED = L"changed"sv;
			static constexpr std::wstring_view JSON_CLOSED = L"closed"sv;
			static constexpr std::wstring_view JSON_UNIT = L"unit"sv;
			static constexpr std::wstring_view JSON_SCENE = L"scene"sv;
			static constexpr std::wstring_view JSON_ID = L"id"sv;
			static constexpr std::wstring_view JSON_VALUE = L"value"sv;
			static constexpr std::wstring_view JSON_TYPE = L"type"sv;
			static constexpr std::wstring_view JSON_TARGET = L"target"sv;
			static constexpr std::wstring_view JSON_LTARGET = L"longtarget"sv;
			static constexpr std::wstring_view JSON_ONOFF = L"onoff"sv;
			static constexpr std::wstring_view JSON_NAME = L"name"sv;
			static constexpr std::wstring_view JSON_LOG = L"log"sv;
			static constexpr std::wstring_view JSON_PATH = L"path"sv;

			static constexpr std::wstring_view JSON_WAKEUP = L"wakeup"sv;
			static constexpr std::wstring_view JSON_WINTOP = L"windowtotop"sv;
			static constexpr std::wstring_view JSON_GETLOG = L"getlog"sv;
			static constexpr std::wstring_view _EXE = L".exe"sv;
		};

		RemotePlugin::RemotePlugin(std::wstring path, HWND hwnd)
			: plugin_ui_(*static_cast<PluginCb*>(this), hwnd),
			  IO::Plugin(
				Utils::to_hash(path), DLG_PLUG_REMOTE_WINDOW,
				Plugin::GuidFromString(_PLUGINGUID),
				path,
				PLUG_HEADER,
				PLUG_DESCRIPTION,
				this,
				IO::PluginClassTypes::ClassRemote,
				(IO::PluginCbType::In2Cb | IO::PluginCbType::Out2Cb | IO::PluginCbType::LogCb | PluginCbType::LogsCb | IO::PluginCbType::ConfCb),
				hwnd
			  ), wse_(ws_.endpoint[Names::ENDPOINT.data()]) {
			PluginCb::out2_cb_ = std::bind(static_cast<void(RemotePlugin::*)(MIDI::MidiUnit&, DWORD)>(&RemotePlugin::cb_out_call_), this, _1, _2);
			PluginCb::cnf_cb_ = std::bind(static_cast<void(RemotePlugin::*)(std::shared_ptr<JSON::MMTConfig>&)>(&RemotePlugin::set_config_cb_), this, _1);

			ws_.get_content = [=]() {
				try {
					HGLOBAL glob{ nullptr };

					do {
						HRSRC res{ nullptr };
						HMODULE hi = ::GetCurrentDllHinstance();
						if (!(res  = ::FindResourceW(hi, MAKEINTRESOURCEW(IDR_ROOT_HTML), RT_HTML)) ||
							!(glob = ::LoadResource(hi, res))) break;

						uint32_t sz = ::SizeofResource(hi, res);
						if (sz == 0) break;
						int8_t* ptr = (int8_t*) ::LockResource(glob);
						if (!ptr) break;
						std::string s = std::string(ptr, ptr + sz);

						::FreeResource(glob);
						return s;
					} while (0);
					if (glob) ::FreeResource(glob);

				} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
				return std::string(Names::EMPTY.data());
			};
			wse_.on_message = [=](std::shared_ptr<WSS::Connection> conn, std::shared_ptr<WSS::InMessage> im) {
				try {
					auto s = Utils::to_string(im->string());
					if (s.empty()) return;
					
					Tiny::TinyJson root{};
					if (!root.ReadJson(s)) return;

					auto action = root.Get<std::wstring>(Names::JSON_ACTION.data());
					if (action.empty()) return;
					else if (action._Equal(Names::JSON_CHANGE.data())) {
						Tiny::xobject cdata = root.Get<Tiny::xobject>(Names::JSON_UNIT.data());
						if (cdata.Count()) {
							cdata.Enter();

							MIDI::Mackie::MIDIDATA m{};
							m.data[0] = static_cast<uint8_t>(cdata.Get<uint16_t>(Names::JSON_SCENE.data(), 255U));
							m.data[1] = static_cast<uint8_t>(cdata.Get<uint16_t>(Names::JSON_ID.data(), 255U));
							m.data[2] = static_cast<uint8_t>(cdata.Get<uint16_t>(Names::JSON_VALUE.data(), 255U));
							m.data[3] = static_cast<uint8_t>(cdata.Get<uint16_t>(Names::JSON_TARGET.data(), 255U));

							if (m.empty() || m_.load(std::memory_order_acquire).equals(m)) return;
							m_.store(m, std::memory_order_release);

							switch (static_cast<MIDI::MidiUnitType>(cdata.Get<uint16_t>(Names::JSON_TYPE.data(), 255U))) {
								case MIDI::MidiUnitType::SLIDER: {
									PluginCb::GetCbIn2()(m, 0);
									break;
								}
								case MIDI::MidiUnitType::BTN: {
									m.data[2] = 0U;
									PluginCb::GetCbIn2()(m, 0);
									std::this_thread::sleep_for(std::chrono::milliseconds(25));
									m.data[2] = 127U;
									PluginCb::GetCbIn2()(m, 0);
									break;
								}
								default: return;
							}

							#if defined (_DEBUG)
							::OutputDebugStringW(m.dump().c_str());
							#endif

							if (loglevel_.load() > 3)
								PluginCb::ToLogRef(log_string().to_log_string(Names::SOURCE.data(),
									(log_string()
										<< Utils::to_string(conn->remote_endpoint_address()).c_str()
										<< L" -> " << m.dump().c_str()).str()
								));
						}
						return;
					}
					else if (action._Equal(Names::JSON_CONFIG.data())) {
						auto& mmt = common_config::Get().GetConfig();
						const std::string cnf = get_build_config_(mmt);
						if (cnf.empty()) return;
						conn->send(cnf, [=](const WS::error_code& ec) {
							if (ec && (loglevel_.load() > 0))
								PluginCb::ToLogRef(log_string().to_log_format(Names::SOURCE.data(),
									common_error_code::Get().get_error(common_error_id::err_REMOTE_CLIENT_ERR),
									Utils::random_hash(conn.get()),
									Utils::to_string(conn->remote_endpoint_address()),
									Utils::to_string(ec)
									)
								);
						});
					}
					else if (action._Equal(Names::JSON_WAKEUP.data())) {

						wakepos_.store(!wakepos_.load());
						::mouse_event(MOUSEEVENTF_MOVE, 0, static_cast<DWORD>(wakepos_.load() ? 1 : -1), 0, 0);
						/*::PostMessage(IO::Plugin::mhwnd_.get(), WM_SYSCOMMAND, (WPARAM)SC_MONITORPOWER, (LPARAM)-1); // Not property work in w11 */

						if (loglevel_.load() > 1)
							PluginCb::ToLogRef(log_string().to_log_string(Names::SOURCE.data(),
								(log_string()
									<< Utils::to_string(conn->remote_endpoint_address()).c_str()
									<< L" -> system wake-up!").str()
							));
					}
					else if (action._Equal(Names::JSON_WINTOP.data())) {

						auto name = root.Get<std::wstring>(Names::JSON_NAME.data());
						if (name.empty()) return;
						if (!name.ends_with(Names::_EXE))
							(void) name.append(Names::_EXE);

						UI::WindowToTop top{};
						(void) top.set(name);

						if (loglevel_.load() > 1)
							PluginCb::ToLogRef(log_string().to_log_string(Names::SOURCE.data(),
								(log_string()
									<< Utils::to_string(conn->remote_endpoint_address()).c_str()
									<< L" -> set application '" << name << L"' to Top window.").str()
							));
					}
					else if (action._Equal(Names::JSON_GETLOG.data())) {

						std::wstring plog{};
						{
							std::filesystem::path p = std::filesystem::path(Utils::app_dir()).parent_path();
							if (!p.empty()) {
								p.append(to_log::Get().logname());
								if (std::filesystem::exists(p))
									plog = p.wstring();
							}
						}
						if (plog.empty()) return;

						auto ft = std::async(std::launch::async, [=]() -> std::string {
							try {
								std::wifstream f(plog, std::ios::in | std::ios::binary | std::ios::ate);
								if (f.is_open()) {
									f.imbue(std::locale(""));
									std::streampos size = f.tellg();
									if (size != 0U) {

										#pragma warning( push )
										#pragma warning( disable : 4244 )
										std::wstring wlog(size, 0);
										#pragma warning( pop )

										try {
											f.seekg(0, std::ios::beg);
											f.read(wlog.data(), size);
											f.close();
										} catch (...) {
											Utils::get_exception(std::current_exception(), __FUNCTIONW__);
											return std::string();
										}
										std::size_t sz = ::WideCharToMultiByte(
											CP_UTF8, 0,
											wlog.data(), static_cast<int>(wlog.length()),
											nullptr, 0, nullptr, nullptr);

										std::string slog(sz, 0);
										::WideCharToMultiByte(
											CP_UTF8, 0, wlog.data(), static_cast<int>(wlog.length()),
											slog.data(), static_cast<int>(sz),
											nullptr, nullptr);

										return WS::AKEY::b64_encode(slog);
									}
								}
							} catch (...) {
								Utils::get_exception(std::current_exception(), __FUNCTIONW__);
							}
							return std::string();
						});

						std::string slog = ft.get();
						if (slog.empty()) return;

						Tiny::TinyJson root{};
						root[Names::JSON_ACTION.data()].Set<std::wstring>(Names::JSON_GETLOG.data());
						root[Names::JSON_PATH.data()].Set<std::wstring>(WS::AKEY::path_escaped(plog));
						root[Names::JSON_LOG.data()].Set<std::wstring>(std::wstring(slog.begin(), slog.end()));

						conn->send(Utils::from_string(root.WriteJson()), [=](const WS::error_code& ec) {
							if (ec && (loglevel_.load() > 0))
								PluginCb::ToLogRef(log_string().to_log_format(Names::SOURCE.data(),
									common_error_code::Get().get_error(common_error_id::err_REMOTE_CLIENT_ERR),
									Utils::random_hash(conn.get()),
									Utils::to_string(conn->remote_endpoint_address()),
									Utils::to_string(ec))
								);
						});
					}
				} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			};
			wse_.on_error = [=](std::shared_ptr<WSS::Connection> conn, const WS::error_code& ec) {
				if (loglevel_.load() > 0)
					PluginCb::ToLogRef(log_string().to_log_format(Names::SOURCE.data(),
						common_error_code::Get().get_error(common_error_id::err_REMOTE_CLIENT_ERR),
						Utils::random_hash(conn.get()),
						Utils::to_string(conn->remote_endpoint_address()).c_str(),
						Utils::to_string(ec).c_str()
						)
					);
			};
			wse_.on_open = [=](std::shared_ptr<WSS::Connection> conn) {
				if (loglevel_.load() > 1)
					PluginCb::ToLogRef(log_string().to_log_format(Names::SOURCE.data(),
						common_error_code::Get().get_error(common_error_id::err_REMOTE_CLIENT_NEW),
						Utils::random_hash(conn.get()),
						Utils::to_string(conn->remote_endpoint_address())
						)
					);
			};
			wse_.on_close = [=](std::shared_ptr<WSS::Connection> conn, int status, const std::string& s) {
				if (loglevel_.load() > 1)
					PluginCb::ToLogRef(log_string().to_log_format(Names::SOURCE.data(),
						common_error_code::Get().get_error(common_error_id::err_REMOTE_CLIENT_CLOSE),
						Utils::random_hash(conn.get()),
						Utils::to_string(conn->remote_endpoint_address()).c_str(),
						status, Utils::to_string(s).c_str()
						)
					);
			};
			wse_.on_handshake = [](std::shared_ptr<WSS::Connection>, WS::CaseInsensitiveMultimap&) {
				return WS::StatusCode::information_switching_protocols;
			};
		}
		RemotePlugin::~RemotePlugin() {
			dispose_();
		}
		void RemotePlugin::dispose_() {
			TRACE_CALL();
			try {
				stop_();
				started_.store(false, std::memory_order_release);
			} catch (...) {}
			is_config_ = is_enable_ = is_started_ = false;
		}
		bool RemotePlugin::stop_() {
			try {
				if (ws_.IsRun()) {
					ws_.stop();
					if (wsthread_.joinable()) wsthread_.join();
				} else if (wsthread_.joinable()) {
					ws_.stop();
					wsthread_.join();
				}
				return true;
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}
		bool RemotePlugin::start_() {
			if (is_started_) {
				if (ws_.IsRun()) return true;

				dispose_();

				return false;
			}
			else if (ws_.IsRun()) {
				bool is_config = is_config_,
					is_enable = is_enable_;

				dispose_();

				is_config_ = is_config;
				is_enable_ = is_enable;
			}
			else if (!is_enable_ || !is_config_) return false;

			std::promise<uint16_t> sp{};
			wsthread_ = std::thread([=](std::promise<uint16_t>& sp_) {
				ws_.start([&sp_](uint16_t port) {
					sp_.set_value(port);
				});
			}, std::ref(sp));

			try {
				PluginCb::ToLog(
					(log_string()
						<< L"Remote server listening on port: " << sp.get_future().get()
						).str()
				);
			} catch (...) {}
			return true;
		}
		void RemotePlugin::load_(REMOTE::RemoteConfig<std::wstring>& rc) {
			is_config_ = !rc.empty();
			is_enable_ = (is_config_ && rc.enable);
			if (is_enable_) {
				loglevel_.store(rc.loglevel);
				rc.server_ua = (log_string() << VER_GUI_EN << L" " << _PLUGINBUILD << L", (" << _PLUGINWINV << L")").str();
				ws_.config.cnf.copy(rc);
			}
		}
		const std::string RemotePlugin::get_build_config_(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALL();
			try {
				if (!mmt->empty()) {
					JSON::json_config jc{};
					Tiny::TinyJson root{};
					root[Names::JSON_ACTION.data()].Set<std::wstring>(Names::JSON_CONFIG.data());

					if (jc.WriteUnitConfig(root, mmt.get()))
						return Utils::from_string(root.WriteJson());
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return std::string();
		}

		#pragma region private call cb
		void RemotePlugin::cb_out_call_(MIDI::MidiUnit& m, DWORD t) {
			if (!started_.load(std::memory_order_acquire)) return;
			try {
				if (m.type == MIDI::MidiUnitType::UNITNONE) return;
				auto list = ws_.get_connections();
				if (list.empty()) return;

				Tiny::TinyJson root{};
				Tiny::TinyJson mjson{};
				root[Names::JSON_ACTION.data()].Set<std::wstring>(Names::JSON_CHANGED.data());

				mjson[Names::JSON_TYPE.data()].Set<uint16_t>(static_cast<uint16_t>(m.type));
				mjson[Names::JSON_ID.data()].Set<uint16_t>(static_cast<uint16_t>(m.key));
				mjson[Names::JSON_SCENE.data()].Set<uint16_t>(static_cast<uint16_t>(m.scene));
				mjson[Names::JSON_TARGET.data()].Set<uint16_t>(static_cast<uint16_t>(m.target));
				mjson[Names::JSON_LTARGET.data()].Set<uint16_t>(static_cast<uint16_t>(m.longtarget));
				mjson[Names::JSON_VALUE.data()].Set<uint16_t>(static_cast<uint16_t>(m.value.value));
				mjson[Names::JSON_ONOFF.data()].Set<bool>(m.value.lvalue);
				root[Names::JSON_UNIT.data()].Set(std::move(mjson));

				const std::string changed = Utils::from_string(root.WriteJson());
				for (auto& a : list)
					a->send(changed);
			} catch (...) {}
		}
		void RemotePlugin::set_config_cb_(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALL();
			try {
				load_(mmt->remoteconf);
				if (is_started_) {
					if (!is_enable_) {
						stop();
						return;
					}
					if (!mmt->empty()) {
						const std::string cnf = get_build_config_(mmt);
						if (cnf.empty()) return;
						for (auto& a : ws_.get_connections())
							a->send(cnf);
					}
					#if defined (_DEBUG_CONFIG)
					::OutputDebugStringW(mmt->remoteconf.dump().c_str());
					#endif
				}
			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
		}
		#pragma endregion

		#pragma region public actions
		bool RemotePlugin::load(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALLX(std::to_wstring((uint32_t)!mmt->empty()));
			try {
				load_(mmt->remoteconf);
			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return true;
		}
		bool RemotePlugin::load(std::wstring s) {
			TRACE_CALLX(s);
			try {
				if (s.empty()) return true;

				worker_background::Get().to_async(std::async(std::launch::async, [=](std::wstring p) {
					try {
						REMOTE::RemoteConfig<std::wstring> rc{};
						JSON::json_config jc{};

						is_enable_ = jc.ReadFile(p,
							[&](Tiny::TinyJson& root, std::wstring& path) -> bool {
								jc.ReadRemoteConfig(root, rc);
								return true;
							}
						);
						if (is_enable_) load_(rc);
						if (is_config_ && is_enable_) common_config::Get().GetConfig()->remoteconf.copy(rc);

					} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
				}, s));

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return true;
		}

		bool RemotePlugin::start(std::shared_ptr<JSON::MMTConfig>& mmt) {
			TRACE_CALLX(std::to_wstring((uint32_t)!mmt->remoteconf.empty()));
			try {
				load_(mmt->remoteconf);
				if (!is_config_ || !is_enable_) return false;

				is_started_ = start_();
				started_.store(is_started_, std::memory_order_release);

				PluginCb::ToLogRef(log_string().to_log_string(__FUNCTIONW__,
					common_error_code::Get().get_error(common_error_id::err_REMOTE_START)));

				export_list_.push_back(
					std::make_pair(
						0,
						(log_string() << L"http://" << mmt->remoteconf.host.c_str() << L":" << mmt->remoteconf.port << L"/").str()
					)
				);
				return is_started_;

			} catch (...) { Utils::get_exception(std::current_exception(), __FUNCTIONW__); }
			return false;
		}
		bool RemotePlugin::stop() {
			TRACE_CALL();
			dispose_();

			PluginCb::ToLogRef(log_string().to_log_string(__FUNCTIONW__,
				common_error_code::Get().get_error(common_error_id::err_REMOTE_STOP)));

			export_list_.clear();
			return true;
		}
		void RemotePlugin::release() {
			TRACE_CALL();
			dispose_();
		}

		IO::PluginUi& RemotePlugin::GetPluginUi() {
			return std::ref(*plugin_ui_.GetPluginUi());
		}
		#pragma endregion

		std::vector<std::pair<uint16_t, std::wstring>>& RemotePlugin::GetDeviceList() {
			TRACE_CALL();
			return std::ref(export_list_);
		}
		std::any RemotePlugin::GetDeviceData() {
			std::any a = (int)42;
			return a;
		}
	}
}