//{{NO_DEPENDENCIES}}
// Включаемый файл, созданный в Microsoft Visual C++.
// Используется resource.rc
//

#ifndef APSTUDIO_INVOKED

#define TEXTS_CONCAT2_(A,B) A ## B
#define TEXTS_CONCAT3_(A,B,C) A ## B ## C

#define VER_FIRST 1
#define VER_SECOND 0
#define VER_MAJOR 4
#define VER_MINOR 1
#define VER_TEXT "1.0.4.1"
#define YEAR_TEXT "2023"

#if defined(__W64)
#    define CURRENT_COPY TEXTS_CONCAT3_("Copyright (C) CC ", YEAR_TEXT, " MIT, x64\0")
#    define CURRENT_VERSION TEXTS_CONCAT2_(VER_TEXT, " x64\0")
#elif defined(__W32)
#    define CURRENT_COPY TEXTS_CONCAT3_("Copyright (C) CC ", YEAR_TEXT, " MIT, x32\0")
#    define CURRENT_VERSION TEXTS_CONCAT2_(VER_TEXT, " x32\0")
#else
#    define CURRENT_COPY TEXTS_CONCAT3_("Copyright (C) CC ", YEAR_TEXT, " MIT, xUN\0")
#    define CURRENT_VERSION TEXTS_CONCAT2_(VER_TEXT, " xUN\0")
#endif


#endif

// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        101
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1000
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif
