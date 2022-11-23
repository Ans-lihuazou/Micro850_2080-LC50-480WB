#include "MsgEncapsulation.h"

CMsgEncapsulation::CMsgEncapsulation() {

	/* Encapsulation Header */
	m_usEHCommond = 0;
	m_usEHLength = 0;
	m_unEHSessionHandle = 0;

	/* Command Specific Data */
	/* address item */
	m_unCSDConnectID = 0;
	/* data item */
	m_usCSDDataLength = 0;

	/* Common Industrial Protocol */
	m_ucCIPServiceCode = 0;
	m_strCIPPathSegment = "";

	/* Common Industrial Protocol */
	m_usConnectionSerialNumber = 0;
	m_usOriginatorSerialNumber = 0;

	m_strValue = "";
	//std::cout << " m_unEHSessionHandle: " << m_unEHSessionHandle << "\n";
}

CMsgEncapsulation::~CMsgEncapsulation() {

}

void CMsgEncapsulation::setEHCommond(const uint16_t& c_usCommond) {

	this->m_usEHCommond = c_usCommond;
}

void CMsgEncapsulation::setEHLength(const uint16_t & c_usLength) {

	this->m_usEHLength = c_usLength;
}

void CMsgEncapsulation::setEHSessionHandle(const uint32_t & c_unHandle) {

	this->m_unEHSessionHandle = c_unHandle;
}

void CMsgEncapsulation::setCSDConnectID(const uint32_t & c_unId) {

	this->m_unCSDConnectID = c_unId;
}

void CMsgEncapsulation::setCSDDataLength(const uint16_t & c_usLength) {

	this->m_usCSDDataLength = c_usLength;
}

void CMsgEncapsulation::setCIPServiceCode(const uint8_t & c_ucCode) {

	this->m_ucCIPServiceCode = c_ucCode;
}

void CMsgEncapsulation::addPath(const uint16_t & c_uspath) {

	this->m_strCIPPathSegment.append(std::to_string(c_uspath));
}

void CMsgEncapsulation::initPath() {

	this->m_strCIPPathSegment.clear();
}

void CMsgEncapsulation::setConnectionSerialNumber(const uint16_t & c_usIp) {

	this->m_usConnectionSerialNumber = c_usIp;
}

void CMsgEncapsulation::setOriginatorSerialNumber(const uint16_t & c_usIp) {

	//this->m_usOriginatorSerialNumber = (uint32_t)(c_usIp << 16) | 0x015a;
	this->m_usOriginatorSerialNumber = (uint32_t)(c_usIp);
}

void CMsgEncapsulation::setANSISymbol(const std::string & c_strSymbol) {

	this->m_strANSISymbol = c_strSymbol;
}

/*void CMsgEncapsulation::setValue(const uint32_t & c_usValue){

	this->m_usValue = c_usValue;
}*/


void CMsgEncapsulation::setDataType(const uint16_t & c_usDataType) {

	this->m_usDataType = c_usDataType;
}

void CMsgEncapsulation::setOptionCount(const uint16_t & c_unOptCount) {

	this->m_usOptionCount = c_unOptCount;
}

void CMsgEncapsulation::setStartIndex(const uint8_t & c_ucStartIndex) {

	this->m_ucStartIndex = c_ucStartIndex;
}

int CMsgEncapsulation::encapsulationEncapsulationHeader(uint8_t * pData, const uint32_t & c_unLength) {

	EIPHEADER_T eipHeader;

	if (m_usEHLength <= 0) {
		return -1;
	}

	eipHeader.usEIPCommand = m_usEHCommond;
	eipHeader.usDataLength = m_usEHLength;
	eipHeader.unSessionHandle = this->m_unEHSessionHandle;
	eipHeader.unStatus = 0;
	eipHeader.ullSenderContext = 0;
	eipHeader.unOptions = 0;

	if (sizeof(eipHeader) > c_unLength) {
		return -2;
	}

	memcpy(pData, &eipHeader, sizeof(eipHeader));
	return sizeof(eipHeader);
}

int CMsgEncapsulation::encapsulationUnConnectCommandSpecificData(uint8_t * pData, const uint32_t & c_unLength) {

	UNCONNECT_EIPDATA_T eipData;

	eipData.unInterfaceHandl = 0;
	eipData.usTimeOut = 0;
	eipData.usItemCounts = 2;
	eipData.usAddressItemTypeID = 0;
	eipData.usAddressItemLength = 0;
	eipData.usDataItemTypeID = 0xB2;
	eipData.usCIPMessageLength = m_usCSDDataLength;

	setEHLength(m_usCSDDataLength + sizeof(eipData));
	memcpy(pData, &eipData, sizeof(eipData));

	if (sizeof(eipData) > c_unLength) {
		return -1;
	}

	return sizeof(eipData);
}

int CMsgEncapsulation::encapsulationCommonIndustrialProtocol(uint8_t * pData, const uint32_t & c_unLength) {

	UNCONNECT_COMMOMINDUSTRIALPROTOCOL_T cip;

	cip.ucServiceCode = m_ucCIPServiceCode;
	/* default value */
	cip.ucPathSize = 2;
	cip.ucPathSegment1 = 0x20;
	cip.ucClass = 0x06;
	cip.ucPathSegment2 = 0x24;
	cip.ucIstance = 0x01;

	memcpy(pData, &cip, sizeof(cip));

	if (sizeof(cip) > c_unLength) {
		return -1;
	}

	setCSDDataLength(m_usCSDDataLength + sizeof(cip));

	return sizeof(cip);
}

int CMsgEncapsulation::encapsulationCIPConnectionManager(uint8_t * pData, const uint32_t & c_unLength) {

	if (m_ucCIPServiceCode == CIP_FORWARD_CLOSE_CODE) {

		CLOSE_CIPCONNECTIOPNMANAGER_T cipClose;

		cipClose.usActualTimeOut = 0xf705;
		cipClose.usConnectionSerialNumber = m_usConnectionSerialNumber;
		cipClose.usOriginatorVendorID = 0x0030;
		cipClose.unOriginatorSerialNumber = m_usOriginatorSerialNumber;
		cipClose.ucConnectionPathSize = 0x03;
		cipClose.ucReserved = 0x00;

		//cipClose.connectionPath.ucPathSegment1 = 0x01;
		//cipClose.connectionPath.ucLinkAddress = 0;
		cipClose.connectionPath.ucPathSegment2 = 0x20;
		cipClose.connectionPath.ucClass = 0x02;
		cipClose.connectionPath.ucPathSegment3 = 0x24;
		cipClose.connectionPath.ucIstance = 0x01;

		if (sizeof(cipClose) > c_unLength) {
			return -1;
		}

		memcpy(pData, &cipClose, sizeof(cipClose));

		setCSDDataLength(sizeof(cipClose));
		return sizeof(cipClose);
	}
	else if (m_ucCIPServiceCode == CIP_LARGE_FORWARD_OPEN_CODE) {

		Large_Forward_Open_CIPCONNECTIOPNMANAGER_T cipOpen;
		cipOpen.usActualTimeOut = 0x9a06;

		//cipOpen.unOTNetworkConnectionID = 0x33000000;
		//cipOpen.unTONetworkConnectionID = 0x9d9d0029;

		cipOpen.unOTNetworkConnectionID = Tool::getRand();
		cipOpen.unTONetworkConnectionID = Tool::getRand();

		cipOpen.usConnectionSerialNumber = m_usConnectionSerialNumber;
		cipOpen.usOriginatorVendorID = 0x0030;
		cipOpen.unOriginatorSerialNumber = m_usOriginatorSerialNumber;

		cipOpen.unConnectionTimeOutMulAndReserved = 0x07;
		cipOpen.unOTRPI = 0x006e6b28;
		cipOpen.unOTNetworkConnectionParam = 0x43f807cc;
		cipOpen.unTORPI = 0x006e6b28;
		cipOpen.unTONetworkConnectionParam = 0x43f807cc;

		cipOpen.ucTrigger = 0xa3;

		cipOpen.ucConnectionPathSize = 0x03;

		//cipOpen.connectionPath.ucPathSegment1 = 0x01;
		//cipOpen.connectionPath.ucLinkAddress = 0;
		cipOpen.connectionPath.ucPathSegment2 = 0x20;
		cipOpen.connectionPath.ucClass = 0x02;
		cipOpen.connectionPath.ucPathSegment3 = 0x24;
		cipOpen.connectionPath.ucIstance = 0x01;

		if (sizeof(cipOpen) > c_unLength) {
			return -2;
		}

		memcpy(pData, &cipOpen, sizeof(cipOpen));

		setCSDDataLength(sizeof(cipOpen));
		return sizeof(cipOpen);
		//read O->T
		//write T->O
	}
	else if (m_ucCIPServiceCode == CIP_FORWARD_OPEN_CODE) {
		Forward_Open_CIPCONNECTIOPNMANAGER_T cipOpen;
		cipOpen.usActualTimeOut = 0xff0f;

		//cipOpen.unOTNetworkConnectionID = 0x33000000;
		//cipOpen.unTONetworkConnectionID = 0x9d9d0029;

		cipOpen.unOTNetworkConnectionID = Tool::getRand();
		cipOpen.unTONetworkConnectionID = Tool::getRand();

		cipOpen.usConnectionSerialNumber = m_usConnectionSerialNumber;
		cipOpen.usOriginatorVendorID = 0x0030; //0x0212;
		cipOpen.unOriginatorSerialNumber = m_usOriginatorSerialNumber;

		cipOpen.unConnectionTimeOutMulAndReserved = 0x02;
		cipOpen.unOTRPI = 0x001e8480;
		cipOpen.unOTNetworkConnectionParam = 0x43f8;
		cipOpen.unTORPI = 0x001e8480;
		cipOpen.unTONetworkConnectionParam = 0x43f8;

		cipOpen.ucTrigger = 0xa3;

		cipOpen.ucConnectionPathSize = 0x02;

		//cipOpen.connectionPath.ucPathSegment1 = 0x01;
		//cipOpen.connectionPath.ucLinkAddress = 0;
		cipOpen.connectionPath.ucPathSegment2 = 0x20;
		cipOpen.connectionPath.ucClass = 0x02;
		cipOpen.connectionPath.ucPathSegment3 = 0x24;
		cipOpen.connectionPath.ucIstance = 0x01;

		if (sizeof(cipOpen) > c_unLength) {
			return -2;
		}

		memcpy(pData, &cipOpen, sizeof(cipOpen));

		setCSDDataLength(sizeof(cipOpen));
		return sizeof(cipOpen);
	}

	return 0;
}

int CMsgEncapsulation::encapsulationConnectCommandSpecificData(uint8_t * pData, const uint32_t & c_unLength) {

	CONNECT_EIPDATA_T eipData;

	eipData.unInterfaceHandl = 0;
	eipData.usTimeOut = 0;
	eipData.usItemCounts = 2;

	eipData.usAddressItemTypeID = 0x00a1;
	eipData.usAddressItemLength = 0x04;
	eipData.unConectionID = this->m_unCSDConnectID;

	eipData.usDataItemTypeID = 0xb1;
	//std::cout << " use length is " << m_usCSDDataLength << "\n";
	eipData.usCIPMessageLength = m_usCSDDataLength + 2;
	eipData.usCIPSquenceCount = clock();

	setEHLength(m_usCSDDataLength + sizeof(eipData));

	if (sizeof(eipData) > c_unLength) {
		return -1;
	}
	memcpy(pData, &eipData, sizeof(eipData));

	return sizeof(eipData);
}

int CMsgEncapsulation::encapsulationReadCommonIndustrialProtocol(uint8_t * pData, const uint32_t & c_unLength) {

	READ_COMMOMINDUSTRIALPROTOCOL_T readRequest;

	readRequest.strANSISymbol = this->m_strANSISymbol;

	readRequest.ucDataSize = this->m_strANSISymbol.length();

	readRequest.ucPathSegmentType = 0x91;

	readRequest.ucPathSize = (readRequest.strANSISymbol.length() + 1) / 2 + 1;

	readRequest.ucServiceCode = m_ucCIPServiceCode;

	if (sizeof(readRequest) > c_unLength) {
		return -1;
	}

	uint32_t unLength = 0;
	memcpy(pData, &readRequest, 4);
	unLength = 4;
	memcpy(pData + unLength, readRequest.strANSISymbol.c_str(), readRequest.strANSISymbol.length());
	unLength += readRequest.strANSISymbol.length();
	//std::cout << "set length is " << m_usCSDDataLength + 4 + readRequest.strANSISymbol.length() << "\n";
	if (unLength & 1) {
		uint8_t ucComplement = 0x00;
		memcpy(pData + unLength, &ucComplement, sizeof(ucComplement));
		unLength++;
	}
	setCSDDataLength(m_usCSDDataLength + unLength);

	return unLength;
}

int CMsgEncapsulation::encapsulationReadCIPClassGeneric(uint8_t * pData, const uint32_t & c_unLength) {

	if (this->m_usOptionCount == 0) {
		return -1;
	}

	if (sizeof(this->m_usOptionCount) > c_unLength) {
		return -2;
	}

	memcpy(pData, &this->m_usOptionCount, sizeof(this->m_usOptionCount));



	setCSDDataLength(sizeof(this->m_usOptionCount));

	return sizeof(this->m_usOptionCount);
}

/*
int CMsgEncapsulation::encapsulationWriteCIPClassGeneric(uint8_t * pData, const uint32_t & c_unLength) {

	int len = 0;
	if (len + sizeof(this->m_usDataType) > c_unLength) {
		return -1;
	}
	memcpy(pData, &this->m_usDataType, sizeof(this->m_usDataType));
	len += sizeof(this->m_usDataType);

	if (len + sizeof(this->m_usOptionCount) > c_unLength) {
		return -2;
	}
	memcpy(pData + len, &this->m_usOptionCount, sizeof(this->m_usOptionCount));
	len += sizeof(this->m_usOptionCount);

	if (len + m_strValue.length() > c_unLength) {
		return -3;
	}
	memcpy(pData + len, m_strValue.c_str(), m_strValue.length());
	len += m_strValue.length();

	setCSDDataLength(len);

	return len;
}
*/

/*
int CMsgEncapsulation::encapsulationWriteCommonIndustrialProtocol(uint8_t * pData, const uint32_t & c_unLength) {

	READ_COMMOMINDUSTRIALPROTOCOL_T readRequest;

	readRequest.strANSISymbol = this->m_strANSISymbol;

	readRequest.ucDataSize = this->m_strANSISymbol.length();

	readRequest.ucPathSegmentType = 0x91;

	readRequest.ucPathSize = (readRequest.strANSISymbol.length() + 1) / 2 + 2;

	readRequest.ucServiceCode = m_ucCIPServiceCode;

	if (sizeof(readRequest) > c_unLength) {
		return -1;
	}

	uint32_t unLength = 0;
	memcpy(pData, &readRequest, 4);
	unLength = 4;
	memcpy(pData + unLength, readRequest.strANSISymbol.c_str(), readRequest.strANSISymbol.length());
	unLength += readRequest.strANSISymbol.length();
	//std::cout << "set length is " << m_usCSDDataLength + 4 + readRequest.strANSISymbol.length() << "\n";
	if (unLength & 1) {
		uint8_t ucComplement = 0x00;
		memcpy(pData + unLength, &ucComplement, sizeof(ucComplement));
		unLength++;
	}

	uint16_t temp = 0x0028;

	memcpy(pData + unLength, &temp, 2);

	unLength += 2;

	setCSDDataLength(m_usCSDDataLength + unLength);

	return unLength;
}
*/

int CMsgEncapsulation::encapsulationRegisterMessage(uint8_t * pData, const uint32_t & c_unLength) {

	uint8_t szEncapsulationHeader[1024];
	uint32_t len = 0;
	setEHCommond(EIP_REGISTER_CMD);
	setEHLength(4);
	if ((len = encapsulationEncapsulationHeader(szEncapsulationHeader, sizeof(szEncapsulationHeader))) < 0) {
		return -1;
	}

	if (len > c_unLength) {
		return -2;
	}
	memcpy(pData, szEncapsulationHeader, len);

	if (len + 4 > c_unLength) {
		return -3;
	}

	uint8_t data[4];
	memset(data, 0, 4);
	data[0] = 0x01;

	memcpy(pData + len, data, 4);
	len += 4;

	return len;
}

int CMsgEncapsulation::encapsulationCloseMessage(uint8_t * pData, const uint32_t & c_unLength) {

	uint8_t szEncapsulationHeader[1024];
	uint8_t szCommandSpecificData[1024];
	uint8_t szCommonIndustrialProtocol[1024];
	uint8_t szCIPConnectionManager[1024];
	uint32_t len1, len2, len3, len4, len;

	len1 = len2 = len3 = len4 = len = 0;

	setEHCommond(EIP_SENDRRDATA_CMD);
	setCIPServiceCode(CIP_FORWARD_CLOSE_CODE);

	if ((len1 = encapsulationCIPConnectionManager(szCIPConnectionManager, sizeof(szCIPConnectionManager))) < 0) {
		return -1;
	}
	if ((len2 = encapsulationCommonIndustrialProtocol(szCommonIndustrialProtocol, sizeof(szCommonIndustrialProtocol))) < 0) {
		return -2;
	}
	if ((len3 = encapsulationUnConnectCommandSpecificData(szCommandSpecificData, sizeof(szCommandSpecificData))) < 0) {
		return -3;
	}
	if ((len4 = encapsulationEncapsulationHeader(szEncapsulationHeader, sizeof(szEncapsulationHeader))) < 0) {
		return -4;
	}

	if (len + len4 > c_unLength) {
		return -5;
	}
	memcpy(pData, szEncapsulationHeader, len4);
	len += len4;

	if (len + len3 > c_unLength) {
		return -6;
	}
	memcpy(pData + len, szCommandSpecificData, len3);
	len += len3;

	if (len + len2 > c_unLength) {
		return -7;
	}
	memcpy(pData + len, szCommonIndustrialProtocol, len2);
	len += len2;

	if (len + len1 > c_unLength) {
		return -8;
	}
	memcpy(pData + len, szCIPConnectionManager, len1);
	len += len1;

	return len;
}

int CMsgEncapsulation::encapsulationOpenMessage(uint8_t * pData, const uint32_t & c_unLength) {

	uint8_t szEncapsulationHeader[1024];
	uint8_t szCommandSpecificData[1024];
	uint8_t szCommonIndustrialProtocol[1024];
	uint8_t szCIPConnectionManager[1024];
	uint32_t len1, len2, len3, len4, len;

	len1 = len2 = len3 = len4 = len = 0;

	setEHCommond(EIP_SENDRRDATA_CMD);
	//setCIPServiceCode(CIP_LARGE_FORWARD_OPEN_CODE);
	setCIPServiceCode(CIP_FORWARD_OPEN_CODE);

	if ((len1 = encapsulationCIPConnectionManager(szCIPConnectionManager, sizeof(szCIPConnectionManager))) < 0) {
		return -1;
	}
	if ((len2 = encapsulationCommonIndustrialProtocol(szCommonIndustrialProtocol, sizeof(szCommonIndustrialProtocol))) < 0) {
		return -2;
	}
	if ((len3 = encapsulationUnConnectCommandSpecificData(szCommandSpecificData, sizeof(szCommandSpecificData))) < 0) {
		return -3;
	}
	if ((len4 = encapsulationEncapsulationHeader(szEncapsulationHeader, sizeof(szEncapsulationHeader))) < 0) {
		return -4;
	}

	if (len + len4 > c_unLength) {
		return -5;
	}
	memcpy(pData, szEncapsulationHeader, len4);
	len += len4;

	if (len + len3 > c_unLength) {
		return -6;
	}
	memcpy(pData + len, szCommandSpecificData, len3);
	len += len3;

	if (len + len2 > c_unLength) {
		return -7;
	}
	memcpy(pData + len, szCommonIndustrialProtocol, len2);
	len += len2;

	if (len + len1 > c_unLength) {
		return -8;
	}
	memcpy(pData + len, szCIPConnectionManager, len1);
	len += len1;

	return len;
}

int CMsgEncapsulation::encapsulationReadMessage(uint8_t * pData, const uint32_t & c_unLength) {

	uint8_t szEncapsulationHeader[1024];
	uint8_t szCommandSpecificData[1024];
	uint8_t szCommonIndustrialProtocol[1024];
	uint8_t szCIPClassGeneric[1024];
	uint32_t len1, len2, len3, len4, len;

	len1 = len2 = len3 = len4 = len = 0;

	setEHCommond(EIP_SENDUNITDATA_CMD);
	setCIPServiceCode(CIP_READ_CODE);

	if ((len1 = encapsulationReadCIPClassGeneric(szCIPClassGeneric, sizeof(szCIPClassGeneric))) < 0) {
		return -1;
	}
	if ((len2 = encapsulationReadCommonIndustrialProtocol(szCommonIndustrialProtocol, sizeof(szCommonIndustrialProtocol))) < 0) {
		return -2;
	}
	if ((len3 = encapsulationConnectCommandSpecificData(szCommandSpecificData, sizeof(szCommandSpecificData))) < 0) {
		return -3;
	}
	if ((len4 = encapsulationEncapsulationHeader(szEncapsulationHeader, sizeof(szEncapsulationHeader))) < 0) {
		return -4;
	}

	if (len + len4 > c_unLength) {
		return -5;
	}
	memcpy(pData, szEncapsulationHeader, len4);
	len += len4;

	if (len + len3 > c_unLength) {
		return -6;
	}
	memcpy(pData + len, szCommandSpecificData, len3);
	len += len3;

	if (len + len2 > c_unLength) {
		return -7;
	}
	memcpy(pData + len, szCommonIndustrialProtocol, len2);
	len += len2;

	if (len + len1 > c_unLength) {
		return -8;
	}
	memcpy(pData + len, szCIPClassGeneric, len1);
	len += len1;
	//std::cout << " read len : " << len1 << " " << len2 << " " << len3 << " " << len4 << "\n";

	return len;
}

/*
int CMsgEncapsulation::encapsulationWriteMessage(uint8_t * pData, const uint32_t & c_unLength) {

	uint8_t szEncapsulationHeader[1024];
	uint8_t szCommandSpecificData[1024];
	uint8_t szCommonIndustrialProtocol[1024];
	uint8_t szCIPClassGeneric[1024];
	uint32_t len1, len2, len3, len4, len;

	len1 = len2 = len3 = len4 = len = 0;

	setEHCommond(EIP_SENDUNITDATA_CMD);
	setCIPServiceCode(CIP_WRITE_CODE);

	if ((len1 = encapsulationWriteCIPClassGeneric(szCIPClassGeneric, sizeof(szCIPClassGeneric))) < 0) {
		return -1;
	}
	if ((len2 = encapsulationWriteCommonIndustrialProtocol(szCommonIndustrialProtocol, sizeof(szCommonIndustrialProtocol))) < 0) {
		return -2;
	}
	if ((len3 = encapsulationConnectCommandSpecificData(szCommandSpecificData, sizeof(szCommandSpecificData))) < 0) {
		return -3;
	}
	if ((len4 = encapsulationEncapsulationHeader(szEncapsulationHeader, sizeof(szEncapsulationHeader))) < 0) {
		return -4;
	}

	if (len + len4 > c_unLength) {
		return -5;
	}
	memcpy(pData, szEncapsulationHeader, len4);
	len += len4;

	if (len + len3 > c_unLength) {
		return -6;
	}
	memcpy(pData + len, szCommandSpecificData, len3);
	len += len3;

	if (len + len2 > c_unLength) {
		return -7;
	}
	memcpy(pData + len, szCommonIndustrialProtocol, len2);
	len += len2;

	if (len + len1 > c_unLength) {
		return -8;
	}
	memcpy(pData + len, szCIPClassGeneric, len1);
	len += len1;

	//std::cout << " read len : " << len1 << " " << len2 << " " << len3 << " " << len4 << "\n";
	return len;
}
*/
int CMsgEncapsulation::unPackReadResponse(uint8_t * pDate, const uint32_t & c_unLength) {

	if (pDate[25] != 0) {
		return -1;
	}

	return pDate[c_unLength - 2] | pDate[c_unLength - 1] << 8;
}


