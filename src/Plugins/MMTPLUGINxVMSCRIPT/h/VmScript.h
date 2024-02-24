/*
	MIDI-MT is a high level application driver for USB MIDI control surface.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace SCRIPT {

		using namespace std::string_view_literals;

		class VmScript {
		private:
			std::shared_ptr<chaiscript::ChaiScript> chai_{};
			std::jthread task_{};
			const ScriptEntry& entry_;
			std::atomic<bool> isended_{ false };
			std::function<void(const uint32_t)> pack_cb_ = [](const uint32_t) {};
			std::function<void(const std::wstring&)> print_cb_ = [](const std::wstring&) {};

			void dispose_();
			void exec_();

		public:

			VmScript(
				const ScriptEntry&,
				std::shared_ptr<chaiscript::ChaiScript>,
				std::function<void(const uint32_t)>,
				std::function<void(const std::wstring&)>);

			VmScript(const VmScript&) = default;
			~VmScript();

			void stop();
			const bool start();
			const bool equals(const std::string&) const;
			const bool empty() const;
			const bool isrun() const;
			const bool iscanceled();
			const bool isend() const;

			const uint32_t hash() const;
			const std::string& name() const;
			std::wstring namew() const;
		};
	}
}