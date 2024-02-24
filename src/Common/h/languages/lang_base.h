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
	namespace LANG {

		struct LANGIDBASE {
		public:
			const LANGID id;
			const wchar_t dllid[6];
			const wchar_t name[12];
			wchar_t locale[24][12]{};
		};

		/*
		* https://docs.translatehouse.org/projects/localization-guide/en/latest/guide/win_lang_ids.html
		* https://learn.microsoft.com/en-us/openspecs/office_standards/ms-oe376/6c085406-a698-4e12-9d4d-c3b0ee3dbc4a
		* https://learn.microsoft.com/en-us/windows/win32/intl/code-page-identifiers
		* https://learn.microsoft.com/en-us/windows/win32/wmformat/localized-system-profiles
		* https://learn.microsoft.com/en-us/windows/win32/tablet/using-the-recognizers-collection
		* 
		* https://flagpedia.net/index
		*/

		static const LANGIDBASE lang_id_base[] = {
			{				0, L"AUTO\0", L"AUTO\0",		{ L"no-NE\0", L'\0'}},
			{    LANG_RUSSIAN, L"RU\0",   L"Russian\0",		{ L"ru-RU\0", L"ru-MD\0", L'\0'}}, /* 1049 */
			{ LANG_BELARUSIAN, L"BY\0",   L"Belarusian\0",	{ L"be-BY\0", L'\0'}}, /* 1059 */
			{  LANG_BULGARIAN, L"BG\0",   L"Bulgarian\0",	{ L"bg-BG\0", L'\0'}}, /* 1026 */
			{    LANG_BOSNIAN, L"BS\0",   L"Bosnian\0",		{ L"bs-Latn-BA\0", L'\0'}}, /* 5146 */ /* +dup */
			{   LANG_CROATIAN, L"HR\0",   L"Croatian\0",	{ L"hr-HR\0", L"hr-BA\0", L'\0'}}, /* 1050 */ /* +dup */
			{      LANG_CZECH, L"CZ\0",   L"Czech\0",		{ L"cs-CZ\0", L'\0'}}, /* 1029 */
			{     LANG_SLOVAK, L"SK\0",   L"Slovak\0",		{ L"sk-SK\0", L'\0'}}, /* 1051 */
			{  LANG_SLOVENIAN, L"SL\0",   L"Slovenian\0",	{ L"sl-SI\0", L'\0'}}, /* 1060 */
			{    LANG_SERBIAN, L"SR\0",   L"Serbian\0",		{ L"sr-Latn-CS\0", L"sr-Cyrl-CS\0", L'\0'}}, /* 3098 */ /* +dup */
			{  LANG_HUNGARIAN, L"HU\0",   L"Hungarian\0",	{ L"hu-HU\0", L'\0'}}, /* 1038 */
			{   LANG_ARMENIAN, L"AR\0",   L"Armenian\0",	{ L"hy-AM\0", L'\0'}}, /* 1067 */
			{   LANG_GEORGIAN, L"GR\0",   L"Georgian\0",	{ L"ka-GE\0", L'\0'}}, /* 1079 */
			{  LANG_UKRAINIAN, L"RU\0",   L"Ukrainian\0",	{ L"uk-UA\0", L'\0'}}, /* 1049 */
			{   LANG_ROMANIAN, L"RO\0",   L"Romanian\0",	{ L"ro-RO\0", L"ro-MD\0", L"rm-CH\0", L'\0'}}, /* 1048 */
			{    LANG_ENGLISH, L"EN\0",   L"English\0",		{ L"en-US\0", L"en-GB\0", L"en-AU\0", L"en-CA\0", L"en-NZ\0", L"en-IE\0", L"en-ZA\0", L"en-JM\0", L"en-BZ\0", L"en-TT\0", L"en-ZW\0", L"en-PH\0", L"en-ID\0", L"en-HK\0", L"en-IN\0", L"en-MY\0", L"en-SG\0", L"en-029\0", L'\0'}}, /* 1033 */
			{     LANG_GERMAN, L"DE\0",   L"German\0",		{ L"de-DE\0", L"de-CH\0", L"de-AT\0", L"de-LU\0", L"de-LI\0", L'\0'}}, /* 1031 */
			{     LANG_FRENCH, L"FR\0",   L"French\0",		{ L"fr-FR\0", L"fr-FR\0", L"fr-BE\0", L"fr-CA\0", L"fr-CH\0", L"fr-LU\0", L"fr-MC\0", L"fr-RE\0", L"fr-CG\0", L"fr-SN\0", L"fr-CM\0", L"fr-CI\0", L"fr-ML\0", L"fr-MA\0", L"fr-FR\0", L"fr-FR\0", L"fr-HT\0", L"fr-029\0", L"fr-015\0", L'\0'}}, /* 1036 */
			{    LANG_FRISIAN, L"WF\0",   L"Frisian\0",		{ L"fy-NL\0", L'\0'}}, /* 1122 */
			{      LANG_DUTCH, L"NL\0",   L"Belgium\0",		{ L"nl-NL\0", L"nl-BE\0", L'\0'}}, /* 1043, 2067 */
			{     LANG_DANISH, L"DA\0",   L"Dutch\0",		{ L"da-DK\0", L'\0'}}, /* 1030 */
			{    LANG_SWEDISH, L"SV\0",   L"Swedish\0",		{ L"sv-SE\0", L"sv-FI\0", L'\0'}}, /* 1053 */
			{  LANG_NORWEGIAN, L"NR\0",   L"Norwegian\0",	{ L"nn-NO\0", L"nb-NO\0", L'\0'}}, /* 2068 */
			{ LANG_PORTUGUESE, L"PG\0",   L"Portuguese\0",	{ L"pt-PT\0", L"pt-BR\0", L'\0'}}, /* 2070 */
			{    LANG_SPANISH, L"SP\0",   L"Spanish\0",		{ L"es-ES\0", L"es-MX\0", L"es-GT\0", L"es-CR\0", L"es-PA\0", L"es-DO\0", L"es-VE\0", L"es-CO\0", L"es-PE\0", L"es-AR\0", L"es-PE\0", L"es-EC\0", L"es-CL\0", L"es-UY\0", L"es-PY\0", L"es-BO\0", L"es-SV\0", L"es-HN\0", L"es-NI\0", L"es-PR\0", L"es-US\0", L"es-419\0", L'\0'}}, /* 1034, 3082 */
			{   LANG_JAPANESE, L"JP\0",   L"Japanese\0",	{ L"ja-JP\0", L'\0'}}, /* 1041 */
			{    LANG_CHINESE, L"CN\0",   L"Chinese\0",		{ L"zh-CN\0", L"zh-HK\0", L"zh-SG\0", L"zh-MO\0", L"zh-TW\0", L'\0'}}, /* 2029 */
			{       LANG_ZULU, L"ZU\0",   L"Zulu\0",		{ L"zu-ZA\0", L'\0'}}, /* 1077 */
		};
	}
}
