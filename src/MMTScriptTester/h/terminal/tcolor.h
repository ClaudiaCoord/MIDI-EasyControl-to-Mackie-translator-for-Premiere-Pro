
#pragma once

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#include <windows.h>
#include <io.h>
#include <memory>
#include <algorithm>
#include <atomic>
#include <string>
#include <cstdlib>
#include <cstring>
#include <iostream>

namespace tcolor {

	enum class style {
		reset = 0,
		bold = 1,
		dim = 2,
		italic = 3,
		underline = 4,
		blink = 5,
		rblink = 6,
		reversed = 7,
		conceal = 8,
		crossed = 9
	};
	enum class fg {
		black = 30,
		red = 31,
		green = 32,
		yellow = 33,
		blue = 34,
		magenta = 35,
		cyan = 36,
		gray = 37,
		reset = 39
	};
	enum class bg {
		black = 40,
		red = 41,
		green = 42,
		yellow = 43,
		blue = 44,
		magenta = 45,
		cyan = 46,
		gray = 47,
		reset = 49
	};
	enum class fgB {
		black = 90,
		red = 91,
		green = 92,
		yellow = 93,
		blue = 94,
		magenta = 95,
		cyan = 96,
		gray = 97
	};
	enum class bgB {
		black = 100,
		red = 101,
		green = 102,
		yellow = 103,
		blue = 104,
		magenta = 105,
		cyan = 106,
		gray = 107
	};
	enum class control {
		Off = 0,
		Auto = 1,
		Force = 2
	};
	enum class winTerm {
		Auto = 0,
		Ansi = 1,
		Native = 2
	};

	namespace tcolor_implementation {

		inline std::atomic<control>& controlMode() noexcept {
			static std::atomic<control> value(control::Auto);
			return value;
		}

		inline std::atomic<winTerm>& winTermMode() noexcept {
			static std::atomic<winTerm> termMode(winTerm::Auto);
			return termMode;
		}

		inline bool isTerminal(const std::wstreambuf* osbuf) noexcept {
			if (osbuf == std::wcout.rdbuf()) {
				static const bool cout_term = _isatty(_fileno(stdout));
				return cout_term;
			}
			else if (osbuf == std::wcerr.rdbuf() || osbuf == std::wclog.rdbuf()) {
				static const bool cerr_term = _isatty(_fileno(stderr));
				return cerr_term;
			}
			return false;
		}

		template <typename T>
		using enableStd = typename std::enable_if<
			std::is_same<T, tcolor::style>::value || std::is_same<T, tcolor::fg>::value
			|| std::is_same<T, tcolor::bg>::value || std::is_same<T, tcolor::fgB>::value
			|| std::is_same<T, tcolor::bgB>::value,
			std::wostream&>::type;

		struct SGR {
			BYTE fgColor;
			BYTE bgColor;
			BYTE bold;
			BYTE underline;
			BOOLEAN inverse;
			BOOLEAN conceal;
		};

		enum class AttrColor : BYTE {
			black = 0,
			red = 4,
			green = 2,
			yellow = 6,
			blue = 1,
			magenta = 5,
			cyan = 3,
			gray = 7
		};

		inline HANDLE getConsoleHandle(const std::wstreambuf* osbuf) noexcept
		{
			if (osbuf == std::wcout.rdbuf()) {
				static const HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
				return hStdout;
			}
			else if (osbuf == std::wcerr.rdbuf() || osbuf == std::wclog.rdbuf()) {
				static const HANDLE hStderr = GetStdHandle(STD_ERROR_HANDLE);
				return hStderr;
			}
			return INVALID_HANDLE_VALUE;
		}

		inline bool setWinTermAnsiColors(const std::wstreambuf* osbuf) noexcept
		{
			HANDLE h = getConsoleHandle(osbuf);
			if (h == INVALID_HANDLE_VALUE) {
				return false;
			}
			DWORD dwMode = 0;
			if (!GetConsoleMode(h, &dwMode)) {
				return false;
			}
			dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			if (!SetConsoleMode(h, dwMode)) {
				return false;
			}
			return true;
		}

		inline bool supportsAnsi(const std::wstreambuf* osbuf) noexcept {
			if (osbuf == std::wcout.rdbuf()) {
				static const bool cout_ansi = setWinTermAnsiColors(osbuf);
				return cout_ansi;
			}
			else if (osbuf == std::wcerr.rdbuf() || osbuf == std::wclog.rdbuf()) {
				static const bool cerr_ansi = setWinTermAnsiColors(osbuf);
				return cerr_ansi;
			}
			return false;
		}

		inline const SGR& defaultState() noexcept
		{
			static const SGR defaultSgr = []() -> SGR {
				CONSOLE_SCREEN_BUFFER_INFO info;
				WORD attrib = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
				if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),
					&info)
					|| GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE),
						&info)) {
					attrib = info.wAttributes;
				}
				SGR sgr = { 0, 0, 0, 0, FALSE, FALSE };
				sgr.fgColor = attrib & 0x0F;
				sgr.bgColor = (attrib & 0xF0) >> 4;
				return sgr;
				}();
				return defaultSgr;
		}

		inline BYTE ansi2attr(BYTE rgb) noexcept
		{
			static const AttrColor rev[8]
				= { AttrColor::black,  AttrColor::red,  AttrColor::green,
					AttrColor::yellow, AttrColor::blue, AttrColor::magenta,
					AttrColor::cyan,   AttrColor::gray };
			return static_cast<BYTE>(rev[rgb]);
		}

		inline void setWinSGR(tcolor::bg col, SGR& state) noexcept
		{
			if (col != tcolor::bg::reset) {
				state.bgColor = ansi2attr(static_cast<BYTE>(col) - 40);
			}
			else {
				state.bgColor = defaultState().bgColor;
			}
		}

		inline void setWinSGR(tcolor::fg col, SGR& state) noexcept
		{
			if (col != tcolor::fg::reset) {
				state.fgColor = ansi2attr(static_cast<BYTE>(col) - 30);
			}
			else {
				state.fgColor = defaultState().fgColor;
			}
		}

		inline void setWinSGR(tcolor::bgB col, SGR& state) noexcept
		{
			state.bgColor = (BACKGROUND_INTENSITY >> 4)
				| ansi2attr(static_cast<BYTE>(col) - 100);
		}

		inline void setWinSGR(tcolor::fgB col, SGR& state) noexcept {
			state.fgColor
				= FOREGROUND_INTENSITY | ansi2attr(static_cast<BYTE>(col) - 90);
		}

		inline void setWinSGR(tcolor::style style, SGR& state) noexcept {
			switch (style) {
				using enum tcolor::style;
				case reset: state = defaultState(); break;
				case bold: state.bold = FOREGROUND_INTENSITY; break;
				case underline:
				case blink: {
					state.underline = BACKGROUND_INTENSITY;
					break;
				}
				case reversed: state.inverse = TRUE; break;
				case conceal: state.conceal = TRUE; break;
				default: break;
			}
		}

		inline SGR& current_state() noexcept
		{
			static SGR state = defaultState();
			return state;
		}

		inline WORD SGR2Attr(const SGR& state) noexcept
		{
			WORD attrib = 0;
			if (state.conceal) {
				if (state.inverse) {
					attrib = (state.fgColor << 4) | state.fgColor;
					if (state.bold)
						attrib |= FOREGROUND_INTENSITY | BACKGROUND_INTENSITY;
				}
				else {
					attrib = (state.bgColor << 4) | state.bgColor;
					if (state.underline)
						attrib |= FOREGROUND_INTENSITY | BACKGROUND_INTENSITY;
				}
			}
			else if (state.inverse) {
				attrib = (state.fgColor << 4) | state.bgColor;
				if (state.bold) attrib |= BACKGROUND_INTENSITY;
				if (state.underline) attrib |= FOREGROUND_INTENSITY;
			}
			else {
				attrib = state.fgColor | (state.bgColor << 4) | state.bold
					| state.underline;
			}
			return attrib;
		}

		template <typename T>
		inline void setWinColorAnsi(std::wostream& os, T const value)
		{
			os << L"\033[" << static_cast<int>(value) << L"m";
		}

		template <typename T>
		inline void setWinColorNative(std::wostream& os, T const value)
		{
			const HANDLE h = getConsoleHandle(os.rdbuf());
			if (h != INVALID_HANDLE_VALUE) {
				setWinSGR(value, current_state());
				os.flush();
				SetConsoleTextAttribute(h, SGR2Attr(current_state()));
			}
		}

		template <typename T>
		inline enableStd<T> setColor(std::wostream& os, T const value)
		{
			if (winTermMode() == winTerm::Auto) {
				if (supportsAnsi(os.rdbuf())) {
					setWinColorAnsi(os, value);
				}
				else {
					setWinColorNative(os, value);
				}
			}
			else if (winTermMode() == winTerm::Ansi) {
				setWinColorAnsi(os, value);
			}
			else {
				setWinColorNative(os, value);
			}
			return os;
		}
	}

	template <typename T>
	inline tcolor_implementation::enableStd<T> operator<<(std::wostream& os,
		const T value)
	{
		const control option = tcolor_implementation::controlMode();
		switch (option) {
		case control::Auto:
			return tcolor_implementation::isTerminal(os.rdbuf())
				? tcolor_implementation::setColor(os, value)
				: os;
		case control::Force: return tcolor_implementation::setColor(os, value);
		default: return os;
		}
	}

	inline void setWinTermMode(const tcolor::winTerm value) noexcept {
		tcolor_implementation::winTermMode() = value;
	}

	inline void setControlMode(const control value) noexcept {
		tcolor_implementation::controlMode() = value;
	}

}

