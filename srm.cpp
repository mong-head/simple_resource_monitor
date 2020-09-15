#include <stdio.h>
#include <Windows.h>
#include <psapi.h> 
#include <sysinfoapi.h>
#include <processthreadsapi.h>

double Get_CPU_usage();
DWORD subtract_times(FILETIME prev, FILETIME now);

int main()
{
	/*장치관리자 : ctrl+shift+esc*/

	/*현재 시간 출력위해 사용*/
	SYSTEM_INFO info;
	SYSTEMTIME local_time;

	/*프로세스 개수 출력위해 사용*/
	DWORD aProcesses[1024], cbNeeded, Cprocesses;
	/*aProcesses : 프로세스 아이디 리스트, cbNeeded : 반환받을 프로세스 아이디 리스트가 필요로 하는 버퍼의 바이트 수, 반환값, Cprocesses : 프로세스의 개수
	실행중인 process의 개수를 세기 위해 사용*/

	/*메모리 사용량 구하기 위해 사용*/
	MEMORYSTATUSEX memStatus = { sizeof memStatus };
	DWORDLONG total_mem, avail_mem;

	/*cpu 사용율 평균을 구하기 위한 배열 및 변수*/
	double cpu_load_5[5] = { 0 };
	double cpu_load_10[10] = { 0 };
	double cpu_load_15[15] = { 0 };
	double cpu_usage;
	double cpu_avg;

	int count = 0; /*index*/

	GetSystemInfo(&info);

	printf("Number of CPUs : %d\n", info.dwNumberOfProcessors);
	/*dwNumberOfProcessors : 유효한 CPU개수(머신에 설치된 CPU 코어 개수)*/

	printf("Index\t\tTime\t\tProcesses\tMemoryLoad\tCPU Load\n");

	while (1)
	{
		/*인덱스*/
		printf("%4d\t", count);

		/*현재 시간 구하기(한국시간)*/
		GetLocalTime(&local_time);

		printf("%04d_%02d_%02d %02d_%02d_%02d :", local_time.wYear, local_time.wMonth, local_time.wDay, local_time.wHour, local_time.wMinute, local_time.wSecond);

		/*프로세스 개수 구하기*/
		EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded);
		Cprocesses = cbNeeded / sizeof(DWORD);

		printf("\t%6d\t", Cprocesses);

		/*메모리 사용량 구하기(memory load)*/
		memStatus.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memStatus);

		printf("\t%5ld%%\t", memStatus.dwMemoryLoad);

		/*CPU Load*/
		printf("\t[CPU Load: %5.2lf]", cpu_usage = Get_CPU_usage());

		cpu_load_15[count % 15] = cpu_load_10[count % 10] = cpu_load_5[count % 5] = cpu_usage;

		if (count >= 5)
		{
			cpu_avg = 0.0;
			for (int i = 0; i < 5; i++) cpu_avg += cpu_load_5[i];
			printf("[5sec avg : %5.2lf]", cpu_avg / 5);
			if (count >= 10)
			{
				cpu_avg = 0.0;
				for (int i = 0; i < 10; i++) cpu_avg += cpu_load_10[i];
				printf("[10sec avg : %5.2lf]", cpu_avg / 10);
				if (count >= 15)
				{
					cpu_avg = 0.0;
					for (int i = 0; i < 15; i++) cpu_avg += cpu_load_15[i];
					printf("[15sec avg : %5.2lf]", cpu_avg / 15);
				}
			}
		}
		Sleep(400);
		printf("\n");
		count++;
	}
	return 0;
}
double Get_CPU_usage()
{
	FILETIME idle_time, kernel_time, user_time, prev_idle_time, prev_kernel_time, prev_user_time;
	DWORD idle, kernel, user, work;
	double cpu_usage_all;
	/*work = kernel + user*/
	/*5번 구한 것을 평균 낸 것을 cpu 사용량으로 한다.*/

	cpu_usage_all = 0X00f; /*5번 한 것을 더하기 위한 변수(sum)*/

	GetSystemTimes(&prev_idle_time, &prev_kernel_time, &prev_user_time);
	Sleep(100);
	for (int i = 0; i < 5; i++)
	{
		GetSystemTimes(&idle_time, &kernel_time, &user_time);

		idle = subtract_times(prev_idle_time, idle_time);
		kernel = subtract_times(prev_kernel_time, kernel_time);
		user = subtract_times(prev_user_time, user_time);
		work = kernel + user;

		cpu_usage_all = (work - idle) * 100 / (double)work;

		prev_idle_time = idle_time;
		prev_user_time = user_time;
		prev_kernel_time = kernel_time;

		Sleep(100);
	}
	//return (cpu_usage_all / 5.0);
	return cpu_usage_all;
}
DWORD subtract_times(FILETIME prev, FILETIME now)
{
	DWORD now_t = ((DWORDLONG)now.dwHighDateTime << 32 | now.dwLowDateTime);
	DWORD prev_t = ((DWORDLONG)prev.dwHighDateTime << 32 | prev.dwLowDateTime);

	return now_t - prev_t;
}
/*
시간 관련 함수 및 구조체)
GetSystemTime(LPSYSTEMTIME IpSystemTime) -> UTC 시간(영국시간) => 거의 사용안함
GetLocalTime(") -> UTC시간에 현재 시스템에 설정된 시간대와 일광절약시간 설정 참고해 현지의 시간으로 바꾼 것 => 이것 사용

SYSTEMTIME의 구조체의 멤버 wHour, wMinute,wSecond 등등 사용할 것임
http://blog.naver.com/PostView.nhn?blogId=tull00&logNo=10143106559
*/

/*
EnumProcesses() -> header : psapi.h

https://pgcck.tistory.com/76

함수를 이용해서 프로세스의 이름의 리스트들을 구하고,
리스트의 개수가 곧 프로세스의 개수이므로 함수에서 저장된 값들을 이용해서 프로세스 개수를 구함.

*/

/*
GlobalMemoryStatusEx()

MEMORYSTATUSEX구조체 사용
CString 사용  -> header : atlstr.h
* char*, string, CString차이
-String : 다양 함수 지원X
-CString : 지원 O /느리다
걍 사용안함

typedef struct _MEMORYSTATUS {
DWORD dwLength;                //구조체 크기
DWORD dwMemoryLoad;            //메모리 사용률
SIZE_T dwTotalPhys;           //전체 메모리
SIZE_T dwAvailPhys;       //사용한 메모리
SIZE_T dwTotalPageFile;
SIZE_T dwAvailPageFile;
SIZE_T dwTotalVirtual;      //총가상메모리
SIZE_T dwAvailVirtual;     //사용한 가상 메모리
} MEMORYSTATUS, *LPMEMORYSTATUS;

https://pgcck.tistory.com/category/%ED%94%84%EB%A1%9C%EA%B7%B8%EB%9E%98%EB%B0%8D/API
https://3dmpengines.tistory.com/1449
http://blog.naver.com/PostView.nhn?blogId=kilsu1024&logNo=220225779284&categoryNo=0&parentCategoryNo=14&viewDate=&currentPage=1&postListTopCurrentPage=1&from=postView
*/

/*
CPU load(사용률)

* cpu load = (OS + user)/(OS+user+idle(쉬는시간)) * 100
= (kernel+user-idle)/(kernel+user) * 100
(kernel = OS+idle)

GetSystemtimes()  ->header = <processthreadsapi.h>

FILETIME 구조체 사용
typedef struct _FILETIME {
DWORD dwLowDateTime;
DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;
여기서 low,high나눈 것은 당시 만들때에는 64비트 정수가 없었기 때문임
32비트+32비트 해서 64비트 만들려고 나누었다.
http://blog.naver.com/PostView.nhn?blogId=tull00&logNo=10143106559

BOOL GetSystemTimes(
PFILETIME lpIdleTime,
PFILETIME lpKernelTime,
PFILETIME lpUserTime
);

cpu 사용률을 구할때, 가장 근접하게 구하는 방법은 100m sec 간격으로 5번 공식사용해 구해서 평균내는 것이다.
https://sec.tistory.com/entry/cpu-%EC%82%AC%EC%9A%A9%EB%A5%A0-%EA%B5%AC%ED%95%98%EA%B8%B0%ED%8E%8C

getsystemtimes 함수를 사용해서 cpu사용률 구하는 코드
https://www.sysnet.pe.kr/Default.aspx?mode=2&sub=0&detail=1&pageno=0&wid=1684&rssMode=1&wtype=0

LARGE_INTEGER라는 구조체를 사용해서 HIGH랑 LOW합치는 게 있다.(난 안씀)
http://blog.naver.com/PostView.nhn?blogId=seokcrew&logNo=30152375741
*/