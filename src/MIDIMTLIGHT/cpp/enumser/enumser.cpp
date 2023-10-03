
#include "global.h"


#if !defined(NO_ENUMSERIAL_USING_SETUPAPI1) || !defined(NO_ENUMSERIAL_USING_SETUPAPI2)
#ifndef _INC_SETUPAPI
#include <setupapi.h>
#endif

#ifndef GUID_DEVINTERFACE_COMPORT
DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86E0D1E0L, 0x8089, 0x11D0, 0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73);
#endif

typedef HKEY(__stdcall SETUPDIOPENDEVREGKEY)(HDEVINFO, PSP_DEVINFO_DATA, DWORD, DWORD, DWORD, REGSAM);
typedef BOOL(__stdcall SETUPDICLASSGUIDSFROMNAME)(LPCTSTR, LPGUID, DWORD, PDWORD);
typedef BOOL(__stdcall SETUPDIDESTROYDEVICEINFOLIST)(HDEVINFO);
typedef BOOL(__stdcall SETUPDIENUMDEVICEINFO)(HDEVINFO, DWORD, PSP_DEVINFO_DATA);
typedef HDEVINFO(__stdcall SETUPDIGETCLASSDEVS)(LPGUID, LPCTSTR, HWND, DWORD);
typedef BOOL(__stdcall SETUPDIGETDEVICEREGISTRYPROPERTY)(HDEVINFO, PSP_DEVINFO_DATA, DWORD, PDWORD, PBYTE, DWORD, PDWORD);
#endif  

#ifndef NO_ENUMSERIAL_USING_ENUMPORTS
#ifndef _WINSPOOL_
#include <winspool.h>
#endif

#pragma comment(lib, "winspool.lib")
#endif

#if !defined(NO_ENUMSERIAL_USING_SETUPAPI1) || !defined(NO_ENUMSERIAL_USING_SETUPAPI2) || !defined(NO_ENUMSERIAL_USING_REGISTRY)
#pragma comment(lib, "advapi32.lib")
#endif


#ifndef NO_ENUMSERIAL_USING_WMI
#ifndef __IWbemLocator_FWD_DEFINED__
#pragma message("To avoid this message, please put WBemCli.h in your pre compiled header (normally stdafx.h)")
#include <WbemCli.h>
#endif

#ifndef _INC_COMDEF
#pragma message("To avoid this message, please put comdef.h in your pre compiled header (normally stdafx.h)")
#include <comdef.h>
#endif

#pragma comment(lib, "WbemUuid.lib")
#endif

#ifndef NO_ENUMSERIAL_USING_COMDB
#ifndef HCOMDB
DECLARE_HANDLE(HCOMDB);
typedef HCOMDB* PHCOMDB;
#endif

#ifndef CDB_REPORT_BYTES
#define CDB_REPORT_BYTES 0x1  
#endif

typedef LONG(__stdcall COMDBOPEN)(PHCOMDB);
typedef LONG(__stdcall COMDBCLOSE)(HCOMDB);
typedef LONG(__stdcall COMDBGETCURRENTPORTUSAGE)(HCOMDB, PBYTE, DWORD, ULONG, LPDWORD);
#endif


#ifndef NO_ENUMSERIAL_USING_CREATEFILE
#if defined CENUMERATESERIAL_USE_STL
BOOL CEnumerateSerial::UsingCreateFile(std::vector<UINT>& ports)
#elif defined _AFX
BOOL CEnumerateSerial::UsingCreateFile(CUIntArray& ports)
#else
BOOL CEnumerateSerial::UsingCreateFile(CSimpleArray<UINT>& ports)
#endif
{
	#if defined CENUMERATESERIAL_USE_STL
	ports.clear();
	#else
	ports.RemoveAll();
	#endif  

	for (UINT i = 1; i < 256; i++)
	{
		TCHAR szPort[32];
		_stprintf_s(szPort, _T("\\\\.\\COM%u"), i);

		BOOL bSuccess = FALSE;
		CAutoHandle port(CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0));
		if (port == INVALID_HANDLE_VALUE)
		{
			DWORD dwError = GetLastError();

			if (dwError == ERROR_ACCESS_DENIED || dwError == ERROR_GEN_FAILURE || dwError == ERROR_SHARING_VIOLATION || dwError == ERROR_SEM_TIMEOUT)
				bSuccess = TRUE;
		}
		else
		{
			bSuccess = TRUE;
		}

		if (bSuccess)
		{
			#if defined CENUMERATESERIAL_USE_STL
			ports.push_back(i);
			#else
			ports.Add(i);
			#endif  
		}
	}

	return TRUE;
}
#endif

#if !defined(NO_ENUMSERIAL_USING_SETUPAPI1) || !defined(NO_ENUMSERIAL_USING_SETUPAPI2) || !defined(NO_ENUMSERIAL_USING_COMDB)
HMODULE CEnumerateSerial::LoadLibraryFromSystem32(LPCTSTR lpFileName)
{
	TCHAR szFullPath[_MAX_PATH];
	szFullPath[0] = _T('\0');
	if (GetSystemDirectory(szFullPath, _countof(szFullPath)) == 0)
		return NULL;

	#pragma warning(suppress: 6102)               
	_tcscat_s(szFullPath, _countof(szFullPath), _T("\\"));
	_tcscat_s(szFullPath, _countof(szFullPath), lpFileName);
	return LoadLibrary(szFullPath);
}
#endif

#if !defined(NO_ENUMSERIAL_USING_SETUPAPI1) || !defined(NO_ENUMSERIAL_USING_SETUPAPI2)
BOOL CEnumerateSerial::RegQueryValueString(HKEY kKey, LPCTSTR lpValueName, LPTSTR& pszValue)
{
	pszValue = NULL;

	DWORD dwType = 0;
	DWORD dwDataSize = 0;
	LONG nError = RegQueryValueEx(kKey, lpValueName, NULL, &dwType, NULL, &dwDataSize);
	if (nError != ERROR_SUCCESS)
	{
		SetLastError(nError);
		return FALSE;
	}

	if (dwType != REG_SZ)
	{
		SetLastError(ERROR_INVALID_DATA);
		return FALSE;
	}

	DWORD dwAllocatedSize = dwDataSize + sizeof(TCHAR);                   
	pszValue = reinterpret_cast<LPTSTR>(LocalAlloc(LMEM_FIXED, dwAllocatedSize));
	if (pszValue == NULL)
		return FALSE;

	pszValue[0] = _T('\0');
	DWORD dwReturnedSize = dwAllocatedSize;
	nError = RegQueryValueEx(kKey, lpValueName, NULL, &dwType, reinterpret_cast<LPBYTE>(pszValue), &dwReturnedSize);
	if (nError != ERROR_SUCCESS)
	{
		LocalFree(pszValue);
		pszValue = NULL;
		SetLastError(nError);
		return FALSE;
	}

	if (dwReturnedSize >= dwAllocatedSize)
	{
		SetLastError(ERROR_INVALID_DATA);
		return FALSE;
	}

	if (pszValue[dwReturnedSize / sizeof(TCHAR) - 1] != _T('\0'))
		pszValue[dwReturnedSize / sizeof(TCHAR)] = _T('\0');

	return TRUE;
}

BOOL CEnumerateSerial::QueryRegistryPortName(HKEY hDeviceKey, int& nPort)
{
	BOOL bAdded = FALSE;

	LPTSTR pszPortName = NULL;
	if (RegQueryValueString(hDeviceKey, _T("PortName"), pszPortName))
	{
		size_t nLen = _tcslen(pszPortName);
		if (nLen > 3)
		{
			if ((_tcsnicmp(pszPortName, _T("COM"), 3) == 0) && IsNumeric((pszPortName + 3), FALSE))
			{
				nPort = _ttoi(pszPortName + 3);

				bAdded = TRUE;
			}
		}
		LocalFree(pszPortName);
	}

	return bAdded;
}
#endif    

BOOL CEnumerateSerial::IsNumeric(LPCSTR pszString, BOOL bIgnoreColon)
{
	size_t nLen = strlen(pszString);
	if (nLen == 0)
		return FALSE;

	BOOL bNumeric = TRUE;

	for (size_t i = 0; i < nLen && bNumeric; i++)
	{
		bNumeric = (isdigit(static_cast<int>(pszString[i])) != 0);
		if (bIgnoreColon && (pszString[i] == ':'))
			bNumeric = TRUE;
	}

	return bNumeric;
}

BOOL CEnumerateSerial::IsNumeric(LPCWSTR pszString, BOOL bIgnoreColon)
{
	size_t nLen = wcslen(pszString);
	if (nLen == 0)
		return FALSE;

	BOOL bNumeric = TRUE;

	for (size_t i = 0; i < nLen && bNumeric; i++)
	{
		bNumeric = (iswdigit(pszString[i]) != 0);
		if (bIgnoreColon && (pszString[i] == L':'))
			bNumeric = TRUE;
	}

	return bNumeric;
}

#ifndef NO_ENUMSERIAL_USING_QUERYDOSDEVICE
#if defined CENUMERATESERIAL_USE_STL
BOOL CEnumerateSerial::UsingQueryDosDevice(std::vector<UINT>& ports)
#elif defined _AFX
BOOL CEnumerateSerial::UsingQueryDosDevice(CUIntArray& ports)
#else
BOOL CEnumerateSerial::UsingQueryDosDevice(CSimpleArray<UINT>& ports)
#endif
{
	BOOL bSuccess = FALSE;

	#if defined CENUMERATESERIAL_USE_STL
	ports.clear();
	#else
	ports.RemoveAll();
	#endif  

	OSVERSIONINFOEX osvi;
	memset(&osvi, 0, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	osvi.dwPlatformId = VER_PLATFORM_WIN32_NT;
	ULONGLONG dwlConditionMask = 0;
	VER_SET_CONDITION(dwlConditionMask, VER_PLATFORMID, VER_EQUAL);
	if (VerifyVersionInfo(&osvi, VER_PLATFORMID, dwlConditionMask))
	{
		int nChars = 4096;
		BOOL bWantStop = FALSE;
		while (nChars && !bWantStop)
		{
			CAutoHeapAlloc devices;
			if (devices.Allocate(nChars * sizeof(TCHAR)))
			{
				LPTSTR pszDevices = static_cast<LPTSTR>(devices.m_pData);
				DWORD dwChars = QueryDosDevice(NULL, pszDevices, nChars);
				if (dwChars == 0)
				{
					DWORD dwError = GetLastError();
					if (dwError == ERROR_INSUFFICIENT_BUFFER)
					{
						nChars *= 2;
					}
					else
						bWantStop = TRUE;
				}
				else
				{
					bSuccess = TRUE;
					bWantStop = TRUE;
					size_t i = 0;

					while (pszDevices[i] != _T('\0'))
					{
						TCHAR* pszCurrentDevice = &(pszDevices[i]);

						size_t nLen = _tcslen(pszCurrentDevice);
						if (nLen > 3)
						{
							if ((_tcsnicmp(pszCurrentDevice, _T("COM"), 3) == 0) && IsNumeric(&(pszCurrentDevice[3]), FALSE))
							{
								int nPort = _ttoi(&pszCurrentDevice[3]);
								#if defined CENUMERATESERIAL_USE_STL
								ports.push_back(nPort);
								#else
								ports.Add(nPort);
								#endif  
							}
						}

						i += (nLen + 1);
					}
				}
			}
			else
			{
				bWantStop = TRUE;
				SetLastError(ERROR_OUTOFMEMORY);
			}
		}
	}
	else
		SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

	return bSuccess;
}
#endif

#ifndef NO_ENUMSERIAL_USING_GETDEFAULTCOMMCONFIG
#if defined CENUMERATESERIAL_USE_STL
BOOL CEnumerateSerial::UsingGetDefaultCommConfig(std::vector<UINT>& ports)
#elif defined _AFX
BOOL CEnumerateSerial::UsingGetDefaultCommConfig(CUIntArray& ports)
#else
BOOL CEnumerateSerial::UsingGetDefaultCommConfig(CSimpleArray<UINT>& ports)
#endif
{
	#if defined CENUMERATESERIAL_USE_STL
	ports.clear();
	#else
	ports.RemoveAll();
	#endif  

	for (UINT i = 1; i < 256; i++)
	{
		TCHAR szPort[32];
		_stprintf_s(szPort, _T("COM%u"), i);

		COMMCONFIG cc;
		DWORD dwSize = sizeof(COMMCONFIG);
		if (GetDefaultCommConfig(szPort, &cc, &dwSize))
		{
			#if defined CENUMERATESERIAL_USE_STL
			ports.push_back(i);
			#else
			ports.Add(i);
			#endif  
		}
	}

	return TRUE;
}
#endif

#ifndef NO_ENUMSERIAL_USING_SETUPAPI1
#if defined CENUMERATESERIAL_USE_STL
#if defined _UNICODE
BOOL CEnumerateSerial::UsingSetupAPI1(std::vector<UINT>& ports, std::vector<std::wstring>& friendlyNames)
#else
BOOL CEnumerateSerial::UsingSetupAPI1(std::vector<UINT>& ports, std::vector<std::string>& friendlyNames)
#endif
#elif defined _AFX
BOOL CEnumerateSerial::UsingSetupAPI1(CUIntArray& ports, CStringArray& friendlyNames)
#else
BOOL CEnumerateSerial::UsingSetupAPI1(CSimpleArray<UINT>& ports, CSimpleArray<CString>& friendlyNames)
#endif
{
	#if defined CENUMERATESERIAL_USE_STL
	ports.clear();
	friendlyNames.clear();
	#else
	ports.RemoveAll();
	friendlyNames.RemoveAll();
	#endif  

	CAutoHModule setupAPI(LoadLibraryFromSystem32(_T("SETUPAPI.DLL")));
	if (setupAPI == NULL)
		return FALSE;

	SETUPDIOPENDEVREGKEY* lpfnSETUPDIOPENDEVREGKEY = reinterpret_cast<SETUPDIOPENDEVREGKEY*>(GetProcAddress(setupAPI, "SetupDiOpenDevRegKey"));
	#if defined _UNICODE
	SETUPDIGETCLASSDEVS* lpfnSETUPDIGETCLASSDEVS = reinterpret_cast<SETUPDIGETCLASSDEVS*>(GetProcAddress(setupAPI, "SetupDiGetClassDevsW"));
	SETUPDIGETDEVICEREGISTRYPROPERTY* lpfnSETUPDIGETDEVICEREGISTRYPROPERTY = reinterpret_cast<SETUPDIGETDEVICEREGISTRYPROPERTY*>(GetProcAddress(setupAPI, "SetupDiGetDeviceRegistryPropertyW"));
	#else
	SETUPDIGETCLASSDEVS* lpfnSETUPDIGETCLASSDEVS = reinterpret_cast<SETUPDIGETCLASSDEVS*>(GetProcAddress(setupAPI, "SetupDiGetClassDevsA"));
	SETUPDIGETDEVICEREGISTRYPROPERTY* lpfnSETUPDIGETDEVICEREGISTRYPROPERTY = reinterpret_cast<SETUPDIGETDEVICEREGISTRYPROPERTY*>(GetProcAddress(setupAPI, "SetupDiGetDeviceRegistryPropertyA"));
	#endif
	SETUPDIDESTROYDEVICEINFOLIST* lpfnSETUPDIDESTROYDEVICEINFOLIST = reinterpret_cast<SETUPDIDESTROYDEVICEINFOLIST*>(GetProcAddress(setupAPI, "SetupDiDestroyDeviceInfoList"));
	SETUPDIENUMDEVICEINFO* lpfnSETUPDIENUMDEVICEINFO = reinterpret_cast<SETUPDIENUMDEVICEINFO*>(GetProcAddress(setupAPI, "SetupDiEnumDeviceInfo"));

	if ((lpfnSETUPDIOPENDEVREGKEY == NULL) || (lpfnSETUPDIDESTROYDEVICEINFOLIST == NULL) ||
		(lpfnSETUPDIENUMDEVICEINFO == NULL) || (lpfnSETUPDIGETCLASSDEVS == NULL) || (lpfnSETUPDIGETDEVICEREGISTRYPROPERTY == NULL))
	{
		setupAPI.m_dwError = ERROR_CALL_NOT_IMPLEMENTED;

		return FALSE;
	}

	GUID guid = GUID_DEVINTERFACE_COMPORT;
	HDEVINFO hDevInfoSet = lpfnSETUPDIGETCLASSDEVS(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hDevInfoSet == INVALID_HANDLE_VALUE)
	{
		setupAPI.m_dwError = GetLastError();

		return FALSE;
	}

	BOOL bMoreItems = TRUE;
	int nIndex = 0;
	SP_DEVINFO_DATA devInfo;
	while (bMoreItems)
	{
		devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
		bMoreItems = lpfnSETUPDIENUMDEVICEINFO(hDevInfoSet, nIndex, &devInfo);
		if (bMoreItems)
		{
			BOOL bAdded = FALSE;

			HKEY hDeviceKey = lpfnSETUPDIOPENDEVREGKEY(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
			if (hDeviceKey != INVALID_HANDLE_VALUE)
			{
				int nPort = 0;
				if (QueryRegistryPortName(hDeviceKey, nPort))
				{
					#if defined CENUMERATESERIAL_USE_STL
					ports.push_back(nPort);
					#else
					ports.Add(nPort);
					#endif  
					bAdded = TRUE;
				}

				RegCloseKey(hDeviceKey);
			}

			if (bAdded)
			{
				TCHAR szFriendlyName[1024];
				szFriendlyName[0] = _T('\0');
				DWORD dwSize = sizeof(szFriendlyName);
				DWORD dwType = 0;
				if (lpfnSETUPDIGETDEVICEREGISTRYPROPERTY(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, reinterpret_cast<PBYTE>(szFriendlyName), dwSize, &dwSize) && (dwType == REG_SZ))
				{
					#if defined CENUMERATESERIAL_USE_STL
					friendlyNames.push_back(szFriendlyName);
					#else
					friendlyNames.Add(szFriendlyName);
					#endif  
				}
				else
				{
					#if defined CENUMERATESERIAL_USE_STL
					friendlyNames.push_back(_T(""));
					#else
					friendlyNames.Add(_T(""));
					#endif  
				}
			}
		}

		++nIndex;
	}

	lpfnSETUPDIDESTROYDEVICEINFOLIST(hDevInfoSet);

	return TRUE;
}
#endif

#ifndef NO_ENUMSERIAL_USING_SETUPAPI2
#if defined CENUMERATESERIAL_USE_STL
#if defined _UNICODE
BOOL CEnumerateSerial::UsingSetupAPI2(std::vector<UINT>& ports, std::vector<std::wstring>& friendlyNames)
#else
BOOL CEnumerateSerial::UsingSetupAPI2(std::vector<UINT>& ports, std::vector<std::string>& friendlyNames)
#endif
#elif defined _AFX
BOOL CEnumerateSerial::UsingSetupAPI2(CUIntArray& ports, CStringArray& friendlyNames)
#else
BOOL CEnumerateSerial::UsingSetupAPI2(CSimpleArray<UINT>& ports, CSimpleArray<CString>& friendlyNames)
#endif
{
	#if defined CENUMERATESERIAL_USE_STL
	ports.clear();
	friendlyNames.clear();
	#else
	ports.RemoveAll();
	friendlyNames.RemoveAll();
	#endif  

	CAutoHModule setupAPI(LoadLibraryFromSystem32(_T("SETUPAPI.DLL")));
	if (setupAPI == NULL)
		return FALSE;

	SETUPDIOPENDEVREGKEY* lpfnSETUPDIOPENDEVREGKEY = reinterpret_cast<SETUPDIOPENDEVREGKEY*>(GetProcAddress(setupAPI, "SetupDiOpenDevRegKey"));
	#if defined _UNICODE
	SETUPDICLASSGUIDSFROMNAME* lpfnSETUPDICLASSGUIDSFROMNAME = reinterpret_cast<SETUPDICLASSGUIDSFROMNAME*>(GetProcAddress(setupAPI, "SetupDiClassGuidsFromNameW"));
	SETUPDIGETCLASSDEVS* lpfnSETUPDIGETCLASSDEVS = reinterpret_cast<SETUPDIGETCLASSDEVS*>(GetProcAddress(setupAPI, "SetupDiGetClassDevsW"));
	SETUPDIGETDEVICEREGISTRYPROPERTY* lpfnSETUPDIGETDEVICEREGISTRYPROPERTY = reinterpret_cast<SETUPDIGETDEVICEREGISTRYPROPERTY*>(GetProcAddress(setupAPI, "SetupDiGetDeviceRegistryPropertyW"));
	#else
	SETUPDICLASSGUIDSFROMNAME* lpfnSETUPDICLASSGUIDSFROMNAME = reinterpret_cast<SETUPDICLASSGUIDSFROMNAME*>(GetProcAddress(setupAPI, "SetupDiClassGuidsFromNameA"));
	SETUPDIGETCLASSDEVS* lpfnSETUPDIGETCLASSDEVS = reinterpret_cast<SETUPDIGETCLASSDEVS*>(GetProcAddress(setupAPI, "SetupDiGetClassDevsA"));
	SETUPDIGETDEVICEREGISTRYPROPERTY* lpfnSETUPDIGETDEVICEREGISTRYPROPERTY = reinterpret_cast<SETUPDIGETDEVICEREGISTRYPROPERTY*>(GetProcAddress(setupAPI, "SetupDiGetDeviceRegistryPropertyA"));
	#endif
	SETUPDIDESTROYDEVICEINFOLIST* lpfnSETUPDIDESTROYDEVICEINFOLIST = reinterpret_cast<SETUPDIDESTROYDEVICEINFOLIST*>(GetProcAddress(setupAPI, "SetupDiDestroyDeviceInfoList"));
	SETUPDIENUMDEVICEINFO* lpfnSETUPDIENUMDEVICEINFO = reinterpret_cast<SETUPDIENUMDEVICEINFO*>(GetProcAddress(setupAPI, "SetupDiEnumDeviceInfo"));

	if ((lpfnSETUPDIOPENDEVREGKEY == NULL) || (lpfnSETUPDICLASSGUIDSFROMNAME == NULL) || (lpfnSETUPDIDESTROYDEVICEINFOLIST == NULL) ||
		(lpfnSETUPDIENUMDEVICEINFO == NULL) || (lpfnSETUPDIGETCLASSDEVS == NULL) || (lpfnSETUPDIGETDEVICEREGISTRYPROPERTY == NULL))
	{
		setupAPI.m_dwError = ERROR_CALL_NOT_IMPLEMENTED;

		return FALSE;
	}

	DWORD dwGuids = 0;
	lpfnSETUPDICLASSGUIDSFROMNAME(_T("Ports"), NULL, 0, &dwGuids);
	if (dwGuids == 0)
	{
		setupAPI.m_dwError = GetLastError();

		return FALSE;
	}

	CAutoHeapAlloc guids;
	if (!guids.Allocate(dwGuids * sizeof(GUID)))
	{
		setupAPI.m_dwError = ERROR_OUTOFMEMORY;

		return FALSE;
	}

	GUID* pGuids = static_cast<GUID*>(guids.m_pData);
	if (!lpfnSETUPDICLASSGUIDSFROMNAME(_T("Ports"), pGuids, dwGuids, &dwGuids))
	{
		setupAPI.m_dwError = GetLastError();

		return FALSE;
	}

	HDEVINFO hDevInfoSet = lpfnSETUPDIGETCLASSDEVS(pGuids, NULL, NULL, DIGCF_PRESENT);
	if (hDevInfoSet == INVALID_HANDLE_VALUE)
	{
		setupAPI.m_dwError = GetLastError();

		return FALSE;
	}

	BOOL bMoreItems = TRUE;
	int nIndex = 0;
	SP_DEVINFO_DATA devInfo;
	while (bMoreItems)
	{
		devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
		bMoreItems = lpfnSETUPDIENUMDEVICEINFO(hDevInfoSet, nIndex, &devInfo);
		if (bMoreItems)
		{
			BOOL bAdded = FALSE;

			HKEY hDeviceKey = lpfnSETUPDIOPENDEVREGKEY(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
			if (hDeviceKey != INVALID_HANDLE_VALUE)
			{
				int nPort = 0;
				if (QueryRegistryPortName(hDeviceKey, nPort))
				{
					#if defined CENUMERATESERIAL_USE_STL
					ports.push_back(nPort);
					#else
					ports.Add(nPort);
					#endif  
					bAdded = TRUE;
				}

				RegCloseKey(hDeviceKey);
			}

			if (bAdded)
			{
				TCHAR szFriendlyName[1024];
				szFriendlyName[0] = _T('\0');
				DWORD dwSize = sizeof(szFriendlyName);
				DWORD dwType = 0;
				if (lpfnSETUPDIGETDEVICEREGISTRYPROPERTY(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, &dwType, reinterpret_cast<PBYTE>(szFriendlyName), dwSize, &dwSize) && (dwType == REG_SZ))
				{
					#if defined CENUMERATESERIAL_USE_STL
					friendlyNames.push_back(szFriendlyName);
					#else
					friendlyNames.Add(szFriendlyName);
					#endif  
				}
				else
				{
					#if defined CENUMERATESERIAL_USE_STL
					friendlyNames.push_back(_T(""));
					#else
					friendlyNames.Add(_T(""));
					#endif  
				}
			}
		}

		++nIndex;
	}

	lpfnSETUPDIDESTROYDEVICEINFOLIST(hDevInfoSet);

	return TRUE;
}
#endif

#ifndef NO_ENUMSERIAL_USING_ENUMPORTS
#if defined CENUMERATESERIAL_USE_STL
BOOL CEnumerateSerial::UsingEnumPorts(std::vector<UINT>& ports)
#elif defined _AFX
BOOL CEnumerateSerial::UsingEnumPorts(CUIntArray& ports)
#else
BOOL CEnumerateSerial::UsingEnumPorts(CSimpleArray<UINT>& ports)
#endif
{
	#if defined CENUMERATESERIAL_USE_STL
	ports.clear();
	#else
	ports.RemoveAll();
	#endif  

	DWORD cbNeeded = 0;
	DWORD dwPorts = 0;
	EnumPorts(NULL, 1, NULL, 0, &cbNeeded, &dwPorts);

	BOOL bSuccess = FALSE;

	CAutoHeapAlloc portsBuffer;
	if (portsBuffer.Allocate(cbNeeded))
	{
		BYTE* pPorts = static_cast<BYTE*>(portsBuffer.m_pData);
		bSuccess = EnumPorts(NULL, 1, pPorts, cbNeeded, &cbNeeded, &dwPorts);
		if (bSuccess)
		{
			PORT_INFO_1* pPortInfo = reinterpret_cast<PORT_INFO_1*>(pPorts);
			for (DWORD i = 0; i < dwPorts; i++)
			{
				size_t nLen = _tcslen(pPortInfo->pName);
				if (nLen > 3)
				{
					if ((_tcsnicmp(pPortInfo->pName, _T("COM"), 3) == 0) && IsNumeric(&(pPortInfo->pName[3]), TRUE))
					{
						int nPort = _ttoi(&(pPortInfo->pName[3]));
						#if defined CENUMERATESERIAL_USE_STL
						ports.push_back(nPort);
						#else
						ports.Add(nPort);
						#endif  
					}
				}

				pPortInfo++;
			}
		}
	}
	else
		SetLastError(ERROR_OUTOFMEMORY);

	return bSuccess;
}
#endif

#ifndef NO_ENUMSERIAL_USING_WMI
#if defined CENUMERATESERIAL_USE_STL
#if defined _UNICODE
BOOL CEnumerateSerial::UsingWMI(std::vector<UINT>& ports, std::vector<std::wstring>& friendlyNames)
#else
BOOL CEnumerateSerial::UsingWMI(std::vector<UINT>& ports, std::vector<std::string>& friendlyNames)
#endif
#elif defined _AFX
BOOL CEnumerateSerial::UsingWMI(CUIntArray& ports, CStringArray& friendlyNames)
#else
BOOL CEnumerateSerial::UsingWMI(CSimpleArray<UINT>& ports, CSimpleArray<CString>& friendlyNames)
#endif
{
	#if defined CENUMERATESERIAL_USE_STL
	ports.clear();
	friendlyNames.clear();
	#else
	ports.RemoveAll();
	friendlyNames.RemoveAll();
	#endif  

	BOOL bSuccess = FALSE;

	IWbemLocator* pLocator = NULL;
	HRESULT hr = CoCreateInstance(CLSID_WbemLocator, NULL, CLSCTX_INPROC_SERVER, IID_IWbemLocator, reinterpret_cast<void**>(&pLocator));
	if (SUCCEEDED(hr))
	{
		IWbemServices* pServices = NULL;
		hr = pLocator->ConnectServer(_bstr_t("\\\\.\\root\\cimv2"), NULL, NULL, NULL, 0, NULL, NULL, &pServices);
		if (SUCCEEDED(hr))
		{
			IEnumWbemClassObject* pClassObject = NULL;
			hr = pServices->CreateInstanceEnum(_bstr_t("Win32_SerialPort"), WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL, &pClassObject);
			if (SUCCEEDED(hr))
			{
				bSuccess = TRUE;

				hr = WBEM_S_NO_ERROR;

				while (hr == WBEM_S_NO_ERROR)
				{
					ULONG uReturned = 0;
					IWbemClassObject* apObj[10];
					memset(apObj, 0, sizeof(apObj));
					hr = pClassObject->Next(WBEM_INFINITE, 10, reinterpret_cast<IWbemClassObject**>(apObj), &uReturned);
					if (SUCCEEDED(hr))
					{
						for (ULONG n = 0; n < uReturned; n++)
						{
							VARIANT varProperty1;
							VariantInit(&varProperty1);
							HRESULT hrGet = apObj[n]->Get(L"DeviceID", 0, &varProperty1, NULL, NULL);
							if (SUCCEEDED(hrGet) && (varProperty1.vt == VT_BSTR) && (wcslen(varProperty1.bstrVal) > 3))
							{
								if ((_wcsnicmp(varProperty1.bstrVal, L"COM", 3) == 0) && IsNumeric(&(varProperty1.bstrVal[3]), TRUE))
								{
									int nPort = _wtoi(&(varProperty1.bstrVal[3]));
									#if defined CENUMERATESERIAL_USE_STL
									ports.push_back(nPort);
									#else
									ports.Add(nPort);
									#endif

									VARIANT varProperty2;
									VariantInit(&varProperty2);
									if (SUCCEEDED(apObj[n]->Get(L"Name", 0, &varProperty2, NULL, NULL)) && (varProperty2.vt == VT_BSTR))
									{
										#if defined CENUMERATESERIAL_USE_STL
										#if defined _UNICODE  
										std::wstring szName(varProperty2.bstrVal);
										#else
										CAutoHeapAlloc szAsciiValue;
										int nLengthA = WideCharToMultiByte(CP_ACP, 0, varProperty2.bstrVal, -1, NULL, 0, NULL, NULL);
										if (nLengthA)
										{
											if (szAsciiValue.Allocate(nLengthA))
												WideCharToMultiByte(CP_ACP, 0, varProperty2.bstrVal, -1, static_cast<LPSTR>(szAsciiValue.m_pData), nLengthA, NULL, NULL);
										}

										std::string szName(static_cast<LPSTR>(szAsciiValue.m_pData));
										#endif
										friendlyNames.push_back(szName);
										#else
										friendlyNames.Add(CString(varProperty2.bstrVal));
										#endif
									}
									else
									{
										#if defined CENUMERATESERIAL_USE_STL
										friendlyNames.push_back(_T(""));
										#else
										friendlyNames.Add(_T(""));
										#endif  
									}

									VariantClear(&varProperty2);
								}
							}

							VariantClear(&varProperty1);

							apObj[n]->Release();
						}
					}
				}

				pClassObject->Release();
			}

			pServices->Release();
		}

		pLocator->Release();
	}

	return bSuccess;
}
#endif

#ifndef NO_ENUMSERIAL_USING_COMDB
#if defined CENUMERATESERIAL_USE_STL
BOOL CEnumerateSerial::UsingComDB(std::vector<UINT>& ports)
#elif defined _AFX
BOOL CEnumerateSerial::UsingComDB(CUIntArray& ports)
#else
BOOL CEnumerateSerial::UsingComDB(CSimpleArray<UINT>& ports)
#endif
{
	#if defined CENUMERATESERIAL_USE_STL
	ports.clear();
	#else
	ports.RemoveAll();
	#endif  

	BOOL bSuccess = FALSE;

	CAutoHModule msPorts(LoadLibraryFromSystem32(_T("MSPORTS.DLL")));
	if (msPorts == NULL)
		return FALSE;

	COMDBOPEN* lpfnLPCOMDBOPEN = reinterpret_cast<COMDBOPEN*>(GetProcAddress(msPorts, "ComDBOpen"));
	COMDBCLOSE* lpfnLPCOMDBCLOSE = reinterpret_cast<COMDBCLOSE*>(GetProcAddress(msPorts, "ComDBClose"));
	COMDBGETCURRENTPORTUSAGE* lpfnCOMDBGETCURRENTPORTUSAGE = reinterpret_cast<COMDBGETCURRENTPORTUSAGE*>(GetProcAddress(msPorts, "ComDBGetCurrentPortUsage"));
	if ((lpfnLPCOMDBOPEN != NULL) && (lpfnLPCOMDBCLOSE != NULL) && (lpfnCOMDBGETCURRENTPORTUSAGE != NULL))
	{
		HCOMDB hComDB;
		DWORD dwComOpen = lpfnLPCOMDBOPEN(&hComDB);
		if (dwComOpen == ERROR_SUCCESS)
		{
			DWORD dwMaxPortsReported = 0;
			DWORD dwPortUsage = lpfnCOMDBGETCURRENTPORTUSAGE(hComDB, NULL, 0, CDB_REPORT_BYTES, &dwMaxPortsReported);
			if (dwPortUsage == ERROR_SUCCESS)
			{
				CAutoHeapAlloc portBytes;
				if (portBytes.Allocate(dwMaxPortsReported))
				{
					bSuccess = TRUE;

					PBYTE pPortBytes = static_cast<PBYTE>(portBytes.m_pData);
					if (lpfnCOMDBGETCURRENTPORTUSAGE(hComDB, pPortBytes, dwMaxPortsReported, CDB_REPORT_BYTES, &dwMaxPortsReported) == ERROR_SUCCESS)
					{
						for (DWORD i = 0; i < dwMaxPortsReported; i++)
						{
							if (pPortBytes[i])
							{
								#if defined CENUMERATESERIAL_USE_STL
								ports.push_back(i + 1);
								#else
								ports.Add(i + 1);
								#endif
							}
						}
					}
				}
				else
					msPorts.m_dwError = ERROR_OUTOFMEMORY;
			}
			else
				msPorts.m_dwError = dwPortUsage;

			lpfnLPCOMDBCLOSE(hComDB);
		}
		else
			msPorts.m_dwError = dwComOpen;
	}
	else
		msPorts.m_dwError = ERROR_CALL_NOT_IMPLEMENTED;

	return bSuccess;
}
#endif

#ifndef NO_ENUMSERIAL_USING_REGISTRY
#if defined CENUMERATESERIAL_USE_STL
#if defined _UNICODE
BOOL CEnumerateSerial::UsingRegistry(std::vector<std::wstring>& ports)
#else
BOOL CEnumerateSerial::UsingRegistry(std::vector<std::string>& ports)
#endif
#elif defined _AFX
BOOL CEnumerateSerial::UsingRegistry(CStringArray& ports)
#else
BOOL CEnumerateSerial::UsingRegistry(CSimpleArray<CString>& ports)
#endif
{
	#if defined CENUMERATESERIAL_USE_STL
	ports.clear();
	#else
	ports.RemoveAll();
	#endif  

	BOOL bSuccess = FALSE;

	HKEY hSERIALCOMM;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_QUERY_VALUE, &hSERIALCOMM) == ERROR_SUCCESS)
	{
		DWORD dwMaxValueNameLen;
		DWORD dwMaxValueLen;
		DWORD dwQueryInfo = RegQueryInfoKey(hSERIALCOMM, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &dwMaxValueNameLen, &dwMaxValueLen, NULL, NULL);
		if (dwQueryInfo == ERROR_SUCCESS)
		{
			DWORD dwMaxValueNameSizeInChars = dwMaxValueNameLen + 1;      
			DWORD dwMaxValueNameSizeInBytes = dwMaxValueNameSizeInChars * sizeof(TCHAR);
			DWORD dwMaxValueDataSizeInChars = dwMaxValueLen / sizeof(TCHAR) + 1;      
			DWORD dwMaxValueDataSizeInBytes = dwMaxValueDataSizeInChars * sizeof(TCHAR);

			CAutoHeapAlloc valueName;
			CAutoHeapAlloc valueData;
			if (valueName.Allocate(dwMaxValueNameSizeInBytes) && valueData.Allocate(dwMaxValueDataSizeInBytes))
			{
				bSuccess = TRUE;

				DWORD dwIndex = 0;
				DWORD dwType;
				DWORD dwValueNameSize = dwMaxValueNameSizeInChars;
				DWORD dwDataSize = dwMaxValueDataSizeInBytes;
				memset(valueName.m_pData, 0, dwMaxValueNameSizeInBytes);
				memset(valueData.m_pData, 0, dwMaxValueDataSizeInBytes);
				TCHAR* szValueName = static_cast<TCHAR*>(valueName.m_pData);
				BYTE* byValue = static_cast<BYTE*>(valueData.m_pData);
				LONG nEnum = RegEnumValue(hSERIALCOMM, dwIndex, szValueName, &dwValueNameSize, NULL, &dwType, byValue, &dwDataSize);
				while (nEnum == ERROR_SUCCESS)
				{
					if (dwType == REG_SZ)
					{
						TCHAR* szPort = reinterpret_cast<TCHAR*>(byValue);
						#if defined CENUMERATESERIAL_USE_STL
						ports.push_back(szPort);
						#else
						ports.Add(szPort);
						#endif  						
					}

					dwValueNameSize = dwMaxValueNameSizeInChars;
					dwDataSize = dwMaxValueDataSizeInBytes;
					memset(valueName.m_pData, 0, dwMaxValueNameSizeInBytes);
					memset(valueData.m_pData, 0, dwMaxValueDataSizeInBytes);
					++dwIndex;
					nEnum = RegEnumValue(hSERIALCOMM, dwIndex, szValueName, &dwValueNameSize, NULL, &dwType, byValue, &dwDataSize);
				}
			}
			else
				SetLastError(ERROR_OUTOFMEMORY);
		}

		RegCloseKey(hSERIALCOMM);

		if (dwQueryInfo != ERROR_SUCCESS)
			SetLastError(dwQueryInfo);
	}

	return bSuccess;
}
#endif
