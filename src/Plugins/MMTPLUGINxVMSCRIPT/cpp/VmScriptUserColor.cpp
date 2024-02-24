/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ In/Out Event bridge.
	(c) CC 2023-2024, MIT

	MMTPLUGINx* DLL (VM script plugin)

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"

namespace Common {
	namespace SCRIPT {

		#pragma region Color Constant
		constexpr size_t IDX_POSITION{ 3U };

		uint32_t ColorConstant::count_{ 0U };
		color_t  ColorConstant::colors_black_ = {
			0, 0, 0, 255
		};
		colorscale_t ColorConstant::colors_scale_ = {
			color_t { 255, 0,   0,   0   }, /* red */
			color_t { 255, 0,   255, 42  }, /* yellow */
			color_t { 0,   0,   255, 84  }, /* lime */
			color_t { 0,   255, 255, 126 }, /* aqua */
			color_t { 0,   255, 0,   168 }, /* blue */
			color_t { 255, 255, 0,   210 }, /* magenta */
			color_t { 255, 0,   0,   255 }  /* red */
		};

		static inline const bool color_t_equals__(const color_t& c1, const color_t& c2) {
			return (c1.at(0) == c2.at(0)) && (c1.at(1) == c2.at(1)) && (c1.at(2) == c2.at(2));
		}
		static inline uint16_t color_index__(const ColorGroup& t) {
			return static_cast<uint16_t>(t);
		}
		static inline void calc_colors__(UnitDef c[4], color_t& left, color_t& right, double weight = 0.0, double weightneg = 0.0) {
			c[0].SetValue(static_cast<uint8_t>(std::round(left[0] * weightneg + right[0] * weight)));
			c[1].SetValue(static_cast<uint8_t>(std::round(left[1] * weightneg + right[1] * weight)));
			c[2].SetValue(static_cast<uint8_t>(std::round(left[2] * weightneg + right[2] * weight)));
		}
		static inline void calc_lights__(UnitDef& c, double l) {
			if (c.empty()) return;
			c.SetValue(static_cast<uint8_t>(max_(0.0, std::round(c.GetValue() - l)) ));
		}

		uint32_t ColorConstant::GetNextColorGroup() {
			return ++ColorConstant::count_;
		}
		void ColorConstant::CallcColor(UnitDef c[4], const uint8_t pos) {
			size_t	id{ 0 },
					sz{ ColorConstant::colors_scale_.size() - 1 };
			color_t& left{ ColorConstant::colors_scale_.at(0) },
					 right{ ColorConstant::colors_black_ };

			for (size_t i = 0U; i < ColorConstant::colors_scale_.size(); i++) {
				if ((pos >= ColorConstant::colors_scale_.at(i).at(IDX_POSITION)) && (pos > left.at(IDX_POSITION))) {
					left = ColorConstant::colors_scale_.at(i);
					id = i;
				}
			}
			if (sz == id)
				right = ColorConstant::colors_scale_.at(id);
			else if (sz > id)
				right = ColorConstant::colors_scale_.at(id + 1);
			else if (sz < id)
				right = ColorConstant::colors_scale_.at(sz);

			if (color_t_equals__(left, right))
				calc_colors__(c, left, left);
			else {
				double	offset = pos - left.at(IDX_POSITION),
						weight = (offset > 0) ? (offset / (offset + right.at(IDX_POSITION) - pos)) : 0.0,
						weightneg = 1.0 - weight;
				calc_colors__(c, left, right, weight, weightneg);
			}
		}
		void ColorConstant::CallcLights(UnitDef c[4], const uint8_t light) {
			double l = std::floor(double(light / 100.0) * 255.0);
			calc_lights__(c[0], l);
			calc_lights__(c[1], l);
			calc_lights__(c[2], l);
			calc_lights__(c[3], l);
		}
		std::wstring ColorConstant::ColorHelper(const ColorGroup& t) {
			switch (t) {
				using enum ColorGroup;
				case RED: return L"RED";
				case BLUE: return L"BLUE";
				case GREEN: return L"GREEN";
				case WHITE: return L"WHITE";
				default: return L"-";
			}
		}
		#pragma endregion

		#pragma region RGBWColor
		RGBWColor::RGBWColor(const std::vector<uint8_t>& r, const std::vector<uint8_t>& g, const std::vector<uint8_t>& b)
			: group_(ColorConstant::GetNextColorGroup()) {

			set_groups_(r, g, b, std::vector<uint8_t>());
		}
		RGBWColor::RGBWColor(const std::vector<uint8_t>& r, const std::vector<uint8_t>& g, const std::vector<uint8_t>& b, const std::vector<uint8_t>& w)
			: group_(ColorConstant::GetNextColorGroup()) {

			set_groups_(r, g, b, w);
		}

