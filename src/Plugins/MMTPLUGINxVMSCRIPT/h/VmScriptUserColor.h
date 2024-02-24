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
		typedef std::array<uint8_t, 4> color_t;
		typedef std::array<color_t, 7> colorscale_t;

		enum class ColorGroup : uint16_t {
			RED = 0,
			GREEN,
			BLUE,
			WHITE
		};

		class ColorConstant {
		private:
			static color_t colors_black_;
			static colorscale_t colors_scale_;
			static uint32_t count_;
		public:
			static uint32_t GetNextColorGroup();
			static void CallcColor(UnitDef[4], const uint8_t);
			static void CallcLights(UnitDef[4], const uint8_t);
			static std::wstring ColorHelper(const ColorGroup&);
		};

		class RGBWColor {
		private:
			UnitDef colors_[4];
			uint32_t group_{ 0 };
			uint8_t  light_{ 100 };

			void set_groups_(const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<uint8_t>&);
			void changed_colors_();

		public:

			RGBWColor(const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<uint8_t>&);
			RGBWColor(const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<uint8_t>&);
			RGBWColor(const RGBWColor&) = default;
			~RGBWColor() = default;

			void SetColor(const uint8_t);
			void SetColor(const uint8_t, const uint8_t, const uint8_t, const uint8_t);
			void SetColor(const uint8_t, const uint8_t, const uint8_t);
			[[maybe_unused]] void SetColor(const ColorGroup, const uint8_t);
			[[maybe_unused]] void SetColor(const std::vector<uint8_t>&);

			void SetLight();
			void SetLight(const uint8_t);
			void SetLight(const uint8_t, const bool);

			void SetValues(const uint8_t, const uint8_t);
			void ApplyValues();
			void UpdateValues();

			uint8_t R() const;
			uint8_t G() const;
			uint8_t B() const;
			uint8_t W() const;

			uint8_t GetColor(const ColorGroup) const;
			uint8_t GetLight() const;
			uint32_t GetGroup() const;

			const bool empty() const;
			std::wstring dump() const;
			std::string dump_s() const;
		};
	}
}