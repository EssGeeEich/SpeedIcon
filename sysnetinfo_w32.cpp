/*	Copyright (c) 2015 SGH
**	
**	Permission is granted to use, modify and redistribute this software.
**	Modified versions of this software MUST be marked as such.
**	
**	This software is provided "AS IS". In no event shall
**	the authors or copyright holders be liable for any claim,
**	damages or other liability. The above copyright notice
**	and this permission notice shall be included in all copies
**	or substantial portions of the software.
**	
**	File created on: 14/12/2015
*/

#ifdef _WIN32

#define NTDDI_VERSION NTDDI_VISTASP1
#define _WIN32_WINNT 0x0600

#include "sysnetinfo.h"
#include <winsock2.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>

bool SysNetInfo::GetNetworkInterfaces(std::vector<std::wstring>& result)
{
	MIB_IF_TABLE2* table = nullptr;
	if(GetIfTable2Ex(MibIfTableNormal,&table) != NO_ERROR || !table)
		return false;
	
	try {
		result.resize(table->NumEntries);
		for(ULONG i = 0; i < table->NumEntries; ++i)
		{
			result[i] = table->Table[i].Alias;
		}
	} catch(...) {
		FreeMibTable(table);
		throw;
	}

	FreeMibTable(table);
	return true;
}

NetworkedData SysNetInfo::GetNetworkedData(const std::wstring& intfc)
{
	NetworkedData data;
	
	MIB_IF_TABLE2* table = nullptr;
	if(GetIfTable2Ex(MibIfTableNormal,&table) != NO_ERROR || !table)
		return data;
	
	try {
		for(ULONG i = 0; i < table->NumEntries; ++i)
		{
			if(intfc == table->Table[i].Alias)
			{
				data.m_recv = table->Table[i].InOctets;
				data.m_send = table->Table[i].OutOctets;
				break;
			}
		}
	} catch(...) {
		FreeMibTable(table);
		throw;
	}
	FreeMibTable(table);
	return data;
}

#endif
