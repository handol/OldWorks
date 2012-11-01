-- HashKey --
hashkey 생성을 위해서 CreateHashKey함수를 사용한다.
hashkey를 생성할 때 생성힌트용 문자열은 client가 cp에게 요청하는 raw url을 사용한다.
"raw url"이라 함은 hot number나 acl을 이용해 수정하기 이전의 url을 말한다.


-- Bill Info --
CreateBillInfoKey() 함수를 생성하여 key를 생성한다.
key는 10자리의 sequence 한 값을 생성한다.
CreateBillInfoKey() 함수는 매우 낮은 확률로 같은 key 값이 생성되는 문제점이 있다.

-- 파라미터 명명 --
[subfix "_"]
클래스 멤버변수로 port 라는 변수가 있고, 멤버함수의 파라미터로 port 라는 변수를 받아들여야 할때,
멤버함수 파라미터인 port 가 멤버변수 port의 scope 를 가리는 문제를 우회하기 위해
xxxx_ 과 같은 _ 이라는 subfix, 예를 들어 port_ 과 같은 파라미터 인자를 사용하며, scope를 가리지 않는
경우에는 _ 라는 subfix를 붙이지 않는다.

[subfix _HandOver]
함수호출중에는 외부에서 Heap에 동적할당하고 함수 내에서 삭제가 발생하는 경우가 있다.
위와 같이 호출된 함수가 파라미터로 Heap에 대한 포인터를 받아서 내부적으로 알아서 삭제를 할 때, _HandOver를 파라미터명에 붙인다.
즉, sendData_HandOver와 같이 xxxxx_HandOver 라는 명명법을 사용한다.


-- msgwatch, mwatch --
CommonSocket.cpp CommonSocket.h mwatchcli.cpp mwatchcli.h 화일 관련 업체에서 제공된 소스이다.
msgwatch.h 는 아래와 같이 생성되는 화일이다.
cat CommonSocket.h mwatchcli.h > msgwatch.h


