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

		class MacroGroup {
		private:
			std::vector<UnitDef> macros_{};

		public:

			MacroGroup();
			MacroGroup(const MacroGroup&) = default;
			~MacroGroup() = default;

			void add(UnitDef);
			void clear();
			void ApplyValues();
			void UpdateValues();

			const bool empty() const;
			std::wstring dump() const;
			std::string dump_s() const;
		};
	}
}