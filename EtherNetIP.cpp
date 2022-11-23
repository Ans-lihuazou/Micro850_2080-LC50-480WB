#include "EtherNetIP.h"

CEtherNetIP::CEtherNetIP() {

	m_unOTNetworkConnectionID = 0;
	m_unTONetworkConnectionID = 0;
}

CEtherNetIP::~CEtherNetIP() {

}

#ifdef _WIN32

void CEtherNetIP::setSocket(SOCKET socket) {

	m_sClient = socket;
}
#elif __linux__

void CEtherNetIP::setSocket(int nSocket) {

	m_sClient = nSocket;
}
#endif

int CEtherNetIP::init() {

	EIPHEADER_T EIPHeader;
	/* REGISTER->CLOSE->OPEN */

	/* send buff */
	unsigned char szRegisterCmdBuf[1024] = { 0 };
	unsigned char szForwardCloseCmdBuf[1024] = { 0 };
	unsigned char szLargeForwardOpenCmdBuf[1024] = { 0 };

	/* recv buff */
	unsigned char szRegisterResponseCmdBuf[1024] = { 0 };
	unsigned char szForwardCloseResponseCmdBuf[1024] = { 0 };
	unsigned char szLargeForwardOpenResponseCmdBuf[1024] = { 0 };

	//uint32_t ip = 0xc0a838e5;
	//msgEncapsulation.setConnectionSerialNumber(ip&(0xffff));
	//m_msgEncapsulation.setOriginatorSerialNumber(ip >> 16);
	m_msgEncapsulation.setConnectionSerialNumber(Tool::getConnectionSerialNumber());
	m_msgEncapsulation.setOriginatorSerialNumber(Tool::getConnectionSerialNumber()*Tool::getConnectionSerialNumber());

	/*  get register request message */
	/* std::cout << "register\n"; */
	int nRegisterCmdLen = m_msgEncapsulation.encapsulationRegisterMessage(szRegisterCmdBuf, sizeof(szRegisterCmdBuf));

	/* send register request */
	send(m_sClient, (char*)szRegisterCmdBuf, nRegisterCmdLen, 0);
	/* int recvLength =  */recv(m_sClient, (char*)szRegisterResponseCmdBuf, 24, 0);
	memcpy(&EIPHeader, szRegisterResponseCmdBuf, 24);

	/* if success status is 0 */
	if (EIPHeader.unStatus != 0) {
		return -1;
	}

	/* Session Handle */
	if (EIPHeader.unSessionHandle == 0) {
		return -2;
	}
	/* set Session Handle  */
	m_msgEncapsulation.setEHSessionHandle(EIPHeader.unSessionHandle);

	recv(m_sClient, (char*)&szRegisterResponseCmdBuf[24], EIPHeader.usDataLength, 0);


	/* send open request */
	/* std::cout << "open\n"; */
	int nLargeForwardOpenCmdLen = m_msgEncapsulation.encapsulationOpenMessage(szLargeForwardOpenCmdBuf, sizeof(szLargeForwardOpenCmdBuf));

	send(m_sClient, (char*)szLargeForwardOpenCmdBuf, nLargeForwardOpenCmdLen, 0);
	recv(m_sClient, (char*)szLargeForwardOpenResponseCmdBuf, 24, 0);
	memset(&EIPHeader, 0, 24);
	memcpy(&EIPHeader, szLargeForwardOpenResponseCmdBuf, 24);

	/* if success status is 0 */
	if (EIPHeader.unStatus != 0) {
		return -4;
	}

	/* int nLength = */ recv(m_sClient, (char*)&szLargeForwardOpenResponseCmdBuf[24], EIPHeader.usDataLength, 0);

	/* if success General Status is 0x00 */
	if (szLargeForwardOpenResponseCmdBuf[24 + 18] != 0) {
		return -5;
	}

	/* get connect id */
	for (int i = 0, j = 20, k = 24; i < 4; i++, j++, k++) {
		m_unOTNetworkConnectionID |= (uint32_t)szLargeForwardOpenResponseCmdBuf[24 + j] << (i * 8);
		m_unTONetworkConnectionID |= (uint32_t)szLargeForwardOpenResponseCmdBuf[24 + k] << (i * 8);
	}

	return 0;
}



int CEtherNetIP::read(const std::string & request, const uint32_t& c_unCount) {

	EIPHEADER_T EIPHeader;
	uint8_t szReadRequest[1024] = { 0 };
	uint32_t unRequestLen = 0;
	m_msgEncapsulation.setCSDConnectID(m_unOTNetworkConnectionID);
	m_msgEncapsulation.setANSISymbol(request);
	m_msgEncapsulation.setOptionCount(c_unCount);

	unRequestLen = m_msgEncapsulation.encapsulationReadMessage(szReadRequest, sizeof(szReadRequest));

	if (unRequestLen <= 0) {

		std::cout << "error: message\n";
		return -1;
	}

	send(m_sClient, (char*)szReadRequest, unRequestLen, 0);

	recv(m_sClient, (char*)&EIPHeader, 24, 0);

	/* if success status is 0 */
	if (EIPHeader.unStatus != 0) {
		return -2;
	}

	if (EIPHeader.usDataLength == 0) {
		return -3;
	}

	memset(szReadRequest, 0, sizeof(szReadRequest));

	int recvLength = recv(m_sClient, (char*)szReadRequest, EIPHeader.usDataLength, 0);

	if (recvLength != EIPHeader.usDataLength) {
		return -4;
	}

	/* if success General Status is 0x00 */
	if (szReadRequest[25] != 0) {
		return -5;
	}

	//std::cout << "read value(hex) is " << m_msgEncapsulation.unPackReadResponse(szReadRequest, recvLength) << "\n";

	return 0;
}
