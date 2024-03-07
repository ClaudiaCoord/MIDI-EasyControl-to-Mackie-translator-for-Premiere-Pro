/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol.
	+ Art-Net protocol.
	(c) CC 2023-2024, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace LIGHT {
		namespace UTILS {

			typedef std::array<int8_t, 4>  color_rgbw_corrector_t;
			typedef std::array<uint8_t, 4> color_rgbw_t;
			typedef std::array<uint16_t, 3> color_hsv_t;

			class FLAG_EXPORT RgbwData {
			public:

				color_rgbw_t colors{};

				const uint8_t MaxCoeff() const;
				const uint8_t MinCoeff() const;

				color_rgbw_t& GetRef();
				uint8_t R() const;
				uint8_t G() const;
				uint8_t B() const;
				uint8_t W() const;

				void Set(const color_rgbw_t&);
				void Set(uint8_t, uint8_t, uint8_t, uint8_t);
				void copy(const color_rgbw_t&);

				const bool equals(const RgbwData&) const;
				const bool empty() const;
				std::wstring dump() const;
			};

			class FLAG_EXPORT HsbData {
			public:

				color_hsv_t colors{};

				static constexpr auto MAX_HUE = 360;
				static constexpr auto MAX_SATURATION = 100;
				static constexpr auto MAX_BRIGHTNESS = 100;

				color_hsv_t& GetRef();
				uint16_t GetHue() const;
				uint8_t  GetSaturation() const;
				uint8_t  GetBrightness() const;

				void Set(const color_hsv_t&);
				void Set(uint16_t, uint8_t, uint8_t);
				void SetHue(uint16_t);
				void SetSaturation(uint8_t);
				void SetBrightness(uint8_t);

				const bool equals(const HsbData&) const;
				const bool empty() const;
				std::wstring dump() const;
			};

			class FLAG_EXPORT ColorControl {
			public:

				enum class ColorGroup : uint8_t {
					RED = 0U,
					GREEN,
					BLUE,
					WHITE
				};
				std::wstring operator=(const ColorControl::ColorGroup& t) const {
					return ColorControl::ColorHelper(t);
				}
				/*
				* Append values:
				* - ColorValues::wstring_view
				* - ColorValues::color_hsv_t
				* - ColorValues::get_color()
				* - ColorControl::ColorHelper()
				* - ScriptBootstrap::script_enum_colorsgroups()
				*/
				enum class ColorsGroup : uint16_t {
					OFF = 0U,
					ON,
					RED,
					MAROON,
					ORANGE,
					YELLOW,
					OLIVE,
					LIME,
					GREEN,
					AQUA,
					TEAL,
					BLUE,
					MAGENTA,
					PURPLE
				};
				std::wstring operator=(const ColorControl::ColorsGroup& t) const {
					return ColorControl::ColorHelper(t);
				}

			protected:

				RgbwData rgbw{};
				HsbData  hsb{};

				void set_HSB_color();
				void set_HSB_color(const color_hsv_t&);
				void set_HSB_color(uint16_t, uint8_t, uint8_t);
				void set_RGB_color();
				void set_RGB_color(const color_rgbw_t&);
				void set_RGB_color(uint8_t, uint8_t, uint8_t, uint8_t);

				///

				const color_hsv_t& get_color(const ColorsGroup&);

			public:

				static std::wstring ColorHelper(const ColorGroup&);
				static std::wstring ColorHelper(const ColorsGroup&);

				ColorControl() = default;
				ColorControl(const ColorControl&) = default;
				virtual ~ColorControl() = default;
			};
		}
	}
}
