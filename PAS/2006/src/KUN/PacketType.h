#ifndef __PACKET_TYPE_H__
#define __PACKET_TYPE_H__

enum PacketType
{
	PT_NONE = 0,

	PT_SessionReadRequest = 11,
	PT_SessionReadResponse = 12,
	PT_SessionWriteRequest = 13,
	PT_SessionWriteResponse = 14,

	PT_ServiceReadRequest = 21,
	PT_ServiceReadResponse = 22,
	PT_ServiceWriteRequest = 23,
	PT_ServiceWriteResponse = 24,

	PT_GuideReadRequest = 31,
	PT_GuideReadResponse = 32,
	PT_GuideWriteRequest = 33,
	PT_GuideWriteResponse = 34,

	PT_HealthCheckRequest = 91,
	PT_HealthCheckResponse = 92
};

#endif
