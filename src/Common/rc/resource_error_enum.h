
#ifndef Commonrcresource_error_enumh
#define Commonrcresource_error_enumh 1

/*
 * Part of MIDI-MT build
 * DO NOT EDIT THIS FILE!
 * EDIT ORIGINAL: Common\rc\template\resource_error_id_template.h
 */


#pragma once

namespace Common {

	enum class FLAG_EXPORT common_error_id : int {
		err_BASE = 30000,
		err_OK = 0,
		err_EMPTY = 1,
		err_UNDEFINED = 2,
		err_JSONCONF_WRITE_ACCESS = 3,
		err_JSONCONF_READ_ACCESS = 4,
		err_JSONCONF_EMPTY = 5,
		err_JSONCONF_NO_DEVICE = 6,
		err_JSONCONF_EMPTY_KEYS = 7,
		err_JSONCONF_WRITE_EMPTY = 8,
		err_INIT_COINITIALIZE = 9,
		err_INIT_COCREATE = 10,
		err_INIT_DEFAULT_AE = 11,
		err_INIT_AS_MANAGER2 = 12,
		err_INIT_AS_AUDIOMETER = 13,
		err_MIXER_STARTOK = 14,
		err_MIXER_STARTERROR = 15,
		err_MIXER_STOPOK = 16,
		err_MIXER_STOPERROR = 17,
		err_NO_INIT = 18,
		err_SESSION_LIST = 19,
		err_LISTISNULL = 20,
		err_SESSIONDISCONNECTED = 21,
		err_SESSIONCREATED = 22,
		err_ITEM_SC_AE = 23,
		err_CBVALUE_BAD_SELECT = 24,
		err_SET_CB_WARNING = 25,
		err_CONFIG_FAIL = 26,
		err_CONFIG_LOAD = 27,
		err_CONFIG_LOADA = 28,
		err_CONFIG_EMPTY = 29,
		err_CONFIG_EMPTY_NAME = 30,
		err_CONFIG_NEW = 31,
		err_NOT_IMPLEMENTED = 32,
		err_NOT_CONFIG = 33,
		err_MIDI_NOT_DEVICES = 34,
		err_MIDI_COUNT_DEVICES = 35,
		err_MIDI_OPEN_DEVICE = 36,
		err_MIDI_START_DEVICE = 37,
		err_MIDI_DEVICE_NOT_CONNECT = 38,
		err_MIDI_DEVICE_FOUND = 39,
		err_MIDI_DEVICE_OPEN = 40,
		err_MIDI_DEVICE_CLOSE = 41,
		err_MIDI_DEVICE_DONE = 42,
		err_MIDI_NOT_DEVICE_NAME = 43,
		err_MIDI_NOT_CONFIG_PROXY = 44,
		err_MIDI_DRIVER_VER_OK = 45,
		err_MIDI_DRIVER_VER_ERROR = 46,
		err_MIDI_SEND_BAD_VALUES = 47,
		err_MIDI_NOT_DRIVER = 48,
		err_MIDI_EMPTY_DRIVER = 49,
		err_MIDI_PROXY_PORTS = 50,
		err_MIDI_PROXY_OFF = 51,
		err_MIDI_OUT_PORTS = 52,
		err_MIDI_OUT_OFF = 53,
		err_MMKEY_START = 54,
		err_MMKEY_STOP = 55,
		err_MQTT_EMPTY_CONFIG = 56,
		err_MQTT_CLIENT_ERROR = 57,
		err_MQTT_CLIENT_OK = 58,
		err_MQTT_CONNECTED = 59,
		err_MQTT_NOT_CONNECTED = 60,
		err_MQTT_DISCONNECTED = 61,
		err_MQTT_NOT_CA_CERT = 62,
		err_MQTT_START = 63,
		err_MQTT_STOP = 64,
		err_MQTT_CA_FILTER = 65,
		err_LOG_LEVEL_NONE = 66,
		err_LOG_LEVEL_INFO = 67,
		err_LOG_LEVEL_NOTICE = 68,
		err_LOG_LEVEL_WARNING = 69,
		err_LOG_LEVEL_ERR = 70,
		err_LOG_LEVEL_DEBUG = 71,
		err_PANEL_INDEX = 72,
		err_PANEL_INIT = 73,
		err_PANEL_ENABLE = 74,
		err_PANEL_NOT_UI_THREAD = 75,
		err_PANEL_NOT_ITEM_VALID = 76,
		err_LIGHT_DMX_START = 77,
		err_LIGHT_DMX_NOT_COM = 78,
		err_LIGHT_DMX_NOT_CONFIG = 79,
		err_LIGHT_DMX_STOP = 80,
		err_LIGHT_DMX_NODEV = 81,
		err_LIGHT_DMX_SELDEV = 82,
		err_LIGHT_ARTNET_START = 83,
		err_LIGHT_ARTNET_NOT_IFACE = 84,
		err_LIGHT_ARTNET_NOT_CONFIG = 85,
		err_LIGHT_ARTNET_STOP = 86,
		err_LIGHT_ARTNET_NOINT = 87,
		err_LIGHT_ARTNET_SELINT = 88,
		err_NOT_CLSID_CONVERT = 89,
		err_BRIDGE_NOT_INIT = 90,
		err_BRIDGE_NOT_LOAD = 91,
		err_BRIDGE_OK_INIT_LOAD = 92,
		err_BRIDGE_OK_START = 93,
		err_BRIDGE_NOT_START = 94,
		err_BRIDGE_OK_STOP = 95,
		err_BRIDGE_NOT_STOP = 96,
		err_PLUGIN_ALREADY_LOADED = 97,
		err_PLUGIN_ALREADY_STARTED = 98,
		err_PLUGIN_ALREADY_STOPED = 99,
		err_PLUGIN_DLL_ERROR = 100,
		err_PLUGIN_DLL_NOT_PLUGIN = 101,
		err_PLUGIN_LIST_INFO = 102,
		err_MIDIMT_UPCONFIG = 103,
		err_MIDIMT_NOPLUGINS = 104,
		err_MIDIMT_CONFEDIT = 105,
		err_MIDIMT_CONFFILTER = 106,
		err_MIDIMT_CONFFROMFILE = 107,
		err_MIDIMT_CONFFROMDRV = 108,
		err_MIDIMT_LANG_ACTIVE = 109,
		err_MIDIMT_VDRV_VERSION = 110,
		err_REMOTE_START = 111,
		err_REMOTE_STOP = 112,
		err_REMOTE_CLIENT_NEW = 113,
		err_REMOTE_CLIENT_ERR = 114,
		err_REMOTE_CLIENT_CLOSE = 115,
		err_COPY_LINK = 116,
		err_NET_IFACE_NONE = 117,
		err_NET_IFACE_SELECT = 118,
		err_NONE = 119,

	};
}

#endif

