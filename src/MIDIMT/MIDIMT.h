/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/

#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <exception>
#include <stdexcept>
#include <system_error>
#include "resource.h"
#include "MidiControllerAPI.h"
#include "DialogStart.h"
#include "RegistryAPI.h"
#include "TrayMenu.h"
#include "TrayNotify.h"
#include "DialogMonitor.h"
