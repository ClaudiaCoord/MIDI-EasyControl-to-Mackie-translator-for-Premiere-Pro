
#define IDS_ERRORID_BASE	55000

D(IDS_ERRORID_BASE,			IDS_ERRORID_BASE, 0);
D(IDS_ERRORID_OK,			IDS_ERRORID_BASE, 0);
D(IDS_ERRORID_EMPTY,			IDS_ERRORID_BASE, 1);
D(IDS_ERRORID_UNDEFINED,		IDS_ERRORID_BASE, 2);

/* Common::json_config */
D(IDS_ERRORID_JSONCONF_WRITE_ACCESS,	IDS_ERRORID_BASE, 3);
D(IDS_ERRORID_JSONCONF_READ_ACCESS,	IDS_ERRORID_BASE, 4);
D(IDS_ERRORID_JSONCONF_EMPTY,		IDS_ERRORID_BASE, 5);
D(IDS_ERRORID_JSONCONF_NO_DEVICE,	IDS_ERRORID_BASE, 6);
D(IDS_ERRORID_JSONCONF_EMPTY_KEYS,	IDS_ERRORID_BASE, 7);
D(IDS_ERRORID_JSONCONF_WRITE_EMPTY,	IDS_ERRORID_BASE, 8);

/* Common::MIXER */
D(IDS_ERRORID_INIT_COINITIALIZE,	IDS_ERRORID_BASE, 9);
D(IDS_ERRORID_INIT_COCREATE,		IDS_ERRORID_BASE, 10);
D(IDS_ERRORID_INIT_DEFAULT_AE,		IDS_ERRORID_BASE, 11);
D(IDS_ERRORID_INIT_AS_MANAGER2,		IDS_ERRORID_BASE, 12);
D(IDS_ERRORID_INIT_AS_AUDIOMETER,	IDS_ERRORID_BASE, 13);
D(IDS_ERRORID_MIXER_STARTOK,		IDS_ERRORID_BASE, 14);
D(IDS_ERRORID_MIXER_STARTERROR,		IDS_ERRORID_BASE, 15);
D(IDS_ERRORID_MIXER_STOPOK,		IDS_ERRORID_BASE, 16);
D(IDS_ERRORID_MIXER_STOPERROR,		IDS_ERRORID_BASE, 17);
D(IDS_ERRORID_NO_INIT,			IDS_ERRORID_BASE, 18);
D(IDS_ERRORID_SESSION_LIST,		IDS_ERRORID_BASE, 19);
D(IDS_ERRORID_LISTISNULL,		IDS_ERRORID_BASE, 20);
D(IDS_ERRORID_SESSIONDISCONNECTED,	IDS_ERRORID_BASE, 21);
D(IDS_ERRORID_SESSIONCREATED,		IDS_ERRORID_BASE, 22);
D(IDS_ERRORID_ITEM_SC_AE,		IDS_ERRORID_BASE, 23);
D(IDS_ERRORID_CBVALUE_BAD_SELECT,	IDS_ERRORID_BASE, 24);

