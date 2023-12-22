/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Viewer/Editor interface.
	(c) CC 2023, MIT

	MIDIMTVEUI DLL

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {
	namespace UI {

		struct lexer_ptr_deleter {
			void operator()(Scintilla::ILexer5*);
		};

		enum class lexer_select : int {
			LEX_NONE = 0,
			LEX_SCRIPT,
			LEX_JSON,
			LEX_LOG
		};

		class FLAG_EXPORT ScintillaBox {
		private:
			handle_ptr<Scintilla::ILexer5*, lexer_ptr_deleter> lexer_{};
			SciFnDirect sct_{ nullptr };
			sptr_t sctdata_{ 0 };
			std::vector<std::string> answer_{};

			bool default_arrow{ true };
			bool isautocompletion{ false };
			bool islexer{ false };
			sptr_t aline = 0, sline = 0, eline = 0;

			std::string get_text_(sptr_t, sptr_t);
			const bool  if_readoly(const bool, bool = false);
			void buildlexer_style_(lexer_select);
			void autoc_close_();
			void build_icons_();
			void arrow_start_(sptr_t);
			void arrow_end_(sptr_t);

			void startline_(sptr_t);
			void endline_(sptr_t);
			void nextline_(int32_t = 0);
			void sc_event_(uint32_t, SCNotification*);

			void dispose_();

		public:

			const bool IsLexer() const;
			const bool IsAutocompletion() const;
			const bool IsReadonly();
			const bool IsModify() const;

			std::forward_list<std::string> AutocompletionList{};

			ScintillaBox();
			~ScintillaBox();

			void init(HWND, lexer_select);
			void init(HWND, std::wstring&);
			void end();
			void reset();
			void load(std::wstring&, lexer_select = lexer_select::LEX_NONE);

			void buildlexer(lexer_select);
			void buildlist(const std::string_view&, bool = true, uint8_t = 0U);
			void startnewline();

			void zoomin();
			void zoomout();
			void gostart();
			void goend();
			void redo();
			void undo();
			void clear();
			void readonly(bool);
			std::string copy();

			std::string get();
			void set(const std::string&);
			void set(const std::wstring&);
			void append(const std::wstring&);

			void set_annotation(const std::string&);
			void clear_annotation();
			
			void sc_event(LPNMHDR);

			sptr_t send(uint32_t);
			sptr_t send(uint32_t, uptr_t);
			sptr_t send(uint32_t, uptr_t, sptr_t);
			void send_bg_color(uptr_t, sptr_t);
			void send_fore_color(uptr_t, sptr_t);
		};
	}
}


