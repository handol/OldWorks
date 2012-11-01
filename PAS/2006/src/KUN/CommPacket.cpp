#include "Exception.h"
#include "CommPacket.h"
#include "MDN.h"
#include "PasLog.h"

PacketType CommPacket::getPacketType() const
{
	return packetType;
}

int CommPacket::serialize(Archive& ar) const
{
	push(ar, packetType);

	pushSpliter(ar);	
	push(ar, mdn);
	
	return 0;
}

int CommPacket::unserialize(Archive& ar)
{
	// 기존 패킷타입이 없을 경우, 그냥 ar 로 부터 읽어 들인다.
	if(packetType == PT_NONE)
	{
		pop(ar, packetType);
	}

	// 기존 패킷타입과 ar로 부터 읽어 들인 타입이 서로 틀리면 예외발생
	else
	{
		PacketType newType;
		pop(ar, newType);

		if(newType != packetType)
		{
			PAS_NOTICE2("Packet Type miss match, old type is %d, new type is %d", packetType, newType);
			ThrowException(ECODE_WRONG_DATA);
		}
	}
	pop(ar, mdn);

	return 0;
}

void CommPacket::push(Archive& ar, const PacketType& value) const
{
	TinyString str(static_cast<const int>(value));
	ar.push(str.toStr(), str.size());
}

void CommPacket::push(Archive& ar, const GuideCode& value) const
{
	TinyString str(static_cast<const int>(value));
	ar.push(str.toStr(), str.size());
}

void CommPacket::push(Archive& ar, const MDN& value) const
{
	TinyString str(value.toString());
	ar.push(str.toStr(), str.size());
}

void CommPacket::push(Archive& ar, const ip_t& value) const
{
	ar.push(value.toStr(), value.size());
}

void CommPacket::push(Archive& ar, const url_t& value) const
{
	ar.push(value.toStr(), value.size());
}

void CommPacket::pop(Archive& ar, PacketType& value)
{
	char buf[1024];
	int readSize;

	readSize = ar.popFirstOf(buf, sizeof(buf), _spliter);
	
	if(readSize <= 0)
		ThrowException(ECODE_NO_DATA);
	
	if(readSize == sizeof(buf))
		ThrowException(ECODE_NOT_ENOUGH_SPACE);

	buf[readSize] = '\0';
	value = static_cast<PacketType>(atoi(buf));
}

void CommPacket::pop(Archive& ar, GuideCode& value)
{
	char buf[1024];
	int readSize;

	readSize = ar.popFirstOf(buf, sizeof(buf), _spliter);
	
	if(readSize <= 0)
		ThrowException(ECODE_NO_DATA);

	if(readSize == sizeof(buf))
		ThrowException(ECODE_NOT_ENOUGH_SPACE);

	buf[readSize] = '\0';
	int guideCode = atoi(buf);
	bool isValid = false;
	if(guideCode == GCODE_Unknown)
	{
		isValid = true;
	}
	else
	{
		for (int i = 1; i < GCODE_MAX; i <<= 1)
		{
			if (guideCode == i )
			{
				isValid = true;
				break;
			}
		}
	}

	if (isValid == true)
	{
		value = static_cast<GuideCode>(atoi(buf));
	}
	else
	{
		ThrowException(ECODE_WRONG_DATA);
	}
}

void CommPacket::pop(Archive& ar, MDN& value)
{
	char buf[1024];
	int readSize;

	readSize = ar.popFirstOf(buf, sizeof(buf), _spliter);
	
	if(readSize <= 0)
		ThrowException(ECODE_NO_DATA);

	if(readSize == sizeof(buf))
		ThrowException(ECODE_NOT_ENOUGH_SPACE);

	buf[readSize] = '\0';
	value = buf;
}

void CommPacket::pop(Archive& ar, ip_t& value)
{
	char buf[1024];
	int readSize;

	readSize = ar.popFirstOf(buf, sizeof(buf), _spliter);
	
	if(readSize <= 0)
		ThrowException(ECODE_NO_DATA);

	if(readSize == sizeof(buf))
		ThrowException(ECODE_NOT_ENOUGH_SPACE);

	buf[readSize] = '\0';
	value = buf;
}

void CommPacket::pop(Archive& ar, url_t& value)
{
	char buf[1024];
	int readSize;

	readSize = ar.popFirstOf(buf, sizeof(buf), _spliter);
	
	if(readSize <= 0)
		ThrowException(ECODE_NO_DATA);

	if(readSize == sizeof(buf))
		ThrowException(ECODE_NOT_ENOUGH_SPACE);

	buf[readSize] = '\0';
	value = buf;
}

void CommPacket::pushSpliter(Archive& ar) const
{
	ar.push(_spliter);
}
