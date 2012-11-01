#ifndef __ARCHIVE_H__
#define __ARCHIVE_H__

#include "Common.h"

#define MAX_RAW_PACKET_SIZE 4096

class Archive
{
public:
	Archive();
	Archive(const char* srcBuf, int size);

	~Archive();

	/// 데이터 쓰기
	/**
	 * 버퍼의 뒤에 데이터를 추가한다.
	 * 기록하는 데이터의 사이즈는 제한이 없다.
	 **/	
	int push(const char* srcBuf, int srcSize);
	int push(char ch);

	/// 데이터 읽기
	/**
	 * 버퍼의 앞에서부터 데이터를 읽어 온다.
	 *
	 * @return destBuf 에 기록된 데이터 사이즈
	 **/
	int pop(char* destBuf, int popSize);

	/// 데이터 읽기
	/**
	* 버퍼의 앞에서부터 token 이 있는 곳까지 데이터를 읽어 온다.
	* token 문자는 destBuf 에 기록되지 않는다.
	* 
	* @return destBuf 에 기록된 데이터 사이즈
	**/
	int popFirstOf(char* destBuf, int bufSize, char token);

	/// 데이터 셋팅
	/**
	 * 이전에 기록된 데이터를 삭제해고, 새로운 데이터로 덮어 쓴다.
	 **/
	void set(const char* srcBuf, int size);

	/// 데이터 읽기
	/**
	 * 저장된 데이터 사이즈가 bufSize 보다 크다면, bufSize 까지만 읽어 온다.
	 *
	 * @param destBuf 기록할 버퍼
	 * @param bufSize destBuf 의 사이즈
	 * @return destBuf 에 기록한 데이터 사이즈
	 **/
	int get(char* destBuf, int bufSize);

	/// 데이터의 시작 포인터
	/**
	 * @return 데이터의 시작 포인터, empty 일 경우 NULL
	 *
	 * @date 2007/02/22
	 * @author Sehoon Yang
	 **/
	char* front();
	const char* front() const;

	/// 데이터 삭제
	void clear();

	/// 데이터 사이즈
	int size() const;

	/// 버퍼 사이즈를 읽어 온다.
	int getMaxSize();

	/// 버퍼 사이즈를 셋팅한다.
	void setMaxSize(int size);

	/// 읽은 데이터 사이즈
	int readSize();

	/// read index 값 읽어 오기
	int getReadIdx();

	/// read index 값 셋팅 하기
	void setReadIdx(int idx);

	/// write index 값 읽어 오기
	int getWriteIdx();

	/// write index 값 셋팅 하기
	void setWriteIdx(int idx);

	/// 남은 공간
	int space();

private:
	void init();

private:
	char _fixedBuf[MAX_RAW_PACKET_SIZE];
	char* _buf;
	int _bufSize;
	int _readIdx;
	int _writeIdx;
};

#endif
