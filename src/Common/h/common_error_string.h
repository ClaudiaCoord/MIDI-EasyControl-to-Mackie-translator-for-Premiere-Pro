/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023, MIT

	COMMON

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

namespace Common {

	using namespace std::string_view_literals;

	constexpr std::wstring_view _DEFAULT_ERRMSGS[] = {
		L"OK"sv,
		L"<empty>"sv,
		L"This error not defined"sv,

		/* Common::json_config */
		L"Write access to the configuration file is denied, \"{0}\""sv,
		L"Configuration file missing or access denied, \"{0}\""sv,
		L"Configuration file is empty, \"{0}\""sv,
		L"MIDI device name not set in ñonfiguration.."sv,
		L"This configuration does not have configured MIDI keys"sv,
		L"Attention, no MIDI keys are configured in this configuration! the written configuration will not work."sv,

		/* Common::MIXER */
		L"Co*Initialize extended failed"sv,
		L"Co*Create instance failed"sv,
		L"Get default audio endpoint failed"sv,
		L"Audio session manager 2 failed"sv,
		L"Audio meter peak failed"sv,

		L"Mixer audio sessions started, at the time of start, there are {0} running programs available for regulation."sv,
		L"The audio sessions mixer could not be started, error details should be in the log file."sv,
		L"The audio sessions mixer is gracefully stopped."sv,
		L"The audio sessions mixer could not be stopped correctly, the details of the error should be in the log file."sv,

		L"Initialize, before call this.."sv,
		L"Get audio session list failed"sv,
		L"Audio session list is null!"sv,
		L"Audio session disconnect, reason=\"{0}\""sv,
		L"Audio session created, name=\"{0}\""sv,
		L"Audio session item \"{0}\" - bad \"{1}\" registered state: {2}/{3}"sv,
		L"The value being set from the CallBack call is an invalid selector."sv,

		/* Common::MIDI */
		L"Setting a callback is only possible after a successful launch of the MIDI bridge."sv,
		L"Configuration fail, file not found, not start.."sv,
		L"Configuration loaded.."sv,
		L"Configuration already loaded.."sv,
		L"Configuration empty.."sv,
		L"New configuration version detected.."sv,
		L"Unsupported implementation in code, method not overridden: {0}"sv,

		L"Configuration name not specified, abort"sv,
		L"Devices not connected, abort"sv,
		L"Warning, inconsistency in the number of devices in the final list, system/list = [{0}/{1}]"sv,
		L"Cannot open MIDI device, abort, {0}/{1}"sv,
		L"Cannot start MIDI device, abort, {0}/{1}"sv,
		L"Device not connected, abort, {0}/{1}"sv,
		L"Device {0} found, {1}"sv,
		L"Device {0} open, {1}"sv,
		L"Device {0} closed (system)"sv,
		L"Device {0} done (system)"sv,
		L"Not set name to active device, abort, {0}/{1}"sv,
		L"Not proxy ports in configuration, abort"sv,
		L"Driver version [{0}]"sv,
		L"Driver version trouble = [{0}]"sv,
		L"Device {0} send bad values = {1}"sv,

		/* Common::MMKey */
		L"The media key control service is running."sv,
		L"The media key control service has stopped."sv,

		/* Common::MQTT */
		L"MQTT configuration is empty, abort.."sv,
		L"MQTT client creation error, canceled.."sv,
		L"MQTT client successfully created, continue.."sv,
		L"Connected to MQTT server:"sv,
		L"Failed connect to MQTT server:"sv,
		L"Disconnected from MQTT server:"sv,
		L"CA certificate file not exists, path=\"{0}\""sv,
		L"The Smart-Home control service is running."sv,
		L"The Smart-Home control service has stopped."sv,

		/* Common::MIDIMT */
		L"Audio Mixer Panel - Cannot continue, index entries are incorrect."sv,
		L"Audio mixer panel - unable to continue, initialization error."sv,
		L"The audio mixer panel window does not match the status, it is not possible to continue with the next operation."sv,
		L"The audio mixer panel is not invoked on the UI thread, changes are not possible."sv,
		L"The audio mixer panel encountered an invalid item, aborted panel window creation."sv,

		L"-" /* err_NONE */
	};

}
