/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Viewer/Editor interface.
	(c) CC 2023-2024, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#include "global.h"
#include "ScintillaHelperSettings.h"

namespace Common {
	namespace UI {

		#define BIT_KEY() (1 << (sizeof(SHORT) * 8 - 1))

		void lexer_ptr_deleter::operator()(Scintilla::ILexer5* lexer) {
			try {
				if (lexer) {
					// lexer->Release(); always is null ?
				}
			} catch (...) {}
		}

		ScintillaBox::ScintillaBox() {
		}
		ScintillaBox::~ScintillaBox() {
			dispose_();
		}

		const bool ScintillaBox::IsLexer() const {
			return islexer;
		}
		const bool ScintillaBox::IsAutocompletion() const {
			return isautocompletion;
		}
		const bool ScintillaBox::IsReadonly() {
			return static_cast<bool>(send(SCI_GETREADONLY));
		}
		const bool ScintillaBox::IsModify() const {
			return (aline == eline);
		}

		void ScintillaBox::dispose_() {
			lexer_.reset();
		}
		void ScintillaBox::autoc_close_() {
			if (send(SCI_AUTOCACTIVE)) send(SCI_AUTOCCANCEL);
		}
		void ScintillaBox::build_icons_() {
			try {
				uint32_t i = 0;
				for (auto a : scint_icons)
					(void)send(SCI_REGISTERIMAGE, i++, reinterpret_cast<sptr_t>(a));
			} catch (...) {}
		}
		void ScintillaBox::arrow_start_(sptr_t line) {
			(void)send(SCI_MARKERDELETEALL, SC_MARK_ARROW);
			(void)send(SCI_MARKERADD, line, SC_MARK_ARROW);
		}
		void ScintillaBox::arrow_end_(sptr_t line) {
			(void)send(SCI_MARKERDELETEALL, SC_MARK_ARROWDOWN);
			(void)send(SCI_MARKERADD, line, SC_MARK_ARROWDOWN);
		}
		const bool ScintillaBox::if_readoly(const bool val, bool isup) {
			if (!isup) {
				const bool ro = send(SCI_GETREADONLY);
				if (ro) send(SCI_SETREADONLY, false);
				return ro;
			}
			if (val) return static_cast<bool>(send(SCI_SETREADONLY, true));
			return false;
		}
		void ScintillaBox::sc_event_(uint32_t m, SCNotification* notify) {
			if (!notify) return;
			try {
				switch (m) {
					case SCN_STYLENEEDED: {
						(void)send(SCI_STARTSTYLING, send(SCI_WORDENDPOSITION, notify->position, true));
						(void)send(SCI_SETSTYLING);
						break;
					}
					case SCN_AUTOCSELECTION: {
						break;
					}
					case SCN_CHARADDED: {

						if ((notify->ch < 32) && (notify->ch != 10) && (notify->ch != 13)) {
							(void)send(SCI_DELETEBACK);

						}
						else if (((notify->ch == 10) || (notify->ch == 13)) && ((::GetKeyState(VK_CONTROL) & BIT_KEY()) == 0)) {

							auto ln = send(SCI_LINEFROMPOSITION, send(SCI_GETCURRENTPOS));
							if (!ln) break;
							auto li = send(SCI_GETLINEINDENTATION, ln - 1);
							auto lv = send(SCI_GETFOLDLEVEL, ln - 1);

							if (lv != SC_FOLDLEVELBASE) {
								if (lv & SC_FOLDLEVELHEADERFLAG) li += send(SCI_GETTABWIDTH);
								(void)send(SCI_SETLINEINDENTATION, ln, li);
								(void)send(SCI_GOTOPOS, send(SCI_GETLINEINDENTPOSITION, ln));
							}

						}
						else if (notify->ch == '}') {

							auto ln = send(SCI_LINEFROMPOSITION, send(SCI_GETCURRENTPOS));
							if (!ln) break;
							auto li = send(SCI_GETLINEINDENTATION, ln - 1);
							auto lv = send(SCI_GETFOLDLEVEL, ln - 1);

							if (lv != SC_FOLDLEVELBASE) {
								if ((lv & SC_FOLDLEVELNONE) == 0)
									li -= send(SCI_GETTABWIDTH);
								(void)send(SCI_SETLINEINDENTATION, ln, li);
								(void)send(SCI_GOTOPOS, send(SCI_GETLINEINDENTPOSITION, ln + 1));
							}

						}
						else {
							if (!AutocompletionList.empty() && !IsReadonly()) {

								auto pe = send(SCI_GETCURRENTPOS);
								auto ps = send(SCI_WORDSTARTPOSITION, pe, true);

								if ((pe > 0) || (ps != pe)) {
									auto sz = (pe - ps);
									if (sz >= 3) {
										std::string s = get_text_(ps, pe);
										if (!s.empty()) {
											std::stringstream ss{};
											for (auto& a : AutocompletionList)
												if (a.starts_with(s)) ss << a.c_str() << ";";
											s = ss.str();
											if (!s.empty() && (s.length() > 1)) {
												s.resize(s.length() - 1);
												(void)send(SCI_AUTOCSHOW, sz, reinterpret_cast<sptr_t>(s.c_str()));
											}
											else autoc_close_();
										}
										else autoc_close_();
									}
									else autoc_close_();
								}
								else autoc_close_();
							}
							if (notify->ch == '(') {
								auto pos = send(SCI_GETCURRENTPOS);
								const char c[] = ")\0";
								(void)send(SCI_ADDTEXT, 1, reinterpret_cast<sptr_t>(c));
								(void)send(SCI_GOTOPOS, pos);
							}
						}
						break;
					}
					case SCN_KEY: {
						::OutputDebugStringW(std::to_wstring(notify->modifiers).c_str());
						break;
					}
					case SCN_MODIFIED: {

						if ((notify->modificationType == SC_MOD_NONE) || !notify->linesAdded)
							break;
						else if ((notify->modificationType & SC_MOD_INSERTTEXT) == SC_MOD_INSERTTEXT)
							nextline_(static_cast<int32_t>(notify->linesAdded));
						else if ((notify->modificationType & SC_MOD_DELETETEXT) == SC_MOD_DELETETEXT) {
							int32_t pos = static_cast<int32_t>(-notify->linesAdded);
							nextline_(-pos);
						}

						#if defined(_DEBUG_EVENT)
						std::string s = ScintillaHelper::parse_event(notify);
						::OutputDebugStringA(s.c_str());
						#endif
						break;
					}
					case SCN_MARGINCLICK: {
						startline_(notify->position);
						break;
					}
					case SCN_MARGINRIGHTCLICK: {
						endline_(notify->position);
						break;
					}
				}
			} catch (...) {}
		}
		void ScintillaBox::sc_event(LPNMHDR hdr) {
			if (!hdr) return;
			try {
				sc_event_(hdr->code, reinterpret_cast<SCNotification*>(hdr));
			} catch (...) {}
		}