		#pragma region private
		void RGBWColor::set_groups_(const std::vector<uint8_t>& r, const std::vector<uint8_t>& g, const std::vector<uint8_t>& b, const std::vector<uint8_t>& w) {
			colors_[0] = UnitDef(r, 0U);
			colors_[1] = UnitDef(g, 0U);
			colors_[2] = UnitDef(b, 0U);
			colors_[3] = (w.size() > 1) ? UnitDef(w, 0U) : UnitDef();
		}
		void RGBWColor::changed_colors_() {
			try {
				if (!ActionConstant::IsUpdate()) return;

				worker_background::Get().to_async(std::async(std::launch::async, [=](const UnitDef clr[4], const size_t sz) {

					for (size_t i = 0U; i < sz; i++)
						if (!clr[i].empty())
							ActionConstant::UpdateSlider(clr[i].GetScene(), clr[i].GetKey(), clr[i].GetValue());
				}, std::ref(colors_), std::size(colors_)));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		void RGBWColor::ApplyValues() {
			changed_colors_();
		}
		void RGBWColor::SetValues(const uint8_t pos, const uint8_t light) {
			if (empty()) return;

			ColorConstant::CallcColor(colors_, pos);
			SetLight(light);
		}
		void RGBWColor::UpdateValues() {
			try {
				if (empty()) return;

				worker_background::Get().to_async(
					std::async(std::launch::async, [=](UnitDef clr[4], const size_t sz) {
					try {
						uint8_t found{ 0 }, count{ 0 };
						for (auto& c : colors_)
							count += (c.empty() ? 0 : 1U);

						if (!count) return;

						auto& mmt = common_config::Get().GetConfig();
						for (auto& a : mmt->units) {
							if (a.target == MIDI::Mackie::Target::VMSCRIPT) {
								for (size_t i = 0U; i < sz; i++) {
									UnitDef& ud = clr[i];
									if (!ud.empty()) {
										if ((a.scene == ud.GetScene()) && (a.key == ud.GetKey())) {
											ud.SetValue(a.value.value);
											found++;
											break;
										}
									}
								}
								if (found == count) break;
							}
						}
					} catch (...) {}

				}, std::ref(colors_), std::size(colors_)));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		[[maybe_unused]] void RGBWColor::SetColor(const std::vector<uint8_t>& v) {
			if (empty() || v.empty()) return;

			if (v.size())
				colors_[0].SetValue(v[0]);
			if (v.size() > 1)
				colors_[1].SetValue(v[1]);
			if (v.size() > 2)
				colors_[2].SetValue(v[2]);
			if (v.size() > 3)
				colors_[3].SetValue(v[3]);
			ColorConstant::CallcLights(colors_, light_);
			changed_colors_();
		}
		[[maybe_unused]] void RGBWColor::SetColor(const ColorGroup t, const uint8_t c) {
			if (empty()) return;

			colors_[static_cast<uint16_t>(t)].SetValue(c);
			ColorConstant::CallcLights(colors_, light_);
			changed_colors_();
		}

		void RGBWColor::SetColor(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t w) {
			if (empty()) return;

			colors_[0].SetValue(r);
			colors_[1].SetValue(g);
			colors_[2].SetValue(b);
			colors_[3].SetValue(w);
			ColorConstant::CallcLights(colors_, light_);
			changed_colors_();
		}
		void RGBWColor::SetColor(const uint8_t r, const uint8_t g, const uint8_t b) {
			SetColor(r, g, b, 0);
		}
		void RGBWColor::SetColor(const uint8_t pos) {
			if (empty()) return;
			ColorConstant::CallcColor(colors_, pos);
		}

		void RGBWColor::SetLight() {
			if (empty()) return;
			ColorConstant::CallcLights(colors_, light_);
		}
		void RGBWColor::SetLight(const uint8_t light) {
			if (empty()) return;

			uint8_t l_ = (light > 100) ? 100 : light;
			if (light_ == l_) {
				if (light_)	ColorConstant::CallcLights(colors_, light_);
				return;
			}
			light_ = l_;
			if (light_)	ColorConstant::CallcLights(colors_, light_);
		}
		void RGBWColor::SetLight(const uint8_t light, const bool isup) {
			if (empty()) return;
			SetLight(light);
			if (isup) changed_colors_();
		}

		uint8_t RGBWColor::R() const {
			return colors_[color_index__(ColorGroup::RED)].GetValue();
		}
		uint8_t RGBWColor::G() const {
			return colors_[color_index__(ColorGroup::GREEN)].GetValue();
		}
		uint8_t RGBWColor::B() const {
			return colors_[color_index__(ColorGroup::BLUE)].GetValue();
		}
		uint8_t RGBWColor::W() const {
			return colors_[color_index__(ColorGroup::WHITE)].GetValue();
		}

		uint8_t RGBWColor::GetColor(const ColorGroup t) const {
			return colors_[color_index__(t)].GetValue();
		}
		uint8_t RGBWColor::GetLight() const {
			return light_;
		}
		uint32_t RGBWColor::GetGroup() const {
			return group_;
		}

		const bool RGBWColor::empty() const {
			return colors_[0].empty() && colors_[1].empty() && colors_[2].empty();
		}
		std::wstring RGBWColor::dump() const {
			log_string ls{};
			ls << L"RGBW Color: ";
			for (size_t i = 0U; i < std::size(colors_); i++)
				ls << colors_[i].dump() << L", ";
			return ls.str();
		}
		std::string RGBWColor::dump_s() const {
			return Utils::from_string(dump());
		}
		#pragma endregion
	}
}