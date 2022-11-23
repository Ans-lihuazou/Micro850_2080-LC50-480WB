#ifndef ETHERNETIP_H__20221123
#define ETHERNETIP_H__20221123

#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

#elif __linux__

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#endif

#include "MsgEncapsulation.h"


class CEtherNetIP {
public:

	CEtherNetIP();
	virtual ~CEtherNetIP();

#ifdef _WIN32

	void setSocket(SOCKET socket);
#elif __linux__

	void setSocket(int nSocket);
#endif

	int init();


	int read(const std::string& request, const uint32_t& c_unCount = 1);

	template<class T>
	int write(const std::string& c_strTag, const T c_usValue, const uint16_t& c_usDataType,
		const uint32_t& c_unCount = 1, const uint8_t& c_ucStartIndex = 0);

private:

	CMsgEncapsulation m_msgEncapsulation;

	uint32_t m_unOTNetworkConnectionID;
	uint32_t m_unTONetworkConnectionID;

#ifdef _WIN32
	SOCKET m_sClient;

#elif __linux__

	int m_sClient;
#endif

};

template<class T>
inline int CEtherNetIP::write(const std::string& c_strTag, const T c_usValue, const uint16_t& c_usDataType,
	const uint32_t& c_unCount, const uint8_t& c_ucStartIndex) {
	EIPHEADER_T EIPHeader;
	uint8_t szWriteRequest[1024] = { 0 };
	int len = 0;
	m_msgEncapsulation.setCSDConnectID(m_unOTNetworkConnectionID);
	m_msgEncapsulation.setANSISymbol(c_strTag);
	m_msgEncapsulation.setOptionCount(c_unCount);
	m_msgEncapsulation.setValue<T>(c_usValue);
	m_msgEncapsulation.setDataType(c_usDataType);
	m_msgEncapsulation.setStartIndex(c_ucStartIndex);

	len = m_msgEncapsulation.encapsulationWriteMessage<T>(szWriteRequest, sizeof(szWriteRequest));
	//std::cout << " request len is " << len << "\n";
	if (len <= 0) {

		std::cout << "error: message\n";
		return -1;
	}

	send(m_sClient, (char*)szWriteRequest, len, 0);

	recv(m_sClient, (char*)&EIPHeader, 24, 0);

	if (EIPHeader.unStatus != 0) {
		return -2;
	}

	if (EIPHeader.usDataLength == 0) {
		return -3;
	}

	memset(szWriteRequest, 0, sizeof(szWriteRequest));

	int recvLength = recv(m_sClient, (char*)szWriteRequest, EIPHeader.usDataLength, 0);

	if (recvLength != EIPHeader.usDataLength) {
		return -4;
	}

	if (szWriteRequest[24] != 0) {
		return -5;
	}

	return 0;
}

#endif /* ETHERNETIP_H__20221123 */