		std::string ScintillaBox::get_text_(sptr_t start, sptr_t end) {
			std::string s{};
			try {
				if (end && (end > start)) {
					Sci_TextRange tr{};
					try {
						tr.chrg.cpMin = static_cast<uint32_t>(start);
						tr.chrg.cpMax = static_cast<uint32_t>(end);
						tr.lpstrText = new char[tr.chrg.cpMin + tr.chrg.cpMax + 1] {};
						sptr_t x = send(SCI_GETTEXTRANGE, 0, reinterpret_cast<sptr_t>(&tr));
						if (x) s = Utils::trim(std::string(tr.lpstrText));
					} catch (...) {}
					if (tr.lpstrText) delete[] tr.lpstrText;
				}
			} catch (...) {}
			return s;
		}

		#pragma region Init*
		void ScintillaBox::init(HWND h, std::wstring& s) {
			try {
				lexer_select lex = lexer_select::LEX_NONE;
				if (!s.empty()) {
					if (s.ends_with(L".cnf"))  lex = lexer_select::LEX_JSON;
					else if (s.ends_with(L".log"))  lex = lexer_select::LEX_LOG;
					else if (s.ends_with(L".chai")) lex = lexer_select::LEX_SCRIPT;
				}

				init(h, lex);
				if (s.empty()) return;

				load(s);
				if (lex == lexer_select::LEX_NONE || lex == lexer_select::LEX_LOG)
					readonly(true);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void ScintillaBox::init(HWND h, lexer_select l) {
			if (!h) return;
			try {
				reset();

				sct_ = (SciFnDirect)::SendMessageW(h, SCI_GETDIRECTFUNCTION, 0, 0);
				sctdata_ = (sptr_t)::SendMessageW(h, SCI_GETDIRECTPOINTER, 0, 0);
				if (!sct_ || !sctdata_)
					throw common_error(__FUNCTIONW__, L"error create reference to Scintilla object..", 100);

				(void)send(SCI_STYLECLEARALL);
				//
				(void)send(SCI_SETSCROLLWIDTHTRACKING, true);
				(void)send(SCI_SETCARETLINEVISIBLEALWAYS, true);
				(void)send(SCI_SETCODEPAGE, SC_CP_UTF8);
				(void)send(SCI_USEPOPUP, SC_POPUP_NEVER);
				(void)send(SCI_STYLESETEOLFILLED, 0, true);
				(void)send(SCI_SETSELEOLFILLED, true);
				(void)send(SCI_SETMODEVENTMASK, SC_MOD_INSERTTEXT | SC_MOD_DELETETEXT | SC_MOD_CHANGELINESTATE);
				//
				(void)send(SCI_RGBAIMAGESETWIDTH, 12);
				(void)send(SCI_RGBAIMAGESETHEIGHT, 12);
				(void)send(SCI_ASSIGNCMDKEY, SCK_END, SCI_LINEENDWRAP);
				(void)send(SCI_ASSIGNCMDKEY, SCK_END + (SCMOD_SHIFT << 16), SCI_LINEENDWRAPEXTEND);
				(void)send(SCI_ASSIGNCMDKEY, SCK_HOME, SCI_HOMEWRAP);
				(void)send(SCI_ASSIGNCMDKEY, SCK_HOME + (SCMOD_SHIFT << 16), SCI_HOMEWRAPEXTEND);
				//
				(void)send(SCI_SETINDENT, 6);
				(void)send(SCI_SETTABINDENTS, true);
				(void)send(SCI_SETINDENTATIONGUIDES, SC_IV_LOOKFORWARD);
				//
				(void)send(SCI_AUTOCSETMAXWIDTH, 50);
				(void)send(SCI_AUTOCGETORDER, true);
				(void)send(SCI_AUTOCSETIGNORECASE, true);
				(void)send(SCI_AUTOCSETCHOOSESINGLE, false);
				(void)send(SCI_AUTOCSETDROPRESTOFWORD, true);
				(void)send(SCI_AUTOCSETOPTIONS, SC_AUTOCOMPLETE_FIXED_SIZE);
				(void)send(SCI_AUTOCSETSEPARATOR, static_cast<uptr_t>(';'));
				(void)send(SCI_AUTOCSETTYPESEPARATOR, static_cast<uptr_t>('?'));
				(void)send(SCI_AUTOCSETFILLUPS, 0, reinterpret_cast<sptr_t>("\t(["));
				//
				(void)send(SCI_SETMARGINS, 3);
				(void)send(SCI_SETMARGINTYPEN, 0, SC_MARGIN_SYMBOL | SC_MARGIN_COLOUR);
				(void)send(SCI_SETMARGINTYPEN, 1, SC_MARGIN_NUMBER);
				(void)send(SCI_SETMARGINTYPEN, 2, SC_MARGIN_BACK);
				(void)send(SCI_SETMARGINWIDTHN, 0, 12);
				(void)send(SCI_SETMARGINWIDTHN, 1, 47);
				(void)send(SCI_SETMARGINWIDTHN, 2, 10);
				(void)send(SCI_SETMARGINSENSITIVEN, 1, true);
				(void)send(SCI_SETMARGINCURSORN, 1, SC_CURSORARROW);
				(void)send(SCI_SETMARGINCURSORN, 0, SC_CURSORREVERSEARROW);
				//
				(void)send(SCI_MARKERDEFINE, 2, SC_MARK_ARROW);
				(void)send(SCI_MARKERDEFINE, 6, SC_MARK_ARROWDOWN);
				(void)send(SCI_MARKERDEFINE, 24, SC_MARK_ARROWS);
				//
				(void)send(SCI_MARKERADD, 0, SC_MARK_ARROWS);
				//
				buildlexer(l);
				build_icons_();

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void ScintillaBox::end() {
			dispose_();
		}
		void ScintillaBox::reset() {
			sct_ = nullptr;
			sctdata_ = 0;
			aline = sline = eline = 0;
		}
		#pragma endregion

		#pragma region Build*
		void ScintillaBox::buildlexer(lexer_select l) {
			try {
				std::string s{};
				islexer = false;

				switch (l) {
					using enum lexer_select;
					case LEX_SCRIPT: s = "cpp"; break;
					case LEX_JSON: s = "json"; break;
					case LEX_LOG: s = "log"; break;
					default: {
						buildlexer_style_(lexer_select::LEX_NONE);
						return;
					}
				}

				lexer_.reset(::CreateLexer(s.c_str()));
				if (!lexer_) return;
				(void)send(SCI_SETILEXER, 0, reinterpret_cast<sptr_t>(lexer_.get()));
				islexer = true;

				buildlexer_style_(l);

			} catch (...) {}
		}
		void ScintillaBox::buildlexer_style_(lexer_select l) {
			try {
				//
				switch (l) {
					using enum lexer_select;
					case LEX_SCRIPT: {
						for (auto a : lexer_cpp_color)
							send_lexer_settings_(this, a);

						(void)send(SCI_SETPROPERTY, reinterpret_cast<uptr_t>("fold"), reinterpret_cast<sptr_t>("1"));
						(void)send(SCI_SETPROPERTY, reinterpret_cast<uptr_t>("fold.compact"), reinterpret_cast<sptr_t>("0"));
						(void)send(SCI_SETPROPERTY, reinterpret_cast<uptr_t>("fold.comment"), reinterpret_cast<sptr_t>("1"));
						(void)send(SCI_SETPROPERTY, reinterpret_cast<uptr_t>("fold.cpp.comment.explicit"), reinterpret_cast<sptr_t>("0"));
						(void)send(SCI_SETPROPERTY, reinterpret_cast<uptr_t>("fold.preprocessor"), reinterpret_cast<sptr_t>("1"));
						(void)send(SCI_SETPROPERTY, reinterpret_cast<uptr_t>("lexer.cpp.track.preprocessor"), reinterpret_cast<sptr_t>("0"));
						(void)send(SCI_SETWRAPMODE, SC_WRAP_WORD);
						(void)send(SCI_SETEDGEMODE, EDGE_MULTILINE);
						buildlist(scint_cpp_list_keywords, true, 0);
						buildlist(scint_cpp_ident_keywords, false, 1);
						break;
					}
					case LEX_JSON: {
						for (auto a : lexer_json_color)
							send_lexer_settings_(this, a);

						(void)send(SCI_SETPROPERTY, reinterpret_cast<uptr_t>("fold"), reinterpret_cast<sptr_t>("1"));
						(void)send(SCI_SETPROPERTY, reinterpret_cast<uptr_t>("fold.compact"), reinterpret_cast<sptr_t>("1"));
						(void)send(SCI_SETPROPERTY, reinterpret_cast<uptr_t>("lexer.json.escape.sequence"), reinterpret_cast<sptr_t>("1"));
						(void)send(SCI_SETPROPERTY, reinterpret_cast<uptr_t>("lexer.json.allow.comments"), reinterpret_cast<sptr_t>("0"));
						(void)send(SCI_SETWRAPMODE, SC_WRAP_WORD);
						(void)send(SCI_SETEDGEMODE, EDGE_MULTILINE);
						buildlist(scint_json_list_keywords, true, 0);
						buildlist(scint_json_ident_keywords, false, 1);
						break;
					}
					case LEX_LOG: {
						for (auto a : lexer_log_color)
							send_lexer_settings_(this, a);

						(void)send(SCI_SETWRAPMODE, SC_WRAP_NONE);
						(void)send(SCI_SETEDGEMODE, EDGE_LINE);
						(void)send(SCI_SETSCROLLWIDTHTRACKING, true);
						(void)send(SCI_SETSCROLLWIDTH, 1);
						buildlist(scint_log_list_keywords, true, 0);
						buildlist(scint_log_ident_keywords, false, 1);
						break;
					}
					case LEX_NONE: {
						for (auto a : lexer_cpp_color)
							send_lexer_system_settings_(this, a);

						(void)send(SCI_SETWRAPMODE, SC_WRAP_NONE);
						(void)send(SCI_SETEDGEMODE, EDGE_NONE);
						(void)send(SCI_SETSCROLLWIDTHTRACKING, true);
						(void)send(SCI_SETSCROLLWIDTH, 1);
						break;
					}
					default: return;
				}
			} catch (...) {}
		}
		void ScintillaBox::buildlist(const std::string_view& s, bool overwr, uint8_t num) {
			if (s.empty()) return;
			try {
				if (overwr)	AutocompletionList.clear();
				size_t pos = 0, ppos = 0, zpos = 0, maxsize = 0;
				std::stringstream ss{};

				while ((pos = s.find(";", pos)) != std::string::npos) {
					std::string sub(s.substr(ppos, pos - ppos));
					if (sub.empty() || (sub.length() < 2)) {
						ppos = ++pos;
						continue;
					}
					AutocompletionList.emplace_front(sub);
					maxsize = (sub.length() > maxsize) ? sub.length() : maxsize;
					ppos = ++pos;

					if ((zpos = sub.find("(", 0)) != std::string::npos)
						ss << sub.substr(0, zpos) << " ";
					else
						ss << sub << " ";
				}
				(void)send(SCI_AUTOCSETMAXWIDTH, maxsize + 2);
				std::string str = Utils::trim(ss.str());
				if (!str.empty())
					(void)send(SCI_SETKEYWORDS, static_cast<uptr_t>(num), reinterpret_cast<sptr_t>(str.c_str()));
			} catch (...) {}
		}
		#pragma endregion

		#pragma region Line*
		void ScintillaBox::startnewline() {
			sline = eline = send(SCI_GETLINECOUNT) - 1;
			arrow_start_(sline);
			arrow_end_(eline);

			(void)send(SCI_GOTOPOS, send(SCI_POSITIONFROMLINE, eline));
		}
		void ScintillaBox::startline_(sptr_t pos) {
			sline = send(SCI_LINEFROMPOSITION, pos);
			sline = (sline > eline) ? eline : sline;
			arrow_start_(sline);
		}
		void ScintillaBox::endline_(sptr_t pos) {
			eline = send(SCI_LINEFROMPOSITION, pos);
			eline = (eline < sline) ? sline : eline;
			arrow_end_(eline);
		}
		void ScintillaBox::nextline_(int32_t e_offset) {
			#if defined(_DEBUG_EVENT)
			log_string ls{};
			ls << L"* nextline: " << e_offset << "\n";
			::OutputDebugStringW(ls.str().c_str());
			#endif

			if (e_offset < 0) {
				sptr_t l = send(SCI_GETLINECOUNT) - 1;

				if (l <= sline)
					eline = sline = l;
				else if (l <= eline)
					eline = l;
				else if (l == eline) {
					return;
				}
				arrow_start_(sline);
				arrow_end_(eline);

			}
			else {
				if (e_offset > 1) arrow_start_(sline);
				eline = send(SCI_GETLINECOUNT) - 1;
				arrow_end_(eline);
				if (e_offset > 0) (void)send(SCI_COLOURISE, 0, -1);
			}

			if (default_arrow) {
				default_arrow = false;
				(void)send(SCI_MARKERDELETEALL, SC_MARK_ARROWS);
				(void)send(SCI_MARKERADD, 0, SC_MARK_ARROW);
			}
			if (aline) clear_annotation();
		}
		void ScintillaBox::gostart() {
			(void)send(SCI_SETFIRSTVISIBLELINE, sline);
		}
		void ScintillaBox::goend() {
			(void)send(SCI_SETFIRSTVISIBLELINE, eline);
		}
		#pragma endregion

		#pragma region Function control
		void ScintillaBox::load(std::wstring& s, lexer_select l) {
			try {
				if (s.empty()) return;

				clear();

				auto ft = std::async(std::launch::async, [=]() -> std::string {
					std::filesystem::path p(s);
					if (std::filesystem::exists(p)) {
						std::wifstream f(p.wstring(), std::ios::in | std::ios::binary | std::ios::ate);
						if (f.is_open()) {
							f.imbue(std::locale(""));
							std::streampos size = f.tellg();
							if (size != 0U) {

								#pragma warning( push )
								#pragma warning( disable : 4244 )
								std::wstring wtxt(size, 0);
								#pragma warning( pop )

								try {
									f.seekg(0, std::ios::beg);
									f.read(wtxt.data(), size);
									f.close();
								} catch (...) {
									Utils::get_exception(std::current_exception(), __FUNCTIONW__);
								}
								if (wtxt.empty()) return std::string();

								size_t sz = ::WideCharToMultiByte(
									CP_UTF8, 0,
									wtxt.data(), static_cast<int>(wtxt.length()),
									nullptr, 0, nullptr, nullptr);

								std::string otxt(sz, 0);
								::WideCharToMultiByte(
									CP_UTF8, 0, wtxt.data(), static_cast<int>(wtxt.length()),
									otxt.data(), static_cast<int>(sz),
									nullptr, nullptr);

								return otxt;
							}
						}
					}
					return std::string();
				});

				std::string otxt = ft.get();
				if (otxt.empty()) return;

				const char* otext = otxt.data();
				(void)send(SCI_ADDTEXT, otxt.length(), reinterpret_cast<sptr_t>(otext));
				if (l == lexer_select::LEX_LOG)
					(void)send(SCI_GOTOPOS, send(SCI_GETLENGTH));
				(void)send(SCI_DROPSELECTIONN, 0);
				(void)send(SCI_CLEARSELECTIONS);
				(void)send(SCI_SETFOCUS, true);

			} catch (...) {
				Utils::get_exception(std::current_exception(), __FUNCTIONW__);
			}
		}
		void ScintillaBox::zoomin() {
			(void)send(SCI_ZOOMIN);
		}
		void ScintillaBox::zoomout() {
			(void)send(SCI_ZOOMOUT);
		}
		void ScintillaBox::redo() {
			if (send(SCI_CANREDO)) (void)send(SCI_REDO);
		}
		void ScintillaBox::undo() {
			if (send(SCI_CANUNDO)) (void)send(SCI_UNDO);
		}
		void ScintillaBox::clear() {
			const bool ro = if_readoly(false);
			(void)send(SCI_CLEARALL);
			sline = eline = 0;
			arrow_start_(sline);
			arrow_end_(eline);
			(void)if_readoly(ro, true);
		}
		void ScintillaBox::readonly(bool b) {
			send(SCI_SETREADONLY, b);
		}
		#pragma endregion

		#pragma region Text*
		std::string ScintillaBox::copy() {
			return get_text_(
				send(SCI_POSITIONFROMLINE, sline),
				send(SCI_POSITIONFROMLINE, eline)
			);
		}
		std::string ScintillaBox::get() {
			try {
				std::string s = get_text_(
					send(SCI_POSITIONFROMLINE, sline),
					send(SCI_POSITIONFROMLINE, eline)
				);
				if (s.empty()) return s;

				(void)send(SCI_SETSAVEPOINT);
				(void)send(SCI_SCROLLCARET);
				return s;
			} catch (...) {}
			return std::string();
		}
		void ScintillaBox::set(const std::string& s) {
			try {
				const bool ro = if_readoly(false);
				send(SCI_ADDTEXT, s.length(), reinterpret_cast<sptr_t>(s.c_str()));
				(void)if_readoly(ro, true);
			} catch (...) {}
		}
		void ScintillaBox::set(const std::wstring& s) {
			try {
				clear_annotation();
				if (s.empty()) return;

				size_t sz = ::WideCharToMultiByte(
					CP_UTF8, 0,
					s.data(), static_cast<int>(s.length()),
					nullptr, 0, nullptr, nullptr);

				std::string s_(sz, 0);
				(void) ::WideCharToMultiByte(
					CP_UTF8, 0, s.data(), static_cast<int>(s.length()),
					s_.data(), static_cast<int>(sz),
					nullptr, nullptr);

				const bool ro = if_readoly(false);
				(void)send(SCI_ADDTEXT, s_.length(), reinterpret_cast<sptr_t>(s_.c_str()));
				(void)send(SCI_GOTOPOS, send(SCI_GETLENGTH));
				(void)if_readoly(ro, true);

			} catch (...) {}
		}
		void ScintillaBox::append(const std::wstring& s) {
			(void)send(SCI_GOTOPOS, send(SCI_GETLENGTH));
			set(s);
		}
		#pragma endregion

		void ScintillaBox::set_annotation(const std::string& s) {
			try {
				answer_.push_back(Utils::trimRef(s));

				std::stringstream ss{};
				for (auto& a : answer_) ss << "\t" << a.data() << "\r\n";
				std::string str = ss.str();

				aline = send(SCI_GETLINECOUNT);
				aline = (aline > 0) ? (aline - 1) : aline;

				(void)send(SCI_ANNOTATIONSETTEXT, aline, reinterpret_cast<sptr_t>(str.data()));
				(void)send(SCI_ANNOTATIONSETVISIBLE, ANNOTATION_BOXED);

			} catch (...) {}
		}
		void ScintillaBox::clear_annotation() {
			if (!aline) return;
			try {
				answer_.clear();

				(void)send(SCI_ANNOTATIONSETTEXT, aline, 0);
				(void)send(SCI_ANNOTATIONSETVISIBLE, ANNOTATION_HIDDEN);
				(void)send(SCI_ANNOTATIONCLEARALL);
			} catch (...) {}
			aline = 0;
		}

		sptr_t ScintillaBox::send(uint32_t m) {
			return sct_(sctdata_, m, 0, 0);
		}
		sptr_t ScintillaBox::send(uint32_t m, uptr_t w) {
			return sct_(sctdata_, m, w, 0);
		}
		sptr_t ScintillaBox::send(uint32_t m, uptr_t w, sptr_t l) {
			return sct_(sctdata_, m, w, l);
		}
		void   ScintillaBox::send_bg_color(uptr_t w, sptr_t l) {
			(void)sct_(sctdata_, SCI_STYLESETBACK, w, l);
		}
		void   ScintillaBox::send_fore_color(uptr_t w, sptr_t l) {
			(void)sct_(sctdata_, SCI_STYLESETFORE, w, l);
		}
	}
}

