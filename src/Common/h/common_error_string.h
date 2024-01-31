/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	+ Audio session volume/mute mixer.
	+ MultiMedia Key translator.
	(c) CC 2023-2024, MIT

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
		L"Write access to the configuration file is denied, '{0}'"sv,
		L"Configuration file missing or access denied, '{0}'"sv,
		L"Configuration file is empty, '{0}'"sv,
		L"MIDI device name not set in configuration.."sv,
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
		L"Audio session disconnect, reason='{0}'"sv,
		L"Audio session created, name='{0}'"sv,
		L"Audio session item '{0}' - bad '{1}' registered state: {2}/{3}"sv,
		L"The value being set from the Callback call is an invalid selector."sv,

		/* Common::MIDI */
		L"Setting a callback is only possible after a successful launch of the MIDI bridge."sv,
		L"Configuration fail, file not found, not start.."sv,
		L"Configuration loaded.."sv,
		L"Configuration already loaded.."sv,
		L"Configuration empty.."sv,
		L"Configuration for '{0}' module is empty.."sv,
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
		L"The virtual MIDI driver is not installed. You need to install the 'Virtual Loop MIDI' package, or download the full installer and reinstall MIDI-MT. More details: https://claudiacoord.github.io/MIDI-MT/docs/EN/Dependencies.html"sv,
		L"The installed virtual MIDI driver was not found or empty! You must install the 'Virtual Loop MIDI' package to run the MIDI-MT translator. More details: https://claudiacoord.github.io/MIDI-MT/docs/EN/Dependencies.html"sv,
		L"Upon startup, {0} proxy MIDI ports will be created."sv,
		L"MIDI proxy ports are disabled."sv,
		L"Upon startup, {0} output MIDI ports will be created."sv,
		L"MIDI output ports are disabled."sv,

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
		L"CA certificate file not exists, path='{0}'"sv,
		L"The Smart-Home control service is running."sv,
		L"The Smart-Home control service has stopped."sv,
		L"CA certificate"sv,

		/* Common::MIDIMT log level */
		L"no log is maintained"sv,
		L"information"sv,
		L"notifications"sv,
		L"warnings"sv,
		L"only errors"sv,
		L"all messages"sv,

		/* Common::MIDIMT Audio Mixer Panel */
		L"Audio Mixer Panel - Cannot continue, index entries are incorrect."sv,
		L"Audio mixer panel - unable to continue, initialization error."sv,
		L"The audio mixer panel window does not match the status, it is not possible to continue with the next operation."sv,
		L"The audio mixer panel is not invoked on the UI thread, changes are not possible."sv,
		L"The audio mixer panel encountered an invalid item, aborted panel window creation."sv,

		/* Common::LIGHT::DMX */
		L"The DMX lighting control service is started using COM{0} - {1}."sv,
		L"The DMX lighting control service is not running, the COM{0} device is not connected.."sv,
		L"The DMX lighting control service is not running and the service configuration is empty."sv,
		L"The DMX lights control service service has stopped."sv,
		L"No suitable devices available"sv,
		L"Select DMX device"sv,

		/* Common::LIGHT::ArtNet */
		L"The ArtNet lighting control service is running using the broadcast address {0}:{1} on network {2}."sv,
		L"The ArtNet lights control service is not started, network {0} not found.."sv,
		L"The ArtNet lighting control service is not running and the service configuration is empty."sv,
		L"The ArtNet lights control service service has stopped."sv,
		L"No suitable interfaces available"sv,
		L"Select ARTNET interface"sv,

		/* Common::IO::Plugins */
		L"Plugin initialization error, unable to convert CLSID/GUID from specified string."sv,
		L"Failed to initialize plugins, no compatible plugins were found."sv,
		L"Failed to load plugins, {0} plugins pre-initialized, loaded 0 plugins."sv,
		L"Plugins initialized successfully, pre-initialization {0}, loaded {1} plugins."sv,
		L"Bridge launched successfully, {0} initialized, {1} loaded, running {2} plugins."sv,
		L"Failed to start the bridge, error when starting plugins, {0} initialized, loaded {1} plugins."sv,
		L"Bridge stopped successfully, {0} initialized, {1} loaded, {2} running, stopped {3} plugins."sv,
		L"Failed to stop the bridge, error when starting plugins, {0} initialized, {1} loaded, running {2} plugins."sv,
		L"Plugins already loaded, reset before reloaded."sv,
		L"Plugins already started."sv,
		L"Plugins already stopped."sv,
		L"Failed to open plugin module. Module is missing or damaged, DLL was not loaded and returned null."sv,
		L"The module is not a MIDI-MT plugin and lacks the necessary entry points."sv,
		L"Enable={0},Config={1},Start={2}"sv,

		/* Common::MIDIMT */
		L"Configuration updated: {0} = {1}{2}"sv,
		L"No modules found, startup impossible."sv,
		L"Please edit the configuration before starting."sv,
		L"Configuration file"sv,
		L"Configuration loaded from file: {0}"sv,
		L"Configuration loaded from driver"sv,
		L"The selected {0} language will be active after the window is restarted."sv,
		L"Installed Virtual MIDI driver version: {0}"sv,

		/* Common::Remote */
		L"The remote control service is running."sv,
		L"The remote control service has stopped."sv,
		L"New client {0}/{1} connected."sv,
		L"Client {0}/{1} error, {2}."sv,
		L"Client {0}/{1} closed, status: {2} - {3}."sv,

		/* Common::JOISTICK */
		L"There is no joystick or gamepad driver."sv,
		L"The specified joystick ID is invalid."sv,
		L"The joystick input cannot be written and the required service is not available."sv,
		L"The specified joystick is not connected to the system."sv,
		L"The gamepad/joystick control service is running, active devices: {0}, online: {1}."sv,
		L"The gamepad/joystick control service has stopped."sv,
		L"The control scene has been changed by gamepad or joystick to {0}."sv,
		L"Control element {0}/{1} is selected with a gamepad or joystick."sv,

		/* All other */
		L"Link copied to clipboard."sv,
		L"No network interfaces.."sv,
		L"Select network interface.."sv,

		/* err_NONE */
		L"-"sv
	};

}
