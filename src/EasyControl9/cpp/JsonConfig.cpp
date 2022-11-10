/*
	MIDI EasyControl9 to MIDI-Mackie translator for Adobe Premiere Pro Control Surfaces.
	(c) CC 2022, MIT

	See README.md for more details.
	NOT FOR CHINESE USE FOR SALES! FREE SOFTWARE!
*/


#include "..\pch.h"

using namespace std;
using namespace tiny;

constexpr char FIELD_NAME[] = "name";
constexpr char FIELD_CONF[] = "config";
constexpr char FIELD_ASTART[] = "autostart";
constexpr char FIELD_MPORT[] = "manualport";
constexpr char FIELD_PROXY[] = "proxy";
constexpr char FIELD_BTNINTERVAL[] = "btninterval";
constexpr char FIELD_BTNLONGINTERVAL[] = "btnlonginterval";
constexpr char FIELD_UNITS[] = "units";
constexpr char FIELD_SCENE[] = "scene";
constexpr char FIELD_ID[] = "id";
constexpr char FIELD_TYPE[] = "type";
constexpr char FIELD_TARGET[] = "target";
constexpr char FIELD_LONGTARGET[] = "longtarget";

bool JsonConfig::Read(MidiDevice& md, string filepath)
{
	try {
		string jtxt;
		ifstream cnf(filepath.c_str(), ios::in | ios::binary | ios::ate);
		if (cnf.is_open())
		{
#pragma warning( push )
#pragma warning( disable : 4244 )
			streampos size = cnf.tellg();
			char* text = new char[size];
#pragma warning( pop )
			cnf.seekg(0, ios::beg);
			cnf.read(text, size);
			cnf.close();
			jtxt = string(text);
			delete[] text;
		}
		if (jtxt.empty())
			return false;

		TinyJson rjson;
		rjson.ReadJson(jtxt);

		md.name = rjson.Get<string>(FIELD_NAME);
		if (md.name.empty())
			return false;

		md.config = rjson.Get<string>(FIELD_CONF);
		if (md.config.empty())
			md.config = Utils::ConvertCofigName(filepath);

		md.proxy = rjson.Get<bool>(FIELD_PROXY, false);
		md.autostart = rjson.Get<bool>(FIELD_ASTART, false);
		md.manualport = rjson.Get<bool>(FIELD_MPORT, false);
		md.btninterval = rjson.Get<uint32_t>(FIELD_BTNINTERVAL, 100U);
		md.btnlonginterval = rjson.Get<uint32_t>(FIELD_BTNLONGINTERVAL, 500U);

		xarray data = rjson.Get<xarray>(FIELD_UNITS);

		size_t count = data.Count();
		if (count <= 0) return false;
		md.Init(count);

		for (size_t i = 0, n = 0; i < count; i++) {

			data.Enter(static_cast<int>(i));
			int sc = data.Get<int>(FIELD_SCENE, 0);
			int id = data.Get<int>(FIELD_ID);
			int type = data.Get<int>(FIELD_TYPE);
			uint16_t target = data.Get<uint16_t>(FIELD_TARGET);
			uint16_t longtarget = data.Get<uint16_t>(FIELD_LONGTARGET);
			if ((id >= 255) || (type >= 255U) || (target > 255U))
				continue;

			md.units[n].id = id;
			md.units[n].scene = sc;
			md.units[n].type = static_cast<MidiUnitType>(type);
			md.units[n].target = static_cast<Mackie::Target>(target);
			md.units[n++].longtarget = static_cast<Mackie::Target>(longtarget);
		}
	}
	catch (...) { return false; }
	return true;
}

bool JsonConfig::Write(MidiDevice* md, string filepath)
{
	if ((md == nullptr) || md->name.empty())
		return false;

	try {
		TinyJson wjson;
		TinyJson ajson;
		wjson[FIELD_NAME].Set(md->name.c_str());
		wjson[FIELD_CONF].Set(md->config.c_str());
		wjson[FIELD_MPORT].Set(md->manualport);
		wjson[FIELD_PROXY].Set(md->proxy);
		wjson[FIELD_ASTART].Set(md->autostart);
		wjson[FIELD_BTNINTERVAL].Set(md->btninterval);
		wjson[FIELD_BTNLONGINTERVAL].Set(md->btnlonginterval);

		for (size_t i = 0; i < md->Count(); i++) {
			MidiUnit unit = md->units[i];
			TinyJson subj;
			subj[FIELD_SCENE].Set(static_cast<uint16_t>(unit.scene));
			subj[FIELD_ID].Set(static_cast<uint16_t>(unit.id));
			subj[FIELD_TYPE].Set(static_cast<uint16_t>(unit.type));
			subj[FIELD_TARGET].Set(static_cast<uint16_t>(unit.target));
			subj[FIELD_LONGTARGET].Set(static_cast<uint16_t>(unit.longtarget));
			ajson.Push(subj);
		}
		wjson[FIELD_UNITS].Set(ajson);

		ofstream cnf(filepath.c_str(), ios::trunc);
		if (!cnf.is_open())
			return false;

		cnf << wjson.WriteJson();
		cnf.close();
	}
	catch (...) { return false; }
	return true;
}

void JsonConfig::Print(MidiDevice* md)
{
	if ((md == nullptr) || md->name.empty())
		return;

	cout << "\t" << FIELD_NAME << "=" << md->name << endl;
	cout << "\t" << FIELD_CONF << "=" << md->config << endl;
	cout << "\t" << FIELD_ASTART << "=" << md->autostart << endl;
	cout << "\t" << FIELD_MPORT << "=" << md->manualport << endl;
	cout << "\t" << FIELD_PROXY << "=" << md->proxy << endl;
	cout << "\t" << FIELD_BTNINTERVAL << "=" << md->btninterval << endl;
	cout << "\t" << FIELD_BTNLONGINTERVAL << "=" << md->btnlonginterval << endl;

	for (size_t i = 0; i < md->Count(); i++) {
		MidiUnit unit = md->units[i];
		cout << "\t" << FIELD_ID << "=" << static_cast<int>(unit.id) << ", ";
		cout << FIELD_SCENE << "=" << static_cast<int>(unit.scene) << ", ";
		cout << FIELD_TYPE << "=" << static_cast<int>(unit.type) << ", ";
		cout << FIELD_TARGET << "=" << static_cast<int>(unit.target) << ", ";
		cout << FIELD_LONGTARGET << "=" << static_cast<int>(unit.longtarget) << endl;
	}
}
