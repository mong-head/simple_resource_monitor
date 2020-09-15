# simple_resource_monitor (2019 1학기)
운영체제 과제 2 : 장치관리자 프로그램 유사하게 구현 (기능만)

## 사용한 함수 리스트

### window API 함수

1. GetSystemInfo(&SYSTEM_INFO)
   - 현재 컴퓨터 CPU정보 알아 오기 위함. 
	 - SYSTEM_INFO구조체 사용
	 - 구조체의 dwNumberOfprocessors사용해 논리 프로세서 수 구함(CPU 개수)
2. GetLocalTime(&SYSTEMTIME)
	 - 현재 한국 시간 구하기 위해 사용
	 - SYSTEMTIME구조체의 여러 요소 사용
3. EnumProcesses(DWORD, sizeof(DWORD), DWORD)
	 - 현재 실행되는 프로세스 개수 알기 위해 사용 
4. GlobalMeomoryStatusEx(&MEMORYSTATUSEX)
	 - 메모리 사용량 알기 위해 사용
	 - MEMORYSTATUSEX구조체의 deMemoryLoad를 사용해 메모리 사용율 구함
5. GetSystemTimes(&FILETIME,&FILETIME,&FILETIME)
	 - CPU Load구하기 위해 사용함.
	 - (kernel+user-idle)/(kernel+user) * 100 공식의 kernel, user, idle을 알 수 있다.
   
### 내가 만든 함수

1. double Get_CPU_usage()
	 - cpu 사용률을 구하기 위해서 사용
	 - 100ms간격으로 5번 GetSystemTimes를 불러 평균을 구한 값이 cpu_usage
2. subtract_times(FILETIME ,FILETIME) 
	 - 1번째 함수에서 사용하기 위한 함수로 100ms간격인 두 시간을 빼기 위해 사용
	 - filetime변수가 32비트짜리 2개로 이루어져있어 이 두개를 64비트로 만들기 위해 비트연산 사용
   
## test description

![image](https://user-images.githubusercontent.com/52481037/93212282-5b2fb200-f79d-11ea-9cfa-c3d5677244e2.png)

장치관리자(ctrl+shift+esc)와 비교한 스크린 샷

## self evaluation

 * Windows api함수들을 이용하는 과제를 통해 api함수와 그와 관련된 구조체의 요소들을 쓰는 방법을 배웠다. 
   C로 하지 않고, C++로 하면 class로 관련 구조체와 함수를 묶었다면 더 좋았을 것 같다.   
   또한, CPU 사용률을 구하는 공식에 대해서도 공부를 할 수 있었다.
      
 * CPU Load를 구할 때 값을 5번을 나누어 평균을 구했음에도 장치 관리자에 나오는 값과는 1초 순간순간의 값이 좀 달랐다. 
   하지만 5초, 10초 15초 평균을 낸 것은 장치관리자의 사용률과 비슷하게 나왔다.  