/* Common::MIDI */
D(IDS_ERRORID_SET_CB_WARNING,		IDS_ERRORID_BASE, 25);
D(IDS_ERRORID_CONFIG_FAIL,		IDS_ERRORID_BASE, 26);
D(IDS_ERRORID_CONFIG_LOAD,		IDS_ERRORID_BASE, 27);
D(IDS_ERRORID_CONFIG_LOADA,		IDS_ERRORID_BASE, 28);
D(IDS_ERRORID_CONFIG_EMPTY,		IDS_ERRORID_BASE, 29);
D(IDS_ERRORID_CONFIG_NEW,		IDS_ERRORID_BASE, 30);
D(IDS_ERRORID_NOT_IMPLEMENTED,		IDS_ERRORID_BASE, 31);
D(IDS_ERRORID_NOT_CONFIG,		IDS_ERRORID_BASE, 32);
D(IDS_ERRORID_NOT_DEVICES,		IDS_ERRORID_BASE, 33);
D(IDS_ERRORID_COUNT_DEVICES,		IDS_ERRORID_BASE, 34);
D(IDS_ERRORID_OPEN_MIDI_DEVICE,		IDS_ERRORID_BASE, 35);
D(IDS_ERRORID_START_MIDI_DEVICE,	IDS_ERRORID_BASE, 36);
D(IDS_ERRORID_DEVICE_NOT_CONNECT,	IDS_ERRORID_BASE, 37);
D(IDS_ERRORID_DEVICE_FOUND,		IDS_ERRORID_BASE, 38);
D(IDS_ERRORID_DEVICE_OPEN,		IDS_ERRORID_BASE, 39);
D(IDS_ERRORID_DEVICE_CLOSE,		IDS_ERRORID_BASE, 40);
D(IDS_ERRORID_DEVICE_DONE,		IDS_ERRORID_BASE, 41);
D(IDS_ERRORID_NOT_DEVICE_NAME,		IDS_ERRORID_BASE, 42);
D(IDS_ERRORID_NOT_CONFIG_PROXY,		IDS_ERRORID_BASE, 43);
D(IDS_ERRORID_DRIVER_VER_OK,		IDS_ERRORID_BASE, 44);
D(IDS_ERRORID_DRIVER_VER_ERROR,		IDS_ERRORID_BASE, 45);
D(IDS_ERRORID_SEND_BAD_VALUES,		IDS_ERRORID_BASE, 46);
D(IDS_ERRORID_NOT_DRIVER,		IDS_ERRORID_BASE, 47);

/* Common::MMKey */
D(IDS_ERRORID_MMKEY_START,		IDS_ERRORID_BASE, 48);
D(IDS_ERRORID_MMKEY_STOP,		IDS_ERRORID_BASE, 49);

/* Common::MQTT */
D(IDS_ERRORID_MQTT_EMPTY_CONFIG,	IDS_ERRORID_BASE, 50);
D(IDS_ERRORID_MQTT_CLIENT_ERROR,	IDS_ERRORID_BASE, 51);
D(IDS_ERRORID_MQTT_CLIENT_OK,		IDS_ERRORID_BASE, 52);
D(IDS_ERRORID_MQTT_CONNECTED,		IDS_ERRORID_BASE, 53);
D(IDS_ERRORID_MQTT_NOT_CONNECTED,	IDS_ERRORID_BASE, 54);
D(IDS_ERRORID_MQTT_DISCONNECTED,	IDS_ERRORID_BASE, 55);
D(IDS_ERRORID_MQTT_NOT_CA_CERT,		IDS_ERRORID_BASE, 56);
D(IDS_ERRORID_MQTT_START,		IDS_ERRORID_BASE, 57);
D(IDS_ERRORID_MQTT_STOP,		IDS_ERRORID_BASE, 58);

/* Common::MIDIMT */
D(IDS_ERRORID_PANEL_INDEX,		IDS_ERRORID_BASE, 59);
D(IDS_ERRORID_PANEL_INIT,		IDS_ERRORID_BASE, 60);
D(IDS_ERRORID_PANEL_ENABLE,		IDS_ERRORID_BASE, 61);
D(IDS_ERRORID_PANEL_NOT_UI_THREAD,	IDS_ERRORID_BASE, 62);
D(IDS_ERRORID_PANEL_NOT_ITEM_VALID,	IDS_ERRORID_BASE, 63);

/* Common::LIGHT::DMX */
D(IDS_ERRORID_LIGHT_DMX_START,		IDS_ERRORID_BASE, 64);
D(IDS_ERRORID_LIGHT_DMX_NOT_COM,	IDS_ERRORID_BASE, 65);
D(IDS_ERRORID_LIGHT_DMX_NOT_CONFIG,	IDS_ERRORID_BASE, 66);
D(IDS_ERRORID_LIGHT_DMX_STOP,		IDS_ERRORID_BASE, 67);

/* Common::LIGHT::ArtNet */
D(IDS_ERRORID_LIGHT_ARTNET_START,	IDS_ERRORID_BASE, 68);
D(IDS_ERRORID_LIGHT_ARTNET_NOT_COM,	IDS_ERRORID_BASE, 69);
D(IDS_ERRORID_LIGHT_ARTNET_NOT_CONFIG,	IDS_ERRORID_BASE, 70);
D(IDS_ERRORID_LIGHT_ARTNET_STOP,	IDS_ERRORID_BASE, 71);

/* err_NONE */
D(IDS_ERRORID_NONE,			IDS_ERRORID_BASE, 72);
