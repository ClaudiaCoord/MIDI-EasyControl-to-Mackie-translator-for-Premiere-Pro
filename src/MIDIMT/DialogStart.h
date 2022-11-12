/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once
class DialogStart
{
	std::ofstream __log{};
	std::wstringstream __log_string{};
	HWND __hwndDlg = nullptr;
	WCHAR wmsgs[2][MAX_PATH];
	static DialogStart *ctrl;
	static void InfoCb(const std::string s);

	void Dispose();
	void BuildDeviceComboBox(const std::string s);
	void BuildProxyComboBox(const uint32_t n);
	void SetRegistryBool(bool b, const wchar_t* label);
	bool GetRegistryBool(const wchar_t* label);
	void SetRegistryRun();
	void DeleteRegistryRun();
	void SetSliderInfo(HWND hwnd, uint32_t id, uint32_t pos);
	void SetSliderValues(HWND hwnd, uint32_t id, uint32_t idinfo, uint32_t minv, uint32_t maxv, uint32_t pos);
	uint32_t GetSliderValue(HWND hwnd, uint32_t id);

public:
	static const wchar_t* outMackie;
	static const wchar_t* outProxy;

	DialogStart(HINSTANCE hinst);
	~DialogStart();

	void InitDialog(HWND hwndDlg);
	void EndDialog();

	void ChangeOnLog();
	bool IsOnLog();
	
	void ChangeOnAutoRun();
	bool IsOnAutoRun();

	void ChangeOnManualPort();
	void ChangeOnAutoStart();
	void ChangeOnSliders();
	void ChangeOnProxy();

	void ConfigSave();
	void AutoStart();
	void Start();
	void Stop();
	bool IsStart();
	bool IsAutoStart();
	bool IsProxy();
};

