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
		uint32_t ColorConstant::count_{ 0U };
		static inline size_t index_color__(const LIGHT::UTILS::ColorControl::ColorGroup& t) {
			return static_cast<size_t>(std::to_underlying(t));
		}
		uint32_t ColorConstant::GetNextColorGroup() {
			return ++ColorConstant::count_;
		}
		#pragma endregion

		#pragma region ColorCorrector
		ColorCorrector::ColorCorrector(const LIGHT::UTILS::color_rgbw_corrector_t& r) : corrector_(r) {
		}
		ColorCorrector::ColorCorrector(const int8_t r, const int8_t g, const int8_t b, const int8_t w) {
			copy_(r, g, b, w);
		}

		int8_t ColorCorrector::operator[](const int i) {
			switch (i) {
				case 0: return corrector_[0];
				case 1: return corrector_[1];
				case 2: return corrector_[2];
				case 3: return corrector_[3];
				default: return 0;
			}
		}

		void ColorCorrector::copy_(const int8_t r, const int8_t g, const int8_t b, const int8_t w) {
			corrector_[0] = r;
			corrector_[1] = g;
			corrector_[2] = b;
			corrector_[3] = w;
		}
		void ColorCorrector::set(const int8_t r, const int8_t g, const int8_t b, const int8_t w) {
			copy_(r, g, b, w);
		}
		LIGHT::UTILS::color_rgbw_corrector_t& ColorCorrector::get() {
			return std::ref(corrector_);
		}
		const bool ColorCorrector::empty() const {
			return !corrector_[0] && !corrector_[1] && !corrector_[2] && !corrector_[3];
		}
		std::wstring ColorCorrector::dump() const {
			return (log_string()
				<< L"[ r:" << (int16_t)corrector_[0]
				<< L", g:" << (int16_t)corrector_[1]
				<< L", b:" << (int16_t)corrector_[2]
				<< L", w:" << (int16_t)corrector_[3]
				<< L" ]\n");
		}
		std::string  ColorCorrector::dump_s() const {
			return Utils::from_string(dump());
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
		RGBWColor::RGBWColor(const RGBWColor& c) {
			group_ = c.group_;
			units_ = c.units_;
			rgbw = c.rgbw;
			hsb = c.hsb;
		}

		#pragma region private
		#define _CORRECT_COLOR_GET(A,B,C) (A[C]) ? static_cast<uint8_t>(A[C] + ((A[C] / 255) * B[C])) : 0
		#define _CORRECT_COLOR_SET(A,C) (A[C]) ? static_cast<int8_t>(std::floor(A[C] / 100)) : 0

		void RGBWColor::set_to_base_(const LIGHT::UTILS::color_rgbw_t& c) {
			if (ccr_.empty()) {
				units_[0].SetValue(c[0]);
				units_[1].SetValue(c[1]);
				units_[2].SetValue(c[2]);
				units_[3].SetValue(c[3]);
				return;
			}
			units_[0].SetValue(_CORRECT_COLOR_GET(c, ccr_, 0));
			units_[1].SetValue(_CORRECT_COLOR_GET(c, ccr_, 1));
			units_[2].SetValue(_CORRECT_COLOR_GET(c, ccr_, 2));
			units_[2].SetValue(_CORRECT_COLOR_GET(c, ccr_, 3));
		}

		void RGBWColor::set_groups_(const std::vector<uint8_t>& r, const std::vector<uint8_t>& g, const std::vector<uint8_t>& b, const std::vector<uint8_t>& w) {
			units_[0] = (r.size() > 1) ? UnitDef(r, 0U) : UnitDef();
			units_[1] = (g.size() > 1) ? UnitDef(g, 0U) : UnitDef();
			units_[2] = (b.size() > 1) ? UnitDef(b, 0U) : UnitDef();
			units_[3] = (w.size() > 1) ? UnitDef(w, 0U) : UnitDef();
		}
		void RGBWColor::set_color_apply_(const LIGHT::UTILS::ColorControl::ColorsGroup& t) {
			try {
				const LIGHT::UTILS::color_hsv_t& ref = ColorControl::get_color(t);

				std::lock_guard<std::mutex> lock(mlock_);
				ColorControl::set_HSB_color(ref);
				set_to_base_(rgbw.GetRef());
				apply_();

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RGBWColor::set_color_apply_() {
			try {
				std::lock_guard<std::mutex> lock(mlock_);
				ColorControl::set_HSB_color();
				set_to_base_(rgbw.GetRef());
				apply_();

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void RGBWColor::apply_() const {
			if (!is_update_async) [[likely]] update_apply_();
			else [[unlikely]] update_apply_async_();
		}
		void RGBWColor::update_apply_() const {
			try {
				if (!ActionConstant::IsUpdate()) return;
				for (auto& u : units_)
					if (!u.empty())
						ActionConstant::UpdateSlider(u.GetScene(), u.GetKey(), u.GetValue());

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RGBWColor::update_apply_async_() const {
			try {
				if (!ActionConstant::IsUpdate()) return;
				worker_background::Get().to_async(std::async(std::launch::async, [=]() {
					update_apply_();
				}));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		#pragma endregion

		void RGBWColor::ApplyValues() {
			if (empty()) return;
			apply_();
		}
		void RGBWColor::UpdateValues() {
			try {
				if (empty()) return;

				worker_background::Get().to_async(
					std::async(std::launch::async, [=]() {
					try {
						uint8_t count{ 0U };
						for (const auto& c : units_)
							count += (c.empty() ? 0 : 1U);

						if (!count) return;

						auto& mmt = common_config::Get().GetConfig();
						if (mmt->empty()) return;

						std::lock_guard<std::mutex> lock(mlock_);

						for (const auto& a : mmt->units) {
							for (auto& u : units_) {
								if (u.empty()) continue;
								if ((a.scene == u.GetScene()) && (a.key == u.GetKey()) && (a.type == u.GetType())) {
									u.SetValue(a.value.value);
									break;
								}
							}
						}
					} catch (...) {}

				}));

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		RGBWColor& RGBWColor::SetColor(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t w) {
			if (empty()) return reinterpret_cast<RGBWColor&>(*this);
			try {
				std::lock_guard<std::mutex> lock(mlock_);

				ColorControl::set_RGB_color(r, g, b, w);
				set_to_base_(rgbw.GetRef());
				apply_();

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
			return reinterpret_cast<RGBWColor&>(*this);
		}
		RGBWColor& RGBWColor::SetColor(const LIGHT::UTILS::ColorControl::ColorsGroup& t) {
			if (empty()) return reinterpret_cast<RGBWColor&>(*this);
			set_color_apply_(t);
			return reinterpret_cast<RGBWColor&>(*this);
		}
		RGBWColor& RGBWColor::SetColorCorrector(ColorCorrector& c) {
			ccr_.set(
				_CORRECT_COLOR_SET(c, 0),
				_CORRECT_COLOR_SET(c, 1),
				_CORRECT_COLOR_SET(c, 2),
				_CORRECT_COLOR_SET(c, 3)
			);
			return reinterpret_cast<RGBWColor&>(*this);
		}

		/* HSB-HSV */
		RGBWColor& RGBWColor::SetColor(const uint16_t h, const uint8_t s, const uint8_t b) {
			if (!empty()) {
				hsb.Set(h, s, b);
				set_color_apply_();
			}
			return reinterpret_cast<RGBWColor&>(*this);
		}
		RGBWColor& RGBWColor::SetHue(const uint16_t hue) {
			if (!empty()) {
				hsb.SetHue(hue);
				set_color_apply_();
			}
			return reinterpret_cast<RGBWColor&>(*this);
		}
		RGBWColor& RGBWColor::SetSaturation(const uint8_t saturation) {
			if (!empty()) {
				hsb.SetSaturation(saturation);
				set_color_apply_();
			}
			return reinterpret_cast<RGBWColor&>(*this);
		}
		RGBWColor& RGBWColor::SetBrightness(const uint8_t brightness) {
			if (!empty()) {
				hsb.SetBrightness(brightness);
				set_color_apply_();
			}
			return reinterpret_cast<RGBWColor&>(*this);
		}
		
		void RGBWColor::FadeIn(const size_t begin, const size_t end, const size_t wait) {
			try {
				if (empty() || (begin > end)) return;

				int32_t end_ = static_cast<int32_t>(min_(end, LIGHT::UTILS::HsbData::MAX_BRIGHTNESS));
				int32_t n_ = static_cast<int32_t>(min_(begin, LIGHT::UTILS::HsbData::MAX_BRIGHTNESS)),
					    x_ = static_cast<int32_t>(hsb.GetBrightness());
				n_ = (x_ > n_) ? x_ : n_;

				if (n_ >= end_) return;

				std::lock_guard<std::mutex> lock(mlock_);
				for (; n_ <= end_; n_++) {
					hsb.SetBrightness(static_cast<uint8_t>(n_));
					ColorControl::set_HSB_color();
					set_to_base_(rgbw.GetRef());
					apply_();
					std::this_thread::sleep_for(std::chrono::milliseconds(wait));
				}

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void RGBWColor::FadeOut(const size_t begin, const size_t end, const size_t wait) {
			try {
				if (empty() || (begin < end)) return;

				int32_t end_ = static_cast<int32_t>(min_(end, LIGHT::UTILS::HsbData::MAX_BRIGHTNESS));
				int32_t x_ = static_cast<int32_t>(hsb.GetBrightness());
				int32_t n_ = (x_) ? x_ : static_cast<int32_t>(min_(begin, LIGHT::UTILS::HsbData::MAX_BRIGHTNESS));

				if (n_ <= end_) return;

				std::lock_guard<std::mutex> lock(mlock_);
				for (; n_ >= end_; n_--) {

					hsb.SetBrightness(static_cast<uint8_t>(n_));
					ColorControl::set_HSB_color();
					set_to_base_(rgbw.GetRef());
					apply_();
					std::this_thread::sleep_for(std::chrono::milliseconds(wait));
				}

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}

		void RGBWColor::On() {
			if (empty()) return;
			set_color_apply_(LIGHT::UTILS::ColorControl::ColorsGroup::ON);
		}
		void RGBWColor::Off() {
			if (empty()) return;
			set_color_apply_(LIGHT::UTILS::ColorControl::ColorsGroup::OFF);
		}

		uint8_t RGBWColor::R() const {
			return rgbw.colors[index_color__(ColorGroup::RED)];
		}
		uint8_t RGBWColor::G() const {
			return rgbw.colors[index_color__(ColorGroup::GREEN)];
		}
		uint8_t RGBWColor::B() const {
			return rgbw.colors[index_color__(ColorGroup::BLUE)];
		}
		uint8_t RGBWColor::W() const {
			return rgbw.colors[index_color__(ColorGroup::WHITE)];
		}

		uint8_t  RGBWColor::GetColor(const LIGHT::UTILS::ColorControl::ColorGroup& t) const {
			return units_[index_color__(t)].GetValue();
		}
		uint16_t RGBWColor::GetHue() const {
			return hsb.GetHue();
		}
		uint8_t  RGBWColor::GetSaturation() const {
			return hsb.GetSaturation();
		}
		uint8_t  RGBWColor::GetBrightness() const {
			return hsb.GetBrightness();
		}
		uint32_t RGBWColor::GetGroup() const {
			return group_;
		}

		const bool   RGBWColor::empty() const {
			return units_[0].empty() && units_[1].empty() && units_[2].empty();
		}
		std::wstring RGBWColor::dump() const {
			log_string ls{};
			log_delimeter ld{};
			ls << L"RGBWColor ->\n"
				<< L"\t" << hsb.dump() << L",\n"
				<< L"\t" << rgbw.dump() << L",\n"
				<< L"\tColor current: [ ";

			for (auto& a : units_)
				ls << ld << a.dump();

			ls << L" ]\n";
			if (!ccr_.empty())
				ls << L"\tColor corrector: " << ccr_.dump() << L"\n";
			return ls.str();
		}
		std::string  RGBWColor::dump_s() const {
			return Utils::from_string(dump());
		}
		#pragma endregion
	}
}