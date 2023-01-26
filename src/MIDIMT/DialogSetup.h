/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2023, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

enum ConfigStatus : int {
	None = 0,
	LoadFile,
	LoadDevice
};

class DialogSetup
{
	HINSTANCE __hinst = nullptr;
	HWND __hwndDlg = nullptr;
	std::shared_ptr<MidiDevice> __dev = nullptr;
	std::vector<ICONDATA*> icons{};
	std::unique_ptr<ListEdit> __lv = nullptr;
	static DialogSetup* ctrl;
	ConfigStatus ConfigDevice = ConfigStatus::None;

	void Clear();
	void Dispose();
	void InitListView();
	static void ErrorCb(const std::string&);
	static void ErrorCb(const std::wstring&);
	static void MonitorCb(uint8_t scene, uint8_t id, uint8_t v, uint32_t t);

public:
	DialogSetup(HINSTANCE);
	~DialogSetup();

	bool ListViewMenu(uint32_t);
	bool ListViewMenu(LPNMHDR lpmh);
	bool ListViewSort(LPNMHDR lpmh);
	bool ListViewEdit(LPNMHDR lpmh);
	bool ListViewClick(LPNMHDR lpmh);
	bool ListViewDbClick(LPNMHDR lpmh);

	bool ButtonSave();
	bool ButtonMonitor();

	void InitDialog(HWND hwndDlg);
	void EndDialog();
};

