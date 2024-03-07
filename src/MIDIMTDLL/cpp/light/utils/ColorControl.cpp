/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ DMX protocol.
	+ Art-Net protocol.
	(c) CC 2023-2024, MIT

	MIDIMMT DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace LIGHT {
		namespace UTILS {

			using namespace std::string_view_literals;

			#pragma region ColorValues
			class ColorValues {
			public:

				static constexpr std::wstring_view RED = L"RED"sv;
				static constexpr std::wstring_view BLUE = L"BLUE"sv;
				static constexpr std::wstring_view GREEN = L"GREEN"sv;
				static constexpr std::wstring_view WHITE = L"WHITE"sv;
				static constexpr std::wstring_view MAROON = L"MAROON"sv;
				static constexpr std::wstring_view ORANGE = L"ORANGE"sv;
				static constexpr std::wstring_view YELLOW = L"YELLOW"sv;
				static constexpr std::wstring_view OLIVE = L"OLIVE"sv;
				static constexpr std::wstring_view LIME = L"LIME"sv;
				static constexpr std::wstring_view AQUA = L"AQUA"sv;
				static constexpr std::wstring_view TEAL = L"TEAL"sv;
				static constexpr std::wstring_view MAGENTA = L"MAGENTA"sv;
				static constexpr std::wstring_view PURPLE = L"PURPLE"sv;
				static constexpr std::wstring_view OFF = L"OFF"sv;
				static constexpr std::wstring_view ON = L"ON"sv;
				static constexpr std::wstring_view EMPTY = L"-"sv;

				#if defined (COLORS_STRING_HELPER)

				template <ColorControl::ColorGroup T1>
				static constexpr std::wstring_view color = L"unknown color"sv;

				template <>
				static constexpr std::wstring_view color<ColorControl::ColorGroup::RED> = RED;
				template <>
				static constexpr std::wstring_view color<ColorControl::ColorGroup::BLUE> = BLUE;
				template <>
				static constexpr std::wstring_view color<ColorControl::ColorGroup::GREEN> = GREEN;
				template <>
				static constexpr std::wstring_view color<ColorControl::ColorGroup::WHITE> = WHITE;

				template <ColorControl::ColorsGroup T1>
				static constexpr std::wstring_view colors = L"unknown colors"sv;

				template <>
				static constexpr std::wstring_view colors<ColorControl::ColorsGroup::ON> = ON;
				template <>
				static constexpr std::wstring_view colors<ColorControl::ColorsGroup::OFF> = OFF;
				template <>
				static constexpr std::wstring_view colors<ColorControl::ColorsGroup::RED> = RED;
				template <>
				static constexpr std::wstring_view colors<ColorControl::ColorsGroup::YELLOW> = YELLOW;
				template <>
				static constexpr std::wstring_view colors<ColorControl::ColorsGroup::LIME> = LIME;
				template <>
				static constexpr std::wstring_view colors<ColorControl::ColorsGroup::AQUA> = AQUA;
				template <>
				static constexpr std::wstring_view colors<ColorControl::ColorsGroup::BLUE> = BLUE;
				template <>
				static constexpr std::wstring_view colors<ColorControl::ColorsGroup::MAGENTA> = MAGENTA;

				#endif

				constexpr static color_hsv_t color_red = {
					0, 100, 100
				};
				constexpr static color_hsv_t color_maroon = {
					10, 80, 50
				};
				constexpr static color_hsv_t color_orange = {
					16, 100, 100
				};
				constexpr static color_hsv_t color_yellow = {
					45, 90, 100
				};
				constexpr static color_hsv_t color_olive = {
					60, 100, 50
				};
				constexpr static color_hsv_t color_lime = {
					120, 100, 100
				};
				constexpr static color_hsv_t color_green = {
					120, 100, 50
				};
				constexpr static color_hsv_t color_aqua = {
					180, 100, 100
				};
				constexpr static color_hsv_t color_teal = {
					180, 100, 50
				};
				constexpr static color_hsv_t color_blue = {
					240, 100, 100
				};
				constexpr static color_hsv_t color_magenta = {
					300, 100, 100
				};
				constexpr static color_hsv_t color_purple = {
					300, 90, 50
				};
				constexpr static color_hsv_t color_white = {
					0, 0, 100
				};
				constexpr static color_hsv_t color_black = {
					0,   0,   0
				};
				static const color_hsv_t& get_color(const ColorControl::ColorsGroup& t) {
					switch (t) {
						using enum ColorControl::ColorsGroup;
						case ON:		return std::ref(color_white);
						case OFF:		return std::ref(color_black);
						case RED:		return std::ref(color_red);
						case MAROON:	return std::ref(color_maroon);
						case ORANGE:	return std::ref(color_orange);
						case YELLOW: 	return std::ref(color_yellow);
						case OLIVE: 	return std::ref(color_olive);
						case LIME:		return std::ref(color_lime);
						case GREEN:		return std::ref(color_green);
						case AQUA:		return std::ref(color_aqua);
						case TEAL:		return std::ref(color_teal);
						case BLUE:		return std::ref(color_blue);
						case MAGENTA:	return std::ref(color_magenta);
						case PURPLE:	return std::ref(color_purple);
						default:		return std::ref(color_black);
					}
				}
			};
			#pragma endregion

			#pragma region RgbwData
			const uint8_t RgbwData::MaxCoeff() const {
				return static_cast<uint8_t>(*std::max_element(colors.begin(), colors.end()));
			}
			const uint8_t RgbwData::MinCoeff() const {
				return static_cast<uint8_t>(*std::min_element(colors.begin(), colors.end()));
			}
			color_rgbw_t& RgbwData::GetRef() {
				return std::ref(colors);
			}

			uint8_t RgbwData::R() const {
				return colors[0];
			}
			uint8_t RgbwData::G() const {
				return colors[1];
			}
			uint8_t RgbwData::B() const {
				return colors[2];
			}
			uint8_t RgbwData::W() const {
				return colors[3];
			}

			void RgbwData::Set(const color_rgbw_t& r) {
				colors[0] = r[0];
				colors[1] = r[1];
				colors[2] = r[2];
				colors[3] = r[3];
			}
			void RgbwData::Set(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
				colors[0] = r;
				colors[1] = g;
				colors[2] = b;
				colors[3] = w;
			}
			void RgbwData::copy(const color_rgbw_t& c) {
				std::copy(c.begin(), c.end(), colors.begin());
			}

			const bool RgbwData::equals(const RgbwData& c) const {
				return (c.colors.at(0) == colors.at(0)) && (c.colors.at(1) == colors.at(1)) && (c.colors.at(2) == colors.at(2));
			}
			const bool RgbwData::empty() const {
				return !colors[0] && !colors[1] && !colors[2] && !colors[3];
			}
			std::wstring RgbwData::dump() const {
				log_string ls{};
				log_delimeter ld{};
				ls << L"RGBW.DATA -> [ ";
				for (auto& a : colors)
					ls << ld << static_cast<uint16_t>(a);
				ls << L" ]";
				return ls.str();
			}
			#pragma endregion

			#pragma region HsbData

			color_hsv_t& HsbData::GetRef() {
				return std::ref(colors);
			}

			uint16_t HsbData::GetHue() const {
				return colors[0];
			}
			uint8_t  HsbData::GetSaturation() const {
				return static_cast<uint8_t>(colors[1]);
			}
			uint8_t  HsbData::GetBrightness() const {
				return static_cast<uint8_t>(colors[2]);
			}

			void HsbData::Set(const color_hsv_t& h) {
				colors[0] = min_(h[0], MAX_HUE);
				colors[1] = min_(static_cast<uint8_t>(h[1]), MAX_SATURATION);
				colors[2] = min_(static_cast<uint8_t>(h[2]), MAX_BRIGHTNESS);
			}
			void HsbData::Set(uint16_t h, uint8_t s, uint8_t b) {
				colors[0] = min_(h, MAX_HUE);
				colors[1] = min_(s, MAX_SATURATION);
				colors[2] = min_(b, MAX_BRIGHTNESS);
			}
			void HsbData::SetHue(uint16_t h) {
				colors[0] = min_(h, MAX_HUE);
			}
			void HsbData::SetSaturation(uint8_t s) {
				colors[1] = min_(s, MAX_SATURATION);
			}
			void HsbData::SetBrightness(uint8_t b) {
				colors[2] = min_(b, MAX_BRIGHTNESS);
			}

			const bool HsbData::equals(const HsbData& h) const {
				return (colors[0] == h.GetHue()) && (colors[1] == h.GetSaturation()) && (colors[2] == h.GetBrightness());
			}
			const bool HsbData::empty() const {
				return !colors[0] && !colors[1] && !colors[2];
			}

			std::wstring HsbData::dump() const {
				return (log_string()
					<< L"HSB.DATA -> [ "
					<< L"Hue: " << colors[0]
					<< L", Saturation: " << colors[1]
					<< L", Brightness: " << colors[2] << L" ]"
					);
			}
			#pragma endregion

			#pragma region ColorControl

			/* HSB color: 0-360, saturation: 0-100, brightness: 0-100 */
			void ColorControl::set_HSB_color(uint16_t h, uint8_t s, uint8_t b) {
				hsb.Set(h, s, b);
				set_HSB_color();
			}
			void ColorControl::set_HSB_color(const color_hsv_t& h) {
				hsb.Set(h);
				set_HSB_color();
			}
			void ColorControl::set_HSB_color() {

				if (!hsb.GetBrightness())
					rgbw.colors.fill(0U);
				else if (!hsb.GetSaturation())
					rgbw.colors.fill(hsb.GetBrightness());
				else {

					double h_ = double(hsb.GetHue()) * 6.0 / 360.0;
					double s_ = double(hsb.GetSaturation()) * 1.0 / 100.0;
					double b_ = double(hsb.GetBrightness());
					int32_t hc_ = static_cast<int32_t>(std::floor(h_));
					double hr_ = h_ - hc_;

					uint8_t pv = static_cast<uint8_t>((1.f - s_) * b_);
					uint8_t qv = static_cast<uint8_t>((1.f - s_ * hr_) * b_);
					uint8_t tv = static_cast<uint8_t>((1.f - s_ * (1 - hr_)) * b_);
					uint8_t bn = static_cast<uint8_t>((b_ / 100.0) * 255.0);

					switch (hc_) {
						case 0: { // r
							rgbw.colors[0] = bn;
							rgbw.colors[1] = tv;
							rgbw.colors[2] = pv;
							break;
						}
						case 1: { // g
							rgbw.colors[0] = qv;
							rgbw.colors[1] = bn;
							rgbw.colors[2] = pv;
							break;
						}
						case 2: {
							rgbw.colors[0] = pv;
							rgbw.colors[1] = bn;
							rgbw.colors[2] = tv;
							break;
						}
						case 3: { // b
							rgbw.colors[0] = pv;
							rgbw.colors[1] = qv;
							rgbw.colors[2] = bn;
							break;
						}
						case 4: {
							rgbw.colors[0] = tv;
							rgbw.colors[1] = pv;
							rgbw.colors[2] = bn;
							break;
						}
						case 5: { // back to r
							rgbw.colors[0] = bn;
							rgbw.colors[1] = pv;
							rgbw.colors[2] = qv;
							break;
						}
					}
				}
				rgbw.colors[3] = static_cast<uint8_t>((100 - hsb.GetSaturation()) * 2.55);
			}

			void ColorControl::set_RGB_color(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
				rgbw.Set(r, g, b, w);
				set_RGB_color();
			}
			void ColorControl::set_RGB_color(const color_rgbw_t& r) {
				rgbw.Set(r);
				set_RGB_color();
			}
			void ColorControl::set_RGB_color() {

				const uint8_t cmin = rgbw.MinCoeff();
				const uint8_t cmax = rgbw.MaxCoeff();
				const uint8_t coff = (cmax - cmin);

				hsb.SetBrightness(cmax);
				if (!hsb.GetBrightness()) {
					hsb.SetHue(0);
					hsb.SetSaturation(0);
					return;
				}

				hsb.SetSaturation(255 * long(coff) / cmax);
				if (!hsb.GetSaturation()) {
					hsb.SetHue(0);
					return;
				}

				if (cmax == rgbw.R())
					hsb.SetHue(0 + 43 * (rgbw.G() - rgbw.B()) / coff);
				else if (cmax == rgbw.colors[1])
					hsb.SetHue(85 + 43 * (rgbw.B() - rgbw.R()) / coff);
				else
					hsb.SetHue(171 + 43 * (rgbw.R() - rgbw.G()) / coff);
			}

			/* color = ColorsGroup::OFF ... ColorsGroup::MAGENTA */
			const color_hsv_t& ColorControl::get_color(const ColorsGroup& t) {
				return ColorValues::get_color(t);
			}

			/* static enum color helper */
			std::wstring ColorControl::ColorHelper(const ColorGroup& t) {
				switch (t) {
					using enum ColorGroup;
					case RED: return ColorValues::RED.data();
					case BLUE: return ColorValues::BLUE.data();
					case GREEN: return ColorValues::GREEN.data();
					case WHITE: return ColorValues::WHITE.data();
					default: return ColorValues::EMPTY.data();
				}
			}
			std::wstring ColorControl::ColorHelper(const ColorsGroup& t) {
				switch (t) {
					using enum ColorsGroup;
					case OFF:		return ColorValues::OFF.data();
					case ON:		return ColorValues::ON.data();
					case RED:		return ColorValues::RED.data();
					case MAROON:	return ColorValues::MAROON.data();
					case ORANGE:	return ColorValues::ORANGE.data();
					case YELLOW:	return ColorValues::YELLOW.data();
					case OLIVE:		return ColorValues::OLIVE.data();
					case LIME:		return ColorValues::LIME.data();
					case GREEN:		return ColorValues::GREEN.data();
					case BLUE:		return ColorValues::BLUE.data();
					case AQUA:		return ColorValues::AQUA.data();
					case TEAL:		return ColorValues::TEAL.data();
					case MAGENTA:	return ColorValues::MAGENTA.data();
					case PURPLE:	return ColorValues::PURPLE.data();
					default:		return ColorValues::EMPTY.data();
				}
			}

			#pragma endregion
		}
	}
}
