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

#ifndef SYSNETINFO_H
#define SYSNETINFO_H
#include <QString>
#include <vector>
#include <cstdint>

struct NetworkedData {
	inline NetworkedData()
		: m_recv(0), m_send(0) {}
	std::uint64_t m_recv;
	std::uint64_t m_send;
};

class SysNetInfo
{
public:
	static bool GetNetworkInterfaces(std::vector<std::wstring>&);
	static NetworkedData GetNetworkedData(std::wstring const&);
};

#endif // SYSNETINFO_H
