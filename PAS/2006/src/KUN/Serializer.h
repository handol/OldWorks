#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__

#include "Archive.h"

class Serializer
{
public:
	virtual ~Serializer() {};
	virtual int serialize(Archive& ar) const = 0;
	virtual int unserialize(Archive& ar) = 0;
};

#endif
