/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#include <windows.h>
#include "MidiControllerAPI.h"
#include <iostream>

using namespace std;


void event_status(const std::string s) {
	cout << s << endl;
}


int main()
{
	bool b = OpenTMidiController(&event_status, CONFIG_NAME);
	if (!b) {
		cout << "OpenMidiController return false" << endl;
		std::cin.get();
		return 1;
	}
	TMidiSetButtonOnInterval(100);
	cout << "---- end INIT" << endl;
	/*
	//Start();
	//Save();

	Sleep(7500);
	cout << "---- go to STOP" << endl;
	Stop();
	Sleep(200);
	cout << "---- go to START" << endl;
	Start();
	*/

	std::cout << "Wait\n";
	std::cin.get();
	std::cout << "Go to end\n";

	/*
	while (true) {
		Sleep(10);
		std::cin.get();
	}
	*/

	CloseTMidiController();
	std::cout << "End\n";
	std::cin.get();
	return 0;
}
