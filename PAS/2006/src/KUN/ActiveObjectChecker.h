#ifndef __ACTIVE_OBJECT_CHECKER_H__
#define __ACTIVE_OBJECT_CHECKER_H__

class ActiveObjectChecker
{
public:
	ActiveObjectChecker()
	{
		_activeObject = true;
	}

	virtual ~ActiveObjectChecker()
	{
		_activeObject = false;
	}

	bool valid()
	{
		return _activeObject;
	}

private:
	bool _activeObject; // 현재 object가 정상적으로 메모리에 할당된 object 인지를 기록하는 flag
};

#endif
