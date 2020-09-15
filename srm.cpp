#include <stdio.h>
#include <Windows.h>
#include <psapi.h> 
#include <sysinfoapi.h>
#include <processthreadsapi.h>

double Get_CPU_usage();
DWORD subtract_times(FILETIME prev, FILETIME now);

int main()
{
	/*��ġ������ : ctrl+shift+esc*/

	/*���� �ð� ������� ���*/
	SYSTEM_INFO info;
	SYSTEMTIME local_time;

	/*���μ��� ���� ������� ���*/
	DWORD aProcesses[1024], cbNeeded, Cprocesses;
	/*aProcesses : ���μ��� ���̵� ����Ʈ, cbNeeded : ��ȯ���� ���μ��� ���̵� ����Ʈ�� �ʿ�� �ϴ� ������ ����Ʈ ��, ��ȯ��, Cprocesses : ���μ����� ����
	�������� process�� ������ ���� ���� ���*/

	/*�޸� ��뷮 ���ϱ� ���� ���*/
	MEMORYSTATUSEX memStatus = { sizeof memStatus };
	DWORDLONG total_mem, avail_mem;

	/*cpu ����� ����� ���ϱ� ���� �迭 �� ����*/
	double cpu_load_5[5] = { 0 };
	double cpu_load_10[10] = { 0 };
	double cpu_load_15[15] = { 0 };
	double cpu_usage;
	double cpu_avg;

	int count = 0; /*index*/

	GetSystemInfo(&info);

	printf("Number of CPUs : %d\n", info.dwNumberOfProcessors);
	/*dwNumberOfProcessors : ��ȿ�� CPU����(�ӽſ� ��ġ�� CPU �ھ� ����)*/

	printf("Index\t\tTime\t\tProcesses\tMemoryLoad\tCPU Load\n");

	while (1)
	{
		/*�ε���*/
		printf("%4d\t", count);

		/*���� �ð� ���ϱ�(�ѱ��ð�)*/
		GetLocalTime(&local_time);

		printf("%04d_%02d_%02d %02d_%02d_%02d :", local_time.wYear, local_time.wMonth, local_time.wDay, local_time.wHour, local_time.wMinute, local_time.wSecond);

		/*���μ��� ���� ���ϱ�*/
		EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded);
		Cprocesses = cbNeeded / sizeof(DWORD);

		printf("\t%6d\t", Cprocesses);

		/*�޸� ��뷮 ���ϱ�(memory load)*/
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
	/*5�� ���� ���� ��� �� ���� cpu ��뷮���� �Ѵ�.*/

	cpu_usage_all = 0X00f; /*5�� �� ���� ���ϱ� ���� ����(sum)*/

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
�ð� ���� �Լ� �� ����ü)
GetSystemTime(LPSYSTEMTIME IpSystemTime) -> UTC �ð�(�����ð�) => ���� ������
GetLocalTime(") -> UTC�ð��� ���� �ý��ۿ� ������ �ð���� �ϱ�����ð� ���� ������ ������ �ð����� �ٲ� �� => �̰� ���

SYSTEMTIME�� ����ü�� ��� wHour, wMinute,wSecond ��� ����� ����
http://blog.naver.com/PostView.nhn?blogId=tull00&logNo=10143106559
*/

/*
EnumProcesses() -> header : psapi.h

https://pgcck.tistory.com/76

�Լ��� �̿��ؼ� ���μ����� �̸��� ����Ʈ���� ���ϰ�,
����Ʈ�� ������ �� ���μ����� �����̹Ƿ� �Լ����� ����� ������ �̿��ؼ� ���μ��� ������ ����.

*/

/*
GlobalMemoryStatusEx()

MEMORYSTATUSEX����ü ���
CString ���  -> header : atlstr.h
* char*, string, CString����
-String : �پ� �Լ� ����X
-CString : ���� O /������
�� ������

typedef struct _MEMORYSTATUS {
DWORD dwLength;                //����ü ũ��
DWORD dwMemoryLoad;            //�޸� ����
SIZE_T dwTotalPhys;           //��ü �޸�
SIZE_T dwAvailPhys;       //����� �޸�
SIZE_T dwTotalPageFile;
SIZE_T dwAvailPageFile;
SIZE_T dwTotalVirtual;      //�Ѱ���޸�
SIZE_T dwAvailVirtual;     //����� ���� �޸�
} MEMORYSTATUS, *LPMEMORYSTATUS;

https://pgcck.tistory.com/category/%ED%94%84%EB%A1%9C%EA%B7%B8%EB%9E%98%EB%B0%8D/API
https://3dmpengines.tistory.com/1449
http://blog.naver.com/PostView.nhn?blogId=kilsu1024&logNo=220225779284&categoryNo=0&parentCategoryNo=14&viewDate=&currentPage=1&postListTopCurrentPage=1&from=postView
*/

/*
CPU load(����)

* cpu load = (OS + user)/(OS+user+idle(���½ð�)) * 100
= (kernel+user-idle)/(kernel+user) * 100
(kernel = OS+idle)

GetSystemtimes()  ->header = <processthreadsapi.h>

FILETIME ����ü ���
typedef struct _FILETIME {
DWORD dwLowDateTime;
DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;
���⼭ low,high���� ���� ��� ���鶧���� 64��Ʈ ������ ������ ������
32��Ʈ+32��Ʈ �ؼ� 64��Ʈ ������� ��������.
http://blog.naver.com/PostView.nhn?blogId=tull00&logNo=10143106559

BOOL GetSystemTimes(
PFILETIME lpIdleTime,
PFILETIME lpKernelTime,
PFILETIME lpUserTime
);

cpu ������ ���Ҷ�, ���� �����ϰ� ���ϴ� ����� 100m sec �������� 5�� ���Ļ���� ���ؼ� ��ճ��� ���̴�.
https://sec.tistory.com/entry/cpu-%EC%82%AC%EC%9A%A9%EB%A5%A0-%EA%B5%AC%ED%95%98%EA%B8%B0%ED%8E%8C

getsystemtimes �Լ��� ����ؼ� cpu���� ���ϴ� �ڵ�
https://www.sysnet.pe.kr/Default.aspx?mode=2&sub=0&detail=1&pageno=0&wid=1684&rssMode=1&wtype=0

LARGE_INTEGER��� ����ü�� ����ؼ� HIGH�� LOW��ġ�� �� �ִ�.(�� �Ⱦ�)
http://blog.naver.com/PostView.nhn?blogId=seokcrew&logNo=30152375741
*/