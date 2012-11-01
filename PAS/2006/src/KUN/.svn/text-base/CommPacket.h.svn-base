#ifndef __COMM_PACKET_H__
#define __COMM_PACKET_H__

#include "PacketType.h"
#include "AsciiSerializer.h"
#include "Archive.h"
#include "MDN.h"
#include "GuideCode.h"

class CommPacket : public AsciiSerializer
{
public:
	using AsciiSerializer::push;
	using AsciiSerializer::pop;

	CommPacket() : AsciiSerializer('|'), mdn(static_cast<int64_t>(0)), packetType(PT_NONE)
	{
	}

	virtual int serialize(Archive& ar) const;
	virtual int unserialize(Archive& ar);

	PacketType getPacketType() const;

protected:
	void push(Archive& ar, const PacketType& value) const;
	void push(Archive& ar, const GuideCode& value) const;
	void push(Archive& ar, const MDN& value) const;
	void push(Archive& ar, const ip_t& value) const;	
	void push(Archive& ar, const url_t& value) const;
	
	void pop(Archive& ar, PacketType& value);
	void pop(Archive& ar, GuideCode& value);
	void pop(Archive& ar, MDN& value);
	void pop(Archive& ar, ip_t& value);
	void pop(Archive& ar, url_t& value);

	void pushSpliter(Archive& ar) const;

public:
	MDN mdn;

protected:
	PacketType packetType;
};

#endif
