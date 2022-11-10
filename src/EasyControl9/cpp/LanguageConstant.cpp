/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#include "..\pch.h"

const char LogString::LogTagOpen[] = "was opened (system)";
const char LogString::LogTagClose[] = "was closed  (system)";
const char LogString::LogTagDone[] = "was done  (system)";
const char LogString::LogNotConnect[] = "not connected, abort";
const char LogString::LogNotDefined[] = "not defined, abort";
const char LogString::LogNotFound[] = "not found, abort";
const char LogString::LogNotOpen[] = "Cannot open ";
const char LogString::LogOpen[] = "Open ";
const char LogString::LogFound[] = "Found ";
const char LogString::LogErrorOpen[] = "Error open MIDI device ";
const char LogString::LogErrorClose[] = "Error close MIDI device ";
const char LogString::LogErrorFault[] = "Unknown error MIDI device ";
const char LogString::LogAutoStart[] = "auto start enabled, running..";
const char LogString::LogAlreadyRun[] = "already running, abort..";
const char LogString::LogNotRun[] = "not running, abort..";
const char LogString::LogStart[] = "running..";
const char LogString::LogStop[] = "stop..";
const char LogString::LogBadValues[] = "send bad values";

const char LogString::LogVMError1[] = "Driver DLL - path not found";
const char LogString::LogVMError2[] = "Invalid port handle";
const char LogString::LogVMError3[] = "Too many commands";
const char LogString::LogVMError4[] = "Too many sessions";
const char LogString::LogVMError5[] = "Invalid name";
const char LogString::LogVMError6[] = "Module not found";
const char LogString::LogVMError7[] = "Bad arguments";
const char LogString::LogVMError8[] = "Already exists";
const char LogString::LogVMError9[] = "Old win version";
const char LogString::LogVMError10[] = "Revision mismatch";
const char LogString::LogVMError11[] = "Alias exists";

const char JsonConfig::LogTag[] = "MIDI configuration file ";
const char JsonConfig::LogLoad[] = "loaded, active device";
const char JsonConfig::LogSave[] = "saved";
const char JsonConfig::LogLoadError[] = "not loaded";
const char JsonConfig::LogSaveError[] = "saved error";
const char JsonConfig::LogNameEmpty[] = "error, name empty";

