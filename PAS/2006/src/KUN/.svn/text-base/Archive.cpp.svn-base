#include "Archive.h"

Archive::Archive()
{
	init();
}

Archive::Archive( const char* srcBuf, int size )
{
	init();

	set(srcBuf, size);
}

Archive::~Archive()
{
	if(_fixedBuf != _buf)
		delete _buf;
}

int Archive::push(const char* srcBuf, int srcSize)
{
	while(space() < srcSize)
		setMaxSize(getMaxSize()*2);
	
	memcpy(&_buf[_writeIdx], srcBuf, srcSize);
	_writeIdx += srcSize;

	return 0;
}

int Archive::push(char ch)
{
	while(space() < sizeof(ch))
		setMaxSize(getMaxSize()*2);

	_buf[_writeIdx] = ch;
	++_writeIdx;

	return 0;
}

int Archive::pop(char* destBuf, int popSize)
{
	int writeIdx = 0;
	
	while(_readIdx < _writeIdx && writeIdx < popSize)
	{
		destBuf[writeIdx] = _buf[_readIdx];
		_readIdx++;
		writeIdx++;
	}

	return writeIdx;
}

int Archive::popFirstOf(char* destBuf, int bufSize, char token)
{
	int writeIdx = 0;

	while(_readIdx < _writeIdx && writeIdx < bufSize)
	{
		if(_buf[_readIdx] != token)
		{
			destBuf[writeIdx] = _buf[_readIdx];
			_readIdx++;
			writeIdx++;
		}
		else
		{
			_readIdx++;
			break;
		}		
	}

	return writeIdx;
}

void Archive::set(const char* srcBuf, int size)
{
	clear();
	push(srcBuf, size);
}

int Archive::get(char* destBuf, int bufSize)
{
	if(bufSize <= 0)
		return 0;

	if(size() == 0)
		return 0;

	int copySize = std::min(bufSize, size());

	memcpy(destBuf, &_buf[_readIdx], copySize);

	return copySize;
}

void Archive::clear()
{
	_readIdx = 0;
	_writeIdx = 0;
}

int Archive::size() const
{
	return _writeIdx - _readIdx;
}

int Archive::readSize()
{
	return _readIdx;
}

int Archive::getReadIdx()
{
	return _readIdx;
}

void Archive::setReadIdx(int idx)
{
	_readIdx = idx;
}

int Archive::getWriteIdx()
{
	return _writeIdx;
}

void Archive::setWriteIdx(int idx)
{
	_writeIdx = idx;
}

char* Archive::front()
{
	return &_buf[_readIdx];
}

const char* Archive::front() const
{
	return &_buf[_readIdx];
}

int Archive::getMaxSize()
{
	return _bufSize;
}

void Archive::setMaxSize( int size )
{
	if(_bufSize < size)
	{
		char* newBuf = new char[size];
		memcpy(newBuf, _buf, _writeIdx);

		if(_fixedBuf != _buf)
			delete _buf;

		_buf = newBuf;
		_bufSize = size;
	}
}

void Archive::init()
{
	_readIdx = 0;
	_writeIdx = 0;
	_buf = _fixedBuf;
	_bufSize = sizeof(_fixedBuf)/sizeof(_fixedBuf[0]);
}

int Archive::space()
{
	return _bufSize - _writeIdx;
}

