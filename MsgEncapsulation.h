#ifndef MESSAGE_ENCAPSULATION_H__20221123
#define MESSAGE_ENCAPSULATION_H__20221123

#include "GlobalTool.h"

class CMsgEncapsulation {

public:

	CMsgEncapsulation();
	virtual ~CMsgEncapsulation();

	/* set */
	void setEHCommond(const uint16_t& c_usCommond);
	void setEHLength(const uint16_t& c_usLength);
	void setEHSessionHandle(const uint32_t& c_unHandle);

	void setCSDConnectID(const uint32_t& c_unID);
	void setCSDDataLength(const uint16_t& c_usLength);

	void setCIPServiceCode(const uint8_t& c_ucCode);

	void addPath(const uint16_t& c_usPath);
	void initPath();

	void setConnectionSerialNumber(const uint16_t& c_usIp);
	void setOriginatorSerialNumber(const uint16_t& c_usIp);

	void setANSISymbol(const std::string& c_strSymbol);

	/* void setValue(const uint32_t& c_usValue); */
	template<class T>
	void setValue(T c_usValue);

	void setDataType(const uint16_t& c_usDataType);

	void setOptionCount(const uint16_t& c_unOptCount);
	void setStartIndex(const uint8_t& c_ucStartIndex);

	/* encapsulation request message */
	int encapsulationRegisterMessage(uint8_t* pData, const uint32_t& c_unLength);
	int encapsulationCloseMessage(uint8_t* pData, const uint32_t& c_unLength);
	int encapsulationOpenMessage(uint8_t* pData, const uint32_t& c_unLength);
	int encapsulationReadMessage(uint8_t* pData, const uint32_t& c_unLength);
	//int encapsulationWriteMessage(uint8_t* pData, const uint32_t& c_unLength);

	template<class T>
	int encapsulationWriteMessage(uint8_t* pData, const uint32_t& c_unLength);

	/* Unpack, Suspend temporarily */
	int unPackReadResponse(uint8_t* pDate, const uint32_t& c_unLength);

protected:
	/* encapsulation message segment */
	int encapsulationEncapsulationHeader(uint8_t* pData, const uint32_t& c_unLength);
	int encapsulationUnConnectCommandSpecificData(uint8_t* pData, const uint32_t& c_unLength);
	int encapsulationCommonIndustrialProtocol(uint8_t* pData, const uint32_t& c_unLength);
	int encapsulationCIPConnectionManager(uint8_t* pData, const uint32_t& c_unLength);
	int encapsulationConnectCommandSpecificData(uint8_t* pData, const uint32_t& c_unLength);

	/* read */
	int encapsulationReadCommonIndustrialProtocol(uint8_t* pData, const uint32_t& c_unLength);
	int encapsulationReadCIPClassGeneric(uint8_t* pData, const uint32_t& c_unLength);

	/* write */
	//int encapsulationWriteCIPClassGeneric(uint8_t* pData, const uint32_t& c_unLength);
	//int encapsulationWriteCommonIndustrialProtocol(uint8_t* pData, const uint32_t& c_unLength);

	template<class T>
	int encapsulationWriteCIPClassGeneric(uint8_t* pData, const uint32_t& c_unLength);
	template<class T>
	int encapsulationWriteCommonIndustrialProtocol(uint8_t* pData, const uint32_t& c_unLength);

	template<class T>
	void setValue_aux(T c_usValue, std::true_type);

	template<class T>
	void setValue_aux(T c_usValue, std::false_type);

	template<class T>
	int encapsulationWriteCIPClassGeneric_aux(uint8_t* pData, const uint32_t& c_unLength, std::true_type);

	template<class T>
	int encapsulationWriteCIPClassGeneric_aux(uint8_t* pData, const uint32_t& c_unLength, std::false_type);

private:

	/* Encapsulation Header */
	uint16_t m_usEHCommond;
	uint16_t m_usEHLength;
	uint32_t m_unEHSessionHandle;

	/* Command Specific Data */
	/* address item */
	uint32_t m_unCSDConnectID;
	/* data item */
	uint16_t m_usCSDDataLength;

	/* open Common Industrial Protocol */
	uint8_t m_ucCIPServiceCode;
	std::string m_strCIPPathSegment;

	/* Command Specific Data */
	uint16_t m_usConnectionSerialNumber;
	uint32_t m_usOriginatorSerialNumber;

	/* read Common Industrial Protocol */
	std::string m_strANSISymbol;
	//uint32_t m_usValue;
	std::string m_strValue;
	uint16_t m_usDataType;

	uint16_t m_usOptionCount;
	uint8_t m_ucStartIndex;
};

template<class T>
inline void CMsgEncapsulation::setValue(T c_usValue) {

	this->m_strValue.clear();
	setValue_aux(c_usValue, typename std::is_pointer<T>::type());
}


template<class T>
inline void CMsgEncapsulation::setValue_aux(T c_usValue, std::true_type) {

	std::cout << sizeof(c_usValue[0])*m_usOptionCount << "\n";
	this->m_strValue.append((char*)(c_usValue), sizeof(c_usValue[0])*m_usOptionCount);
}

template<class T>
inline void CMsgEncapsulation::setValue_aux(T c_usValue, std::false_type) {

	if (std::is_integral<T>::value == true) {
		this->m_strValue.append((char*)(&c_usValue), sizeof(T));
	}
	else {
		this->m_strValue = c_usValue;
	}

}

template<class T>
inline int CMsgEncapsulation::encapsulationWriteCIPClassGeneric_aux(uint8_t * pData, const uint32_t & c_unLength, std::false_type) {

	return 0;
}

template<class T>
inline int CMsgEncapsulation::encapsulationWriteCIPClassGeneric_aux(uint8_t * pData, const uint32_t & c_unLength, std::true_type) {

	return 0;
}


template<class T>
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

	if (std::is_integral<T>::value == false && std::is_pointer<T>::value == false) {

		/* is string */
		uint16_t usLength = this->m_strValue.length();
		std::cout << "is tsring: " << this->m_strValue << "\n";
		if (len + sizeof(usLength) > c_unLength) {
			return -3;
		}

		memcpy(pData + len, &usLength, sizeof(usLength));
		len += sizeof(usLength);

	}

	//T value = static_cast<T>(this->m_pValue);

	if (len + m_strValue.length() > c_unLength) {
		return -4;
	}

	//std::cout << value[5] << "\n";

	memcpy(pData + len, m_strValue.c_str(), m_strValue.length());
	len += m_strValue.length();

	setCSDDataLength(len);
	std::cout << m_usCSDDataLength << "\n";
	return len;
}

template<class T>
int CMsgEncapsulation::encapsulationWriteCommonIndustrialProtocol(uint8_t * pData, const uint32_t & c_unLength) {

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

	if (std::is_pointer<T>::value == true) {

		/* is array */
		uint8_t code = 0x28;

		memcpy(pData + unLength, &code, 1);
		unLength++;
		memcpy(pData + unLength, &m_ucStartIndex, 1);
		unLength++;
		readRequest.ucPathSize++;
	}

	setCSDDataLength(m_usCSDDataLength + unLength);
	std::cout << m_usCSDDataLength << "\n";
	return unLength;
}

template<class T>
inline int CMsgEncapsulation::encapsulationWriteMessage(uint8_t * pData, const uint32_t & c_unLength) {

	uint8_t szEncapsulationHeader[1024];
	uint8_t szCommandSpecificData[1024];
	uint8_t szCommonIndustrialProtocol[1024];
	uint8_t szCIPClassGeneric[1024];
	uint32_t len1, len2, len3, len4, len;

	len1 = len2 = len3 = len4 = len = 0;

	setEHCommond(EIP_SENDUNITDATA_CMD);
	setCIPServiceCode(CIP_WRITE_CODE);

	if ((len1 = encapsulationWriteCIPClassGeneric<T>(szCIPClassGeneric, sizeof(szCIPClassGeneric))) < 0) {
		return -1;
	}
	if ((len2 = encapsulationWriteCommonIndustrialProtocol<T>(szCommonIndustrialProtocol, sizeof(szCommonIndustrialProtocol))) < 0) {
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

#endif /* MESSAGE_ENCAPSULATION_H__20221123 */