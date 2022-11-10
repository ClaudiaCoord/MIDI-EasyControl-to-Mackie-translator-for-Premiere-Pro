/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once
class DialogMonitor
{
	HWND __hwndDlg = nullptr;
	std::wstringstream __log_string{};
	static DialogMonitor *ctrl;
	void Dispose();
	static void MonitorCb(uint8_t scene, uint8_t id, uint8_t v, uint32_t t);
	static void ErrorCb(const std::string s);

public:
	DialogMonitor();
	~DialogMonitor();

	void InitDialog(HWND hwndDlg);
	void Start();
	void Stop();
	void Clear();
	void EndDialog();

};

