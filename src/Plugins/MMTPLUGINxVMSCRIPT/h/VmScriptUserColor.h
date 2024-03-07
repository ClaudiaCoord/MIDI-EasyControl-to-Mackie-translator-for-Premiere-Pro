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
		typedef std::array<UnitDef, 4> color_def_t;

		class ColorConstant {
		private:
			static uint32_t count_;
		public:
			static uint32_t GetNextColorGroup();
		};

		class ColorCorrector {
		private:
			LIGHT::UTILS::color_rgbw_corrector_t corrector_{};
			void copy_(const int8_t, const int8_t, const int8_t, const int8_t);

		public:

			ColorCorrector(const LIGHT::UTILS::color_rgbw_corrector_t&);
			ColorCorrector(const int8_t, const int8_t, const int8_t, const int8_t);
			ColorCorrector(const ColorCorrector&) = default;
			ColorCorrector() = default;
			~ColorCorrector() = default;

			int8_t operator[](const int);

			void set(const int8_t, const int8_t, const int8_t, const int8_t);
			LIGHT::UTILS::color_rgbw_corrector_t& get();
			const bool empty() const;
			std::wstring dump() const;
			std::string dump_s() const;
		};

		class RGBWColor : public LIGHT::UTILS::ColorControl {
		private:

			uint32_t group_{ 0 };
			color_def_t units_{};
			std::mutex mlock_{};
			ColorCorrector ccr_{};

			void set_groups_(const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<uint8_t>&);
			void set_to_base_(const LIGHT::UTILS::color_rgbw_t& c);
			void set_color_apply_(const LIGHT::UTILS::ColorControl::ColorsGroup&);
			void set_color_apply_();

			void apply_() const;
			void update_apply_() const;
			void update_apply_async_() const;

		public:

			bool is_update_async{ false };

			RGBWColor(const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<uint8_t>&);
			RGBWColor(const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<uint8_t>&, const std::vector<uint8_t>&);
			RGBWColor(const RGBWColor&);
			~RGBWColor() = default;

			RGBWColor& SetColor(const LIGHT::UTILS::ColorControl::ColorsGroup&);
			RGBWColor& SetColor(const uint8_t, const uint8_t, const uint8_t, const uint8_t);
			RGBWColor& SetColor(const uint16_t, const uint8_t, const uint8_t);

			RGBWColor& SetHue(const uint16_t);
			RGBWColor& SetSaturation(const uint8_t);
			RGBWColor& SetBrightness(const uint8_t);

			RGBWColor& SetColorCorrector(ColorCorrector&);

			void FadeIn(const size_t, const size_t, const size_t);
			void FadeOut(const size_t, const size_t, const size_t);
			void On();
			void Off();

			void UpdateValues();
			void ApplyValues();

			uint8_t R() const;
			uint8_t G() const;
			uint8_t B() const;
			uint8_t W() const;

			uint8_t  GetColor(const LIGHT::UTILS::ColorControl::ColorGroup&) const;
			uint16_t GetHue() const;
			uint8_t  GetSaturation() const;
			uint8_t  GetBrightness() const;
			uint32_t GetGroup() const;

			const bool empty() const;
			std::wstring dump() const;
			std::string dump_s() const;
		};
	}
}