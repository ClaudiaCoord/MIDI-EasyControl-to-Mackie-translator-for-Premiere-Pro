/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL + MIDIMTBR DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace IO {

		using namespace std::string_view_literals;

		#define TO_BIT(A) Common::IO::PluginCb::to_bit(Common::IO::PluginCbType::A)

		template<class T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
		constexpr T operator|(T lhs, T rhs) {
			return static_cast<T>(
				static_cast<std::underlying_type<T>::type>(lhs) |
				static_cast<std::underlying_type<T>::type>(rhs));
		}

		class FLAG_EXPORT PluginCb {
		protected:
			uint32_t			id_{ 0 };
			PluginCbType		using_{ PluginCbType::None };
			PluginClassTypes	type_{ PluginClassTypes::ClassNone };
			//
			callOut1Cb_t		out1_cb_ = nullptr;
			callOut2Cb_t		out2_cb_ = nullptr;
			callIn1Cb_t			in1_cb_  = nullptr;
			callIn2Cb_t			in2_cb_  = nullptr;
			//
			callToLog_t			log_cb_  = nullptr;
			callToLogS_t        logs_cb_ = nullptr;
			callFromlog_t		logo_cb_ = nullptr;
			callGetPid_t		pid_cb_  = nullptr;
			callConfigCb_t		cnf_cb_  = nullptr;

			void set_cb_default_();
			bool class_filter_(PluginClassTypes);

		public:

			static inline const uint16_t to_bit(PluginCbType t) {
				return static_cast<uint16_t>(t);
			}

			PluginCb();
			virtual ~PluginCb() = default;

			template<class T1>
			PluginCb(T1* clz, PluginClassTypes t, PluginCbType cbt) {
				using_ = cbt;
				type_ = t;
				id_ = Utils::random_hash(clz);
				set_cb_default_();
			}

			const bool empty() const;
			const bool IsCbType(uint16_t) const;
			const bool IsCbType(PluginCbType) const;

			const uint32_t GetId() const;
			const PluginCbType GetUsing() const;
			const PluginClassTypes GetType() const;

			std::wstring dump();

			#pragma region virtuals
			virtual void SetCbOut(PluginCb*);
			virtual void RemoveCbOut(uint32_t);

			virtual void SetCbPid(PluginCb*);
			virtual void RemoveCbPid(uint32_t);

			virtual void SetCbConfig(PluginCb*);
			virtual void RemoveCbConfig(uint32_t);

			virtual void SetCbOutLog(PluginCb*);
			virtual void RemoveCbOutLog(uint32_t);

			virtual callOut1Cb_t GetCbOut1();
			virtual callOut2Cb_t GetCbOut2();

			virtual callIn1Cb_t  GetCbIn1();
			virtual callIn2Cb_t  GetCbIn2();

			virtual callToLog_t    GetCbLog();
			virtual callToLogS_t   GetCbLogS();
			virtual callFromlog_t  GetCbLogOut();
			virtual callGetPid_t   GetCbPid();
			virtual callConfigCb_t GetCbConfig();
			#pragma endregion

			template<class T1>
			void ToLog(T1 arg) {
				if (!arg.empty()) ToLogRef<T1>(std::ref(arg));
			}

			template<class T1>
			void ToLogRef(T1& arg) {
				try {
					if (arg.empty()) return;
					log_string ls{};
					ls << L"[" << PluginHelper::GetTologTypes(type_) << L"]:";

					if constexpr (std::is_same_v<std::wstring, T1>)
						ls << arg;
					else if constexpr (std::is_same_v<log_string, T1>)
						ls << arg.str();

					if (logs_cb_) logs_cb_(ls);
				} catch (...) {}
			}

			#pragma region  subscribe PluginCbSet
			template<class T1>
			void SetCb(T1& clz) {

				constexpr bool has_setcbout = requires(T1 a, PluginCb* p) {
					a.SetCbOut(p);
				};
				constexpr bool has_setcbpid = requires(T1 a, PluginCb * p) {
					a.SetCbPid(p);
				};
				constexpr bool has_setcbconfig = requires(T1 a, PluginCb * p) {
					a.SetCbConfig(p);
				};
				constexpr bool has_getcblogo = requires(T1 a, PluginCb * p) {
					a.SetCbOutLog(p);
				};
				constexpr bool has_getcbin1 = requires(T1 a) {
					{a.GetCbIn1()} ->  std::convertible_to<callIn1Cb_t>;
				};
				constexpr bool has_getcbin2 = requires(T1 a) {
					{a.GetCbIn2()} ->  std::convertible_to<callIn2Cb_t>;
				};
				constexpr bool has_getcblog = requires(T1 a) {
					{a.GetCbLog()} ->  std::convertible_to<callToLog_t>;
				};
				constexpr bool has_getcblogs = requires(T1 a) {
					{a.GetCbLogS()} ->  std::convertible_to<callToLogS_t>;
				};

				if ((using_ == PluginCbType::None) ||
					(clz.GetUsing() == PluginCbType::None) ||
					(!class_filter_(clz.GetType()))) return;

				#if defined(_DEBUG)
				::OutputDebugStringW((log_string() << this->dump() << clz.dump()).c_str());
				#endif

				uint16_t key__;

				key__ = TO_BIT(Out1Cb);
				if (IsCbType(key__) && clz.IsCbType(key__) && has_setcbout) {
					clz.SetCbOut(static_cast<PluginCb*>(this));
				} else {
					key__ = TO_BIT(Out2Cb);
					if (IsCbType(key__) && clz.IsCbType(key__) && has_setcbout)
						clz.SetCbOut(static_cast<PluginCb*>(this));
				}
				key__ = TO_BIT(In1Cb);
				if (IsCbType(key__) && clz.IsCbType(key__) && has_getcbin1) {
					in1_cb_ = clz.GetCbIn1();
				}
				key__ = TO_BIT(In2Cb);
				if (IsCbType(key__) && clz.IsCbType(key__) && has_getcbin2) {
					in2_cb_ = clz.GetCbIn2();
				}
				key__ = TO_BIT(PidCb);
				if (IsCbType(key__) && clz.IsCbType(key__) && has_setcbpid) {
					clz.SetCbPid(static_cast<PluginCb*>(this));
				}
				if (clz.GetType() == PluginClassTypes::ClassSys) {
					key__ = TO_BIT(LogCb);
					if (IsCbType(key__) && clz.IsCbType(key__) && has_getcblog) {
						log_cb_ = clz.GetCbLog();
					}
					key__ = TO_BIT(LogsCb);
					if (IsCbType(key__) && clz.IsCbType(key__) && has_getcblogs) {
						logs_cb_ = clz.GetCbLogS();
					}
					key__ = TO_BIT(ConfCb);
					if (IsCbType(key__) && clz.IsCbType(key__) && has_setcbconfig) {
						clz.SetCbConfig(static_cast<PluginCb*>(this));
					}
					key__ = TO_BIT(LogoCb);
					if (IsCbType(key__) && clz.IsCbType(key__) && has_getcblogo) {
						clz.SetCbOutLog(static_cast<PluginCb*>(this));
					}
				}
			}
			#pragma endregion

			#pragma region  unsubscribe PluginCbSet
			template<class T1>
			void UnSetCb(T1& clz) {

				constexpr bool has_removecbout = requires(T1 a, uint32_t i) {
					a.RemoveCbOut(i);
				};
				constexpr bool has_removecbpid = requires(T1 a, uint32_t i) {
					a.RemoveCbPid(i);
				};
				constexpr bool has_removecbconfig = requires(T1 a, uint32_t i) {
					a.RemoveCbConfig(i);
				};
				constexpr bool has_removecblog = requires(T1 a, uint32_t i) {
					a.RemoveCbOutLog(i);
				};

				if ((using_ == PluginCbType::None) ||
					(clz.GetUsing() == PluginCbType::None) ||
					(!class_filter_(clz.GetType()))) return;

				uint16_t key__;

				key__ = TO_BIT(Out1Cb);
				if (IsCbType(key__) && clz.IsCbType(key__) && has_removecbout) {
					clz.RemoveCbOut(id_);
				}
				key__ = TO_BIT(Out2Cb);
				if (IsCbType(key__) && clz.IsCbType(key__) && has_removecbout) {
					clz.RemoveCbOut(id_);
				}
				key__ = TO_BIT(In1Cb);
				if (IsCbType(key__) && clz.IsCbType(key__)) {
					in1_cb_ = [](DWORD, DWORD) {};
				}
				key__ = TO_BIT(In2Cb);
				if (IsCbType(key__) && clz.IsCbType(key__)) {
					in2_cb_ = [](Common::MIDI::Mackie::MIDIDATA, DWORD) {};
				}
				key__ = TO_BIT(PidCb);
				if (IsCbType(key__) && clz.IsCbType(key__) && has_removecbpid) {
					clz.RemoveCbPid(id_);
				}
				if (clz.GetType() == PluginClassTypes::ClassSys) {
					key__ = TO_BIT(LogCb);
					if (IsCbType(key__) && clz.IsCbType(key__)) {
						log_cb_ = [](std::wstring&) {};
					}
					key__ = TO_BIT(LogsCb);
					if (IsCbType(key__) && clz.IsCbType(key__)) {
						logs_cb_ = [](log_string&) {};
					}
					key__ = TO_BIT(LogoCb);
					if (IsCbType(key__) && clz.IsCbType(key__) && has_removecblog) {
						clz.RemoveCbOutLog(id_);
					}
					key__ = TO_BIT(ConfCb);
					if (IsCbType(key__) && clz.IsCbType(key__) && has_removecbconfig) {
						clz.RemoveCbConfig(id_);
					}
				}
			}
			#pragma endregion
		};
	}
}

