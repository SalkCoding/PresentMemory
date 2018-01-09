#include <Windows.h>
#include <WinBase.h>
#include <CommCtrl.h>
#include <Pdh.h>
#include <assert.h>
#include <dxgi.h>
#include <d3d9.h>
#include <stdio.h>
#include <locale.h>
#include <intrin.h>
#include <VersionHelpers.h>
#include "resource.h"
#pragma once
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"") 
#pragma comment(lib,"Pdh.lib")
#pragma warning(disable:4244)
#pragma warning(disable:4996)
#define FAST 0
#define NORMAL 1
#define SLOW 2
#define WINDOW64 64
#define WINDOW32 32
typedef HRESULT(WINAPI* LPCREATEDXGIFACTORY)(REFIID, void**);
VOID Information();
VOID GetMonitorInformation();
VOID SaveInforMation();
VOID GetGPUName(BOOLEAN Enum);
VOID GetDiskInformation();
VOID Init();
VOID GetCPUUsage();
VOID GetCPUClock();

INT_PTR CALLBACK SimpleDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK IntoDetailDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK PrintDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK EnumMonitorProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
HWND hDlgMain;
HINSTANCE hInst;
DWORD Len;
TCHAR PercentStr[MAX_PATH] = { NULL };

typedef struct tagGPU {
	TCHAR InGPU[MAX_PATH];
	TCHAR OutGPU[MAX_PATH];
	TCHAR DriverVersion[MAX_PATH];
	TCHAR EnumGPU[10][MAX_PATH];
}GPU;
typedef struct tagMonitor {
	INT BitPixels[2];
	INT MonitorNumber;
	TCHAR DeviceName[MAX_PATH];
	TCHAR DeviceName2[MAX_PATH];
}Monitor;
typedef struct tagDisk {
	ULARGE_INTEGER TotalSpace;
	ULARGE_INTEGER TotalFree;
	DOUBLE Reserved1;//C,D����̺� �뷮 ����
	DOUBLE Reserved2;//C,D����̺� �� ���� ����
	DOUBLE Reserved3;//C,D����̺� �� ��뷮
}Disk;
typedef struct tagWindows {
	HKEY hKey;
	DWORD DataType;
	DWORD DataLen;
	TCHAR WindowVersion[MAX_PATH];
	INT WindowBits;
	BOOLEAN Windows10 = FALSE;
}Windows;
typedef struct tagCPU {
	TCHAR CPUName[MAX_PATH];
	INT CPUUsage;
    INT CPUFrequency;
	INT CPUMaxFrequency;
	INT BusFrequency;
}CPU;
typedef struct tagComputerInfo {
	TCHAR ComputerName[MAX_PATH];
	TCHAR UserName[MAX_PATH];
	TCHAR ComputerKinds[MAX_PATH];
	Disk CDiskInfo;
	Disk DDiskInfo;
	Windows Window;
	MEMORYSTATUSEX Memory;
	SYSTEM_INFO System;
	GPU Graphic;
	CPU CenterProcessUnit;
	Monitor MonitorInfo;
	POINT Display;
}ComputerInfo;
ComputerInfo Computer;
TCHAR *Item[] = { L"����(0.3��)",L"����(1��)",L"����(5��)" };
TCHAR *Items[] = { L"�����",L"Ű����",L"�׷��� ī��",L"CPU" };

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance
	, LPSTR lpszCmdParam, int nCmdShow)
{
	hInst = hInstance;
	Computer.Memory.dwLength = sizeof(MEMORYSTATUSEX);
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_SIMPLE), HWND_DESKTOP, SimpleDlgProc);
	return 0;
}

TCHAR MonitorStr[MAX_PATH * 100] = { NULL };
TCHAR Store[MAX_PATH] = { NULL };
int CountMonitor = 1;
BOOL CALLBACK DetailEnumMonitorProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
	MONITORINFOEX Mi;
	Mi.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(hMonitor, &Mi);
	wsprintf(Store, L"%d��° ����� : %s\r\n", CountMonitor, Mi.szDevice);
	lstrcat(MonitorStr, Store);
	CountMonitor++;
	return TRUE;
}

BOOL CALLBACK EnumMonitorProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
	if (Computer.MonitorInfo.MonitorNumber != 2) {//2�������� ����
		MONITORINFOEX Mi;
		Mi.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hMonitor, &Mi);
		Computer.MonitorInfo.BitPixels[Computer.MonitorInfo.MonitorNumber] = GetDeviceCaps((HDC)dwData, BITSPIXEL);
		if(Computer.MonitorInfo.MonitorNumber == 0)
			lstrcpy(Computer.MonitorInfo.DeviceName, Mi.szDevice);
		else
			lstrcpy(Computer.MonitorInfo.DeviceName2, Mi.szDevice);
		Computer.MonitorInfo.MonitorNumber++;
		return TRUE;
	}
	else
		return FALSE;
}


INT_PTR CALLBACK SimpleDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	switch (iMessage) {
	case WM_INITDIALOG:
		GlobalMemoryStatusEx(&Computer.Memory);
		SendDlgItemMessage(hDlg, IDC_SIMPLE_CPUPRESENTPROGRESS, PBM_SETRANGE, NULL, MAKELPARAM(0, 100));
		SendDlgItemMessage(hDlg, IDC_SIMPLE_PRESENTPROGRESS, PBM_SETRANGE, NULL, MAKELPARAM(0, 100));
		SendDlgItemMessage(hDlg, IDC_SIMPLE_AVAILPHYSPROGRESS, PBM_SETRANGE, NULL, MAKELPARAM(0, (Computer.Memory.ullTotalPhys / 1024) / 1024));
		SetTimer(hDlg, 1, 1000, NULL);
		Init();
		return TRUE;
	case WM_TIMER:
		GetCPUUsage();
		GlobalMemoryStatusEx(&Computer.Memory);
		swprintf(PercentStr, L"%d %%", Computer.CenterProcessUnit.CPUUsage);
		SetDlgItemText(hDlg, IDC_SIMPLE_CPUPRESENT, PercentStr);
		SendDlgItemMessage(hDlg, IDC_SIMPLE_CPUPRESENTPROGRESS, PBM_SETPOS, Computer.CenterProcessUnit.CPUUsage, NULL);
		swprintf(PercentStr, L"%d %%", Computer.Memory.dwMemoryLoad);
		SetDlgItemText(hDlg, IDC_SIMPLE_PRESENT, PercentStr);
		SendDlgItemMessage(hDlg, IDC_SIMPLE_PRESENTPROGRESS, PBM_SETPOS, Computer.Memory.dwMemoryLoad, NULL);
		swprintf(PercentStr, L"%.2f GB", (DOUBLE)((Computer.Memory.ullAvailPhys / 1024) / 1024) / 1024);
		SetDlgItemText(hDlg, IDC_SIMPLE_AVAILPHYS, PercentStr);
		SendDlgItemMessage(hDlg, IDC_SIMPLE_AVAILPHYSPROGRESS, PBM_SETPOS, ((Computer.Memory.ullTotalPhys / 1024) / 1024) - ((Computer.Memory.ullAvailPhys / 1024) / 1024), NULL);
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_INTODETAIL:
			KillTimer(hDlg, 1);
			EndDialog(hDlg, TRUE);
			DialogBox(hInst, MAKEINTRESOURCE(IDD_INTODETAIL),HWND_DESKTOP,IntoDetailDlgProc);
			break;
		case IDC_SIMPLE_REFRESH:
			GetCPUUsage();
			GlobalMemoryStatusEx(&Computer.Memory);
			swprintf(PercentStr, L"%d %%", Computer.CenterProcessUnit.CPUUsage);
			SetDlgItemText(hDlgMain, IDC_SIMPLE_CPUPRESENT, PercentStr);
			SendDlgItemMessage(hDlg, IDC_SIMPLE_CPUPRESENTPROGRESS, PBM_SETPOS, Computer.CenterProcessUnit.CPUUsage, NULL);
			swprintf(PercentStr, L"%d %%", Computer.Memory.dwMemoryLoad);
			SetDlgItemText(hDlgMain, IDC_SIMPLE_PRESENT, PercentStr);
			SendDlgItemMessage(hDlg, IDC_SIMPLE_PRESENTPROGRESS, PBM_SETPOS, Computer.Memory.dwMemoryLoad, NULL);
			swprintf(PercentStr, L"%.2f GB", (DOUBLE)((Computer.Memory.ullAvailPhys / 1024) / 1024) / 1024);
			SetDlgItemText(hDlgMain, IDC_SIMPLE_AVAILPHYS, PercentStr);
			SendDlgItemMessage(hDlg, IDC_SIMPLE_AVAILPHYSPROGRESS, PBM_SETPOS, ((Computer.Memory.ullTotalPhys / 1024) / 1024) - ((Computer.Memory.ullAvailPhys / 1024) / 1024), NULL);
			break;
		case IDC_SIMPLE_EXIT:
			SendMessage(hDlg, WM_CLOSE, NULL, NULL);
			break;
		}
		return TRUE;
	case WM_CLOSE:
		KillTimer(hDlg, 1);
		EndDialog(hDlg, TRUE);
		return TRUE;
	}
	return FALSE;
}

int Select = NORMAL;
int ToolSelect = 0;
BOOL Check = FALSE;
BOOL Sound = FALSE;
INT_PTR CALLBACK IntoDetailDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage) {
	case WM_INITDIALOG: {
		hDlgMain = hDlg;
		GetNativeSystemInfo(&Computer.System);
		Init();
		Computer.Window.DataType = REG_SZ;
		Computer.Window.DataLen = MAX_PATH;
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_READ, &Computer.Window.hKey) == ERROR_SUCCESS) {
			RegQueryValueEx(Computer.Window.hKey, TEXT("ProcessorNameString"), NULL, &Computer.Window.DataType, (LPBYTE)Computer.CenterProcessUnit.CPUName, &Computer.Window.DataLen);
			SetDlgItemText(hDlgMain, IDC_CPUINFO, Computer.CenterProcessUnit.CPUName);
			RegCloseKey(Computer.Window.hKey);
		}//Cpu�̸�
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), NULL, KEY_READ, &Computer.Window.hKey) == ERROR_SUCCESS) {
			RegQueryValueEx(Computer.Window.hKey, TEXT("ProductName"), NULL, &Computer.Window.DataType, (LPBYTE)Computer.Window.WindowVersion, &Computer.Window.DataLen);
			SetDlgItemText(hDlgMain, IDC_WINDOWVERSION, Computer.Window.WindowVersion);
			RegCloseKey(Computer.Window.hKey);
			if (wcsstr(Computer.Window.WindowVersion, L"10") != NULL) {
				Computer.Window.Windows10 = TRUE;
			}
		}//������ ����
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("SYSTEM\\HardwareConfig\\Current"), 0, KEY_READ, &Computer.Window.hKey) == ERROR_SUCCESS) {
			RegQueryValueEx(Computer.Window.hKey, TEXT("SystemProductName"), NULL, &Computer.Window.DataType, (LPBYTE)Computer.ComputerKinds, &Computer.Window.DataLen);
			SetDlgItemText(hDlgMain, IDC_KINDS, Computer.ComputerKinds);
			RegCloseKey(Computer.Window.hKey);
		}//��ǻ�� ����
		if (RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\NVIDIA Corporation\\Global\\GFExperience"), 0, KEY_READ, &Computer.Window.hKey) == ERROR_SUCCESS) {
			RegQueryValueEx(Computer.Window.hKey, TEXT("CurrentNotifiedDriverVersion"), NULL, &Computer.Window.DataType, (LPBYTE)Computer.Graphic.DriverVersion, &Computer.Window.DataLen);
			SetDlgItemText(hDlgMain, IDC_DRIVERVERSION, Computer.Graphic.DriverVersion);
			RegCloseKey(Computer.Window.hKey);
		}//�׷��� ����̹� ����
		if (Computer.System.dwOemId == 9) {
			SetDlgItemText(hDlg, IDC_WINDOWBITS, L"���� �ü��� 64��Ʈ �ü�� �Դϴ�.");
			Computer.Window.WindowBits = WINDOW64;
		}
		else {
			SetDlgItemText(hDlg, IDC_WINDOWBITS, L"���� �ü��� 32��Ʈ �ü�� �Դϴ�.");
			Computer.Window.WindowBits = WINDOW32;
		}//�ü�� ��Ʈ�� ������
		GetGPUName(FALSE);//���� �׷��� ī�� ���-> ������ ��Ŵ
		GetMonitorInformation();//����� ���� ���ϱ� -> 2�������� ����.
		TCHAR SizeStr[20] = { NULL };//�ػ� ���ϱ�
		RECT rt;
		GetClientRect(GetDesktopWindow(), &rt);
		Computer.Display.x = rt.right;
		Computer.Display.y = rt.bottom;
		swprintf(SizeStr, L"%d x %d", Computer.Display.x, Computer.Display.y);
		SetDlgItemText(hDlg, IDC_SCREENSIZE, SizeStr);
		//���м�----------------------------
		system("ver>date.txt");
		FILE *fp = fopen("date.txt", "r");
		if (fp != NULL) {
			TCHAR Str[50] = { NULL };
			fgetc(fp);
			fgetws(Str, 50, fp);
			SetDlgItemText(hDlgMain, IDC_WINDOWVERSION2, Str);
			fclose(fp);
			DeleteFile(L"date.txt");
		}
		for (int i = 0; i < 4; i++) {
			SendDlgItemMessage(hDlg, IDC_SETSPEED, CB_ADDSTRING, NULL, (LPARAM)Item[i]);
			SendDlgItemMessage(hDlg, IDC_INVESTIGATE, CB_ADDSTRING, NULL, (LPARAM)Items[i]);
			GetComputerName(Computer.ComputerName, &Len);//��ǻ�� �̸��� �����̸��� ��� �����ʿ䰡 �����Ƿ�
			SetDlgItemText(hDlgMain, IDC_COMPUTERNAME, Computer.ComputerName);
			GetUserName(Computer.UserName, &Len);
			SetDlgItemText(hDlgMain, IDC_USERNAME, Computer.UserName);
		}
		GetNativeSystemInfo(&Computer.System);
		GlobalMemoryStatusEx(&Computer.Memory);
		GetDiskInformation();
		SendDlgItemMessage(hDlg, IDC_CPUPRESENTPROGRESS, PBM_SETRANGE, NULL, MAKELPARAM(0, 100));
		SendDlgItemMessage(hDlg, IDC_PRESENTPROGRESS, PBM_SETRANGE, NULL, MAKELPARAM(0, 100));//�޸𸮻����� %�̹Ƿ� 0~100����
		SendDlgItemMessage(hDlg, IDC_AVAILPHYSPROGRESS, PBM_SETRANGE, NULL, MAKELPARAM(0, ((Computer.Memory.ullTotalPhys / 1024) / 1024)));
		SendDlgItemMessage(hDlg, IDC_AVAILVIRTUALPROGRESS, PBM_SETRANGE, NULL, MAKELPARAM(0, ((Computer.Memory.ullTotalVirtual / 1024) / 1024)));
		SendDlgItemMessage(hDlg, IDC_AVAILPAGEPROGRESS, PBM_SETRANGE, NULL, MAKELPARAM(0, ((Computer.Memory.ullTotalPageFile / 1024) / 1024)));
		SendDlgItemMessage(hDlg, IDC_CCANUSESPACEPROGRESS2, PBM_SETRANGE, NULL, MAKELPARAM(0, (((Computer.CDiskInfo.TotalSpace.QuadPart) / 1024) / 1024) / 1024));
		SendDlgItemMessage(hDlg, IDC_DCANUSESPACEPROGRESS, PBM_SETRANGE, NULL, MAKELPARAM(0, (((Computer.DDiskInfo.TotalSpace.QuadPart) / 1024) / 1024) / 1024));
		SendDlgItemMessage(hDlg, IDC_DISKTOTALPROGRESS, PBM_SETRANGE, NULL, MAKELPARAM(0, Computer.CDiskInfo.Reserved1));
		Information();
		return TRUE;
	}
	case WM_CLOSE:
		KillTimer(hDlg, 1);
		EndDialog(hDlg, TRUE);
			break;
	case WM_TIMER:
		switch (wParam) {
		case 1:
			Information();
			break;
		}
		return TRUE;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_SOUND:
			Sound = SendDlgItemMessage(hDlg, IDC_SOUND, BM_GETCHECK, NULL, NULL);
			return TRUE;
		case IDC_AUTO:
			Check = SendDlgItemMessage(hDlg, IDC_AUTO, BM_GETCHECK, NULL, NULL);
			if (Check)
			{
				switch (Select) {
				case FAST:
					KillTimer(hDlg, 1);//�޺� �ڽ��� �ٲ܋� ������ �ߺ� ������ ����
					SetTimer(hDlg, 1, 300, 0);
					SendMessage(hDlg, WM_TIMER, NULL, NULL);
					break;
				case NORMAL:
					KillTimer(hDlg, 1);
					SetTimer(hDlg, 1, 1000, 0);
					SendMessage(hDlg, WM_TIMER, NULL, NULL);
					break;
				case SLOW:
					KillTimer(hDlg, 1);
					SetTimer(hDlg, 1, 5000, 0);
					SendMessage(hDlg, WM_TIMER, NULL, NULL);
					break;
				}
			}
			else {
				KillTimer(hDlg, 1);
			}
			return TRUE;
		case IDC_SETSPEED:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				Select = SendDlgItemMessage(hDlg, IDC_SETSPEED, CB_GETCURSEL, NULL, NULL);
				SendMessage(hDlgMain, WM_COMMAND, IDC_AUTO, NULL);
				break;
			}
			return TRUE;
		case IDC_INTOSIMPLE:
			KillTimer(hDlg, 1);
			EndDialog(hDlg, TRUE);
			DialogBox(hInst, MAKEINTRESOURCE(IDD_SIMPLE), HWND_DESKTOP, SimpleDlgProc);
			return TRUE;
		case IDC_REFRESH:
			Information();
			return TRUE;
		case IDC_SAVEFILE:
			SaveInforMation();
			return TRUE;
		case IDC_INVESTIGATE:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE:
				ToolSelect = SendDlgItemMessage(hDlg, IDC_INVESTIGATE, CB_GETCURSEL, NULL, NULL);
				break;
			}
			return TRUE;
		case IDC_RUNCOMMAND:
			DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_PRINTDLG), hDlg, PrintDlgProc, ToolSelect);
			return TRUE;
		case IDOK:
			KillTimer(hDlg, 1);
			EndDialog(hDlg, IDOK);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

enum {
	GetMonitor = 0,
	GetKeyBoard = 1,
	GetGraphic = 2,
	GetCPU = 3
};
static int arFunc[] = { 0,10,12,10,12,10,24,7,8,9,10,11,12 };
TCHAR KeyType[50] = { NULL };
TCHAR Explain[MAX_PATH * 10] = { NULL };
INT_PTR CALLBACK PrintDlgProc(HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	switch(iMessage) {
	case WM_INITDIALOG:
		switch (lParam) {
		case GetMonitor:
			SetWindowText(hDlg, L"����� ���� ����");
			hdc = GetDC(hDlg);
			CountMonitor = 1;
			EnumDisplayMonitors(NULL,NULL,DetailEnumMonitorProc,(LPARAM)hdc);
			SetDlgItemText(hDlg, IDC_PRINTEDIT, MonitorStr);
			ReleaseDC(hDlg, hdc);
			break;
		case GetKeyBoard:
			SetWindowText(hDlg, L"Ű���� ���� ����");
			switch (GetKeyboardType(0)) {
			case 1:lstrcpy(KeyType, L"IBM PC/XT or compatible(83 key)"); break;
			case 2:lstrcpy(KeyType, L"Olivetti ICO(102 key)"); break;
			case 3:lstrcpy(KeyType, L"IBM PC/AT (84 key)"); break;
			case 4:lstrcpy(KeyType, L"IBM enhanced(101 or 102 key)"); break;
			case 5:lstrcpy(KeyType, L"Nokia 1050"); break;
			case 6:lstrcpy(KeyType, L"Nokia 9140"); break;
			case 7:lstrcpy(KeyType, L"�Ϻ� Ű����"); break;
			default:lstrcpy(KeyType, L"��Ÿ Ű���� �Ǵ� ��Ʈ�� Ű����"); break;
			}
			wsprintf(Explain, L"Ű���� Ÿ�� : %s\r\n���� Ÿ�� : %d, ���Ű�� : %d��",
				KeyType, GetKeyboardType(1), arFunc[GetKeyboardType(2)]);
			SetDlgItemText(hDlg, IDC_PRINTEDIT, Explain);
			break;
		case GetGraphic: {
			GetGPUName(TRUE);
			SetWindowText(hDlg, L"�׷��� ī�� ���� ����");
			lstrcpy(Explain, L"�ַ� ù��°�� �׷��� ī�尡 ���� �������� �׷��� ī�� �Ǵ� ���� �׷���ī�� �Դϴ�.\r\n2��°�� �����̰ų� ù��° �������� ���ۼ����� ����Ǿ\r\n���翡�� ���������� �ʴ� �׷��� ī���Դϴ�.\r\n�� �Ʒ��δ� ���Ǵ� �Ȱ����ϴ�.\r\n(���� ����ũ�μ���Ʈ ������ ������ ����̹��� �������� ��ü �׷��� ī���Դϴ�.)\r\n\r\n");
			for (int i = 0; lstrlen(Computer.Graphic.EnumGPU[i]) != 0; i++) {
				lstrcat(Explain, Computer.Graphic.EnumGPU[i]);
				lstrcat(Explain, L"\r\n");
			}
			SetDlgItemText(hDlg, IDC_PRINTEDIT, Explain);
			break;
		case GetCPU:
			if (Computer.Window.Windows10 != TRUE) {//������ 8���ϸ� ���� ����
				SetWindowText(hDlg, L"CPU ���� ����");
				swprintf(Explain, L"CPU : %s\r\n\r\n", Computer.CenterProcessUnit.CPUName);
				SetDlgItemText(hDlg, IDC_PRINTEDIT, Explain);
				break;
			}
			GetCPUClock();
			SetWindowText(hDlg, L"CPU ���� ����");
			swprintf(Explain, L"CPU : %s\r\n\r\n",Computer.CenterProcessUnit.CPUName);
			swprintf(Store, L"CPU ���� �ӵ� (Ŭ��) : %.1f Ghz (%d Mhz)\r\n", ((float)Computer.CenterProcessUnit.CPUFrequency / (float)1000),Computer.CenterProcessUnit.CPUFrequency);
			lstrcat(Explain, Store);
			swprintf(Store, L"CPU �ִ� ���� �ӵ� (�ͺ� �ν�Ʈ Ŭ��) : %.1f Ghz (%d Mhz)\r\n", ((float)Computer.CenterProcessUnit.CPUMaxFrequency / (float)1000),Computer.CenterProcessUnit.CPUMaxFrequency);
			lstrcat(Explain, Store);
			swprintf(Store, L"CPU ���� ���� �ӵ� : %.1f Ghz (%d Mhz)\r\n", ((float)Computer.CenterProcessUnit.BusFrequency / (float)1000), Computer.CenterProcessUnit.BusFrequency);
			lstrcat(Explain, Store);
			SetDlgItemText(hDlg, IDC_PRINTEDIT, Explain);
			break;
		}
		}
		return TRUE;
	case WM_CLOSE:
		EndDialog(hDlg, TRUE);
		return TRUE;
	}
	return FALSE;
}

VOID Information() {
	GetNativeSystemInfo(&Computer.System);
	GlobalMemoryStatusEx(&Computer.Memory);
	GetDiskInformation();
	GetCPUUsage();
	SetDlgItemText(hDlgMain, IDC_OUTGRAPHICCARD, Computer.Graphic.OutGPU);
	SetDlgItemText(hDlgMain, IDC_INGRAPHICCARD, Computer.Graphic.InGPU);
	swprintf(PercentStr, L"%d %%", Computer.CenterProcessUnit.CPUUsage);
	SetDlgItemText(hDlgMain, IDC_CPUPRESENT, PercentStr);
	SendDlgItemMessage(hDlgMain, IDC_CPUPRESENTPROGRESS, PBM_SETPOS, Computer.CenterProcessUnit.CPUUsage, NULL);
	SetDlgItemInt(hDlgMain, IDC_NUMBEROFPROCESS, Computer.System.dwNumberOfProcessors, FALSE);
	swprintf(PercentStr, L"%d %%", Computer.Memory.dwMemoryLoad);
	SetDlgItemText(hDlgMain, IDC_PRESENT, PercentStr);
	SendDlgItemMessage(hDlgMain, IDC_PRESENTPROGRESS, PBM_SETPOS, Computer.Memory.dwMemoryLoad, NULL);
	swprintf(PercentStr, L"%.2f GB", (DOUBLE)((Computer.Memory.ullTotalPhys / 1024) / 1024) / 1024);
	SetDlgItemText(hDlgMain, IDC_TOTALPHYS, PercentStr);
	swprintf(PercentStr, L"%.2f GB", (DOUBLE)(((Computer.Memory.ullTotalPhys / 1024) / 1024) - ((Computer.Memory.ullAvailPhys / 1024) / 1024))/1024);
	SetDlgItemText(hDlgMain, IDC_USEDPHYS, PercentStr);
	swprintf(PercentStr, L"%.2f GB", (DOUBLE)((Computer.Memory.ullAvailPhys / 1024) / 1024) / 1024);
	SetDlgItemText(hDlgMain, IDC_AVAILPHYS, PercentStr);
	SendDlgItemMessage(hDlgMain, IDC_AVAILPHYSPROGRESS, PBM_SETPOS, ((Computer.Memory.ullTotalPhys / 1024) / 1024) - ((Computer.Memory.ullAvailPhys / 1024) / 1024), NULL);
	swprintf(PercentStr, L"%.2f GB", (DOUBLE)((Computer.Memory.ullTotalVirtual / 1024) / 1024) / 1024);
	SetDlgItemText(hDlgMain, IDC_TOTALVIRTUAL, PercentStr);
	swprintf(PercentStr, L"%.2f GB", (DOUBLE)((Computer.Memory.ullAvailVirtual / 1024) / 1024) / 1024);
	SetDlgItemText(hDlgMain, IDC_AVAILVIRTUAL, PercentStr);
	SendDlgItemMessage(hDlgMain, IDC_AVAILVIRTUALPROGRESS, PBM_SETPOS, ((Computer.Memory.ullTotalVirtual / 1024) / 1024) - ((Computer.Memory.ullAvailVirtual / 1024) / 1024), NULL);
	swprintf(PercentStr, L"%.2f GB", (DOUBLE)((Computer.Memory.ullAvailExtendedVirtual / 1024) / 1024) / 1024);
	SetDlgItemText(hDlgMain, IDC_AVAILEXTENDEDVIRTUAL, PercentStr);
	swprintf(PercentStr, L"%.2f GB", (DOUBLE)((Computer.Memory.ullTotalPageFile / 1024) / 1024) / 1024);
	SetDlgItemText(hDlgMain, IDC_TOTALPAGE, PercentStr);
	swprintf(PercentStr, L"%.2f GB", (DOUBLE)((Computer.Memory.ullAvailPageFile / 1024) / 1024) / 1024);
	SetDlgItemText(hDlgMain, IDC_AVAILPAGE, PercentStr);
	swprintf(PercentStr, L"%.2f GB", (DOUBLE)((Computer.Memory.ullAvailPageFile / 1024) / 1024) / 1024);
	SetDlgItemText(hDlgMain, IDC_AVAILPAGE, PercentStr);
	SendDlgItemMessage(hDlgMain, IDC_AVAILPAGEPROGRESS, PBM_SETPOS, ((Computer.Memory.ullTotalPageFile / 1024) / 1024) - ((Computer.Memory.ullAvailPageFile / 1024) / 1024), NULL);
	swprintf(PercentStr, L"%.2f GB", (DOUBLE)((Computer.CDiskInfo.TotalSpace.QuadPart) / 1024) / 1024 / 1024);
	SetDlgItemText(hDlgMain, IDC_CTOTALSPACE, PercentStr);
	swprintf(PercentStr, L"%.2f GB", (DOUBLE)((Computer.CDiskInfo.TotalFree.QuadPart) / 1024) / 1024 / 1024);
	SetDlgItemText(hDlgMain, IDC_CCANUSESPACE, PercentStr);
	SendDlgItemMessage(hDlgMain, IDC_CCANUSESPACEPROGRESS2, PBM_SETPOS, (((((Computer.CDiskInfo.TotalSpace.QuadPart) / 1024) / 1024) / 1024) - ((((Computer.CDiskInfo.TotalFree.QuadPart) / 1024) / 1024) / 1024)), NULL);
	if ((Computer.CDiskInfo.Reserved1 != 0) || (Computer.CDiskInfo.Reserved2 != 0) || (Computer.CDiskInfo.Reserved3 != 0)) {
		swprintf(PercentStr, L"%.2f GB", (DOUBLE)((Computer.DDiskInfo.TotalSpace.QuadPart) / 1024) / 1024 / 1024);
		SetDlgItemText(hDlgMain, IDC_DTOTALSPACE, PercentStr);
		swprintf(PercentStr, L"%.2f GB", (DOUBLE)((Computer.DDiskInfo.TotalFree.QuadPart) / 1024) / 1024 / 1024);
		SetDlgItemText(hDlgMain, IDC_DCANUSESPACE, PercentStr);
		SendDlgItemMessage(hDlgMain, IDC_DCANUSESPACEPROGRESS, PBM_SETPOS, (((((Computer.DDiskInfo.TotalSpace.QuadPart) / 1024) / 1024) / 1024) - ((((Computer.DDiskInfo.TotalFree.QuadPart) / 1024) / 1024) / 1024)), NULL);
		swprintf(PercentStr, L"%.2f GB", Computer.CDiskInfo.Reserved1);
		SetDlgItemText(hDlgMain, IDC_DISKTOTAL, PercentStr);
		swprintf(PercentStr, L"%.2f GB", Computer.CDiskInfo.Reserved2);
		SetDlgItemText(hDlgMain, IDC_DISKTOTALCANUSE, PercentStr);
		swprintf(PercentStr, L"%.2f GB", Computer.CDiskInfo.Reserved3);
		SetDlgItemText(hDlgMain, IDC_DISKTOTALUSED, PercentStr);
		SendDlgItemMessage(hDlgMain, IDC_DISKTOTALPROGRESS, PBM_SETPOS, Computer.CDiskInfo.Reserved3, NULL);
	}
	else {
		SetDlgItemText(hDlgMain, IDC_DTOTALSPACE, L"���� ����̺�(D:)�� �������� �ʽ��ϴ�!.");
		SetDlgItemText(hDlgMain, IDC_DCANUSESPACE, L"���� ����̺�(D:)�� �������� �ʽ��ϴ�!.");
		SendDlgItemMessage(hDlgMain, IDC_DCANUSESPACEPROGRESS, PBM_SETPOS, 0, NULL);
		SetDlgItemText(hDlgMain, IDC_DISKTOTAL, L"���� �ý��ۿ����� ���� ���Ҽ� �����ϴ�.");
		SetDlgItemText(hDlgMain, IDC_DISKTOTALCANUSE, L"���� �ý��ۿ����� ���� ���Ҽ� �����ϴ�.");
		SetDlgItemText(hDlgMain, IDC_DISKTOTALUSED, L"���� �ý��ۿ����� ���� ���Ҽ� �����ϴ�.");
		SendDlgItemMessage(hDlgMain, IDC_DISKTOTALPROGRESS, PBM_SETPOS, 0, NULL);
	}
	if (Sound) {
		MessageBeep(MB_OK);
	}
}
VOID GetDiskInformation() {
	if (!GetDiskFreeSpaceEx(L"C:\\", NULL, &Computer.CDiskInfo.TotalSpace, &Computer.CDiskInfo.TotalFree))
		return;
	if (!GetDriveType(L"D:\\") == DRIVE_UNKNOWN) {
		if (!GetDiskFreeSpaceEx(L"D:\\", NULL, &Computer.DDiskInfo.TotalSpace, &Computer.DDiskInfo.TotalFree))
			return;
	}
	Computer.CDiskInfo.Reserved1 = (DOUBLE)((DOUBLE)((Computer.CDiskInfo.TotalSpace.QuadPart / 1024) / 1024) / 1024) + (DOUBLE)((DOUBLE)((Computer.DDiskInfo.TotalSpace.QuadPart / 1024) / 1024) / 1024);//Reserved�� �ƹ� ��ũ�γ� �ᵵ �������
	Computer.CDiskInfo.Reserved2 = (DOUBLE)((DOUBLE)((Computer.CDiskInfo.TotalFree.QuadPart / 1024) / 1024) / 1024) + (DOUBLE)((DOUBLE)((Computer.DDiskInfo.TotalFree.QuadPart / 1024) / 1024) / 1024);
	Computer.CDiskInfo.Reserved3 = Computer.CDiskInfo.Reserved1 - Computer.CDiskInfo.Reserved2;
}

/*
GPU�� ���Ѵ�( ���� ,���� �ٱ��� ������ ��ü �׷���ī�嵵 ���� )
*/
VOID GetGPUName(BOOLEAN Enum) {
	if (Computer.Window.Windows10 != TRUE) {//10 ���� ������ ����
		LPWSTR strGPUName = NULL;
		DISPLAY_DEVICE Display;
		WCHAR szGpuName[128] = { 0, };
		memset(&Display, 0, sizeof(DISPLAY_DEVICE));
		Display.cb = sizeof(Display);
		if (Enum == TRUE) {
			for (int i = 0; i < 10; i++) {
				if (EnumDisplayDevices(NULL, i, &Display, 0)) {
					lstrcpy(Computer.Graphic.EnumGPU[i],Display.DeviceString);
				}
			}
			return;
		}
		if (EnumDisplayDevices(NULL, 0, &Display, 0)) {
			strGPUName = Display.DeviceString;
			SetDlgItemText(hDlgMain, IDC_INGRAPHICCARD, strGPUName);
			lstrcpy(Computer.Graphic.InGPU, strGPUName);
		}
		if (EnumDisplayDevices(NULL, 1, &Display, 0)) {
			strGPUName = Display.DeviceString;
			SetDlgItemText(hDlgMain, IDC_OUTGRAPHICCARD, strGPUName);
			lstrcpy(Computer.Graphic.OutGPU, strGPUName);
		}
	}
	else {
		HINSTANCE hDXGI = LoadLibrary(L"dxgi.dll");
		IDXGIAdapter* pAdapter = nullptr;
		LPCREATEDXGIFACTORY pCreateDXGIFactory = nullptr;
		pCreateDXGIFactory = (LPCREATEDXGIFACTORY)GetProcAddress(hDXGI, "CreateDXGIFactory");
		IDXGIFactory* pDXGIFactory;
		pCreateDXGIFactory(__uuidof(IDXGIFactory), (LPVOID*)&pDXGIFactory);
		assert(pDXGIFactory != 0);
		pDXGIFactory->EnumAdapters(0, &pAdapter);
		DXGI_ADAPTER_DESC desc;
		if (Enum == TRUE) {
			for (int i = 0; i < 10; i++) {
				pDXGIFactory->EnumAdapters(i, &pAdapter);
				memset(&desc, 0, sizeof(DXGI_ADAPTER_DESC));
				if (pAdapter == NULL) {
					FreeLibrary(hDXGI);
					return;
				}
				pAdapter->GetDesc(&desc);
				lstrcpy(Computer.Graphic.EnumGPU[i],desc.Description);
			}
			FreeLibrary(hDXGI);
			return;
		}
		memset(&desc, 0, sizeof(DXGI_ADAPTER_DESC));
		pAdapter->GetDesc(&desc);
		lstrcpy(Computer.Graphic.OutGPU, desc.Description);
		pDXGIFactory->EnumAdapters(1, &pAdapter);
		memset(&desc, 0, sizeof(DXGI_ADAPTER_DESC));
		pAdapter->GetDesc(&desc);
		lstrcpy(Computer.Graphic.InGPU, desc.Description);
		FreeLibrary(hDXGI);
	}
}

PDH_FMT_COUNTERVALUE counterVal;
PDH_HQUERY cpuQuery;
PDH_HCOUNTER cpuTotal;
VOID Init() {
	PdhOpenQuery(NULL, NULL, &cpuQuery);
	PdhAddCounter(cpuQuery, L"\\Processor(_TOTAL)\\% Processor Time", NULL, &cpuTotal);
	PdhCollectQueryData(cpuQuery);
}

VOID GetCPUUsage() {
	PdhCollectQueryData(cpuQuery);
	PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
	Computer.CenterProcessUnit.CPUUsage = counterVal.doubleValue;
}

VOID GetCPUClock()
{
	INT CPUInfo[4] = { 0 };
	__cpuid(CPUInfo, 0);
	if (CPUInfo[0] >= 0x16) {
		__cpuid(CPUInfo, 0x16);
		Computer.CenterProcessUnit.CPUFrequency = CPUInfo[0];
		Computer.CenterProcessUnit.CPUMaxFrequency = CPUInfo[1];
		Computer.CenterProcessUnit.BusFrequency = CPUInfo[2];
	}
	else {
		Computer.CenterProcessUnit.CPUFrequency = 0;
		Computer.CenterProcessUnit.CPUMaxFrequency = 0;
		Computer.CenterProcessUnit.BusFrequency = 0;
	}
}

VOID GetMonitorInformation() {
	HDC hdc = GetDC(hDlgMain);
	TCHAR PrintStr[MAX_PATH] = { NULL };
	Computer.MonitorInfo.MonitorNumber = 0;//�ʱ�ȭ
	memset(Computer.MonitorInfo.DeviceName, 0, sizeof(Computer.MonitorInfo.DeviceName));
	memset(Computer.MonitorInfo.BitPixels, 0, sizeof(Computer.MonitorInfo.BitPixels));
	EnumDisplayMonitors(NULL, NULL, EnumMonitorProc, (LPARAM)hdc);
	SetDlgItemInt(hDlgMain, IDC_NUMBEROFMONITOR, Computer.MonitorInfo.MonitorNumber, FALSE);
	if (Computer.MonitorInfo.BitPixels[1] != NULL) {
		wsprintf(PrintStr, L"%d ��Ʈ ���� �����, %d ��Ʈ ���� �����", Computer.MonitorInfo.BitPixels[0], Computer.MonitorInfo.BitPixels[1]);
		SetDlgItemText(hDlgMain, IDC_MONITORBITPIXEL, PrintStr);
	}
	else {
		wsprintf(PrintStr, L"%d ��Ʈ ���� �����", Computer.MonitorInfo.BitPixels[0]);
		SetDlgItemText(hDlgMain, IDC_MONITORBITPIXEL, PrintStr);
	}
	if (lstrlen(Computer.MonitorInfo.DeviceName2) != NULL) {
		SetDlgItemText(hDlgMain, IDC_MONITORNAME, Computer.MonitorInfo.DeviceName);
		SetDlgItemText(hDlgMain, IDC_MONITORNAME2, Computer.MonitorInfo.DeviceName2);
	}
	else {
		SetDlgItemText(hDlgMain, IDC_MONITORNAME, Computer.MonitorInfo.DeviceName);
		SetDlgItemText(hDlgMain, IDC_MONITORNAME2, L"2��° ����Ͱ� �������� �ʽ��ϴ�!. (��� ����� ȯ���� �ƴմϴ�!.)");
	}
	ReleaseDC(hDlgMain, hdc);
}

VOID SaveInforMation() {
	//Information(); �̷��� �Ұ�� �ѹ��� ���ΰ�ħ���� �ֽ� ������ �����Ҽ� ������ ����ڰ� ��ġ �ʰԵ� ���ΰ�ħ�Ҽ� �ֱ� ������ ������.
	GetNativeSystemInfo(&Computer.System);
	GlobalMemoryStatusEx(&Computer.Memory);
	GetDiskInformation();
	GetMonitorInformation();
	if (Computer.Window.Windows10 == TRUE)//������ 10���ϸ� ���� ����
		GetCPUClock();
	//���� ���� Computer ����ü�� ������ ä���ִ� �Լ��� ȣ��
	TCHAR Memory[MAX_PATH] = { NULL };//�̸��� �޸��� ��ǻ� ���� ���ڿ�
	TCHAR Somemary[MAX_PATH] = { NULL };
	DWORDLONG PhyMemory;
	GetPhysicallyInstalledSystemMemory(&PhyMemory);
	FILE *fp = _wfopen(L"ComputerInfo.txt", L"w");
	_wsetlocale(LC_ALL, L"kor");//�ѱ� ���� ����
	fwprintf(fp, L"�ý��� ��\r\n");
	fwprintf(fp, L"%s\r\n", Computer.ComputerKinds);
	fwprintf(fp, L"CPU ����\r\n");
	fwprintf(fp, L"%s\r\n",Computer.CenterProcessUnit.CPUName);
	if (Computer.Window.Windows10 == TRUE) {//������ 10���ϸ� ���� ����
		fwprintf(fp, L"CPU �⺻ ���� �ӵ�\r\n");
		fwprintf(fp, L"%.1f Ghz (%d Mhz)\r\n", ((float)Computer.CenterProcessUnit.CPUFrequency, Computer.CenterProcessUnit.CPUFrequency / (float)1000), Computer.CenterProcessUnit.CPUFrequency);
		fwprintf(fp, L"CPU �ִ� ���� �ӵ� (�ͺ� �ν�Ʈ Ŭ��)\r\n");
		fwprintf(fp, L"%.1f Ghz (%d Mhz)\r\n", ((float)Computer.CenterProcessUnit.CPUMaxFrequency, Computer.CenterProcessUnit.CPUMaxFrequency / (float)1000), Computer.CenterProcessUnit.CPUMaxFrequency);
		fwprintf(fp, L"CPU ���� ���� �ӵ�\r\n");
		fwprintf(fp, L"%.1f Ghz (%d Mhz)\r\n", ((float)Computer.CenterProcessUnit.BusFrequency, Computer.CenterProcessUnit.BusFrequency / (float)1000), Computer.CenterProcessUnit.BusFrequency);
	}
	fwprintf(fp, L"���μ��� ( CPU �ھ� ) ����\r\n");
	fwprintf(fp, L"%d ��\r\n", Computer.System.dwNumberOfProcessors);
	fwprintf(fp, L"�׷��� ī�� ����\r\n");
	fwprintf(fp, L"%s\r\n", Computer.Graphic.InGPU);
	fwprintf(fp, L"�׷��� ī�� ����\r\n");
	fwprintf(fp, L"%s\r\n", Computer.Graphic.OutGPU);
	fwprintf(fp, L"NVDIA �׷��� ����̹� ����\r\n");
	fwprintf(fp, L"%s\r\n", Computer.Graphic.DriverVersion);
	fwprintf(fp, L"���� �޸� ũ��\r\n");
	fwprintf(fp, L"�� %.2lf GB\r\n", (float)PhyMemory / 1024 / 1024);
	fwprintf(fp, L"%llu ( %.2lf GB )\r\n", Computer.Memory.ullTotalPhys, (double)Computer.Memory.ullTotalPhys / 1024 / 1024 / 1024);
	fwprintf(fp, L"���� �޸� ũ��\r\n");
	fwprintf(fp, L"%llu ( %.2lf GB )\r\n", Computer.Memory.ullTotalVirtual, (double)Computer.Memory.ullTotalVirtual / 1024 / 1024 / 1024);
	fwprintf(fp, L"������ ���� ũ��\r\n");
	fwprintf(fp, L"%llu ( %.2lf GB )\r\n", Computer.Memory.ullTotalPageFile, (double)Computer.Memory.ullTotalPageFile / 1024 / 1024 / 1024);
	fwprintf(fp, L"�� ���� ������� �뷮\r\n");
	fwprintf(fp, L"%llu ( %.8lf GB )\r\n", (Computer.CDiskInfo.TotalSpace.QuadPart + Computer.DDiskInfo.TotalSpace.QuadPart), Computer.CDiskInfo.Reserved1);
	fwprintf(fp, L"�� �����ִ� ���� ������� �뷮\r\n");
	fwprintf(fp, L"%llu ( %.8lf GB )\r\n", (Computer.CDiskInfo.TotalFree.QuadPart + Computer.DDiskInfo.TotalFree.QuadPart), Computer.CDiskInfo.Reserved2);
	fwprintf(fp, L"���� ���� ��������� �� ��뷮\r\n");
	fwprintf(fp, L"%lf ( %.8lf GB )\r\n", (Computer.CDiskInfo.Reserved1 - Computer.CDiskInfo.Reserved2), Computer.CDiskInfo.Reserved3);
	fwprintf(fp, L"���� C����̺� �뷮\r\n");
	fwprintf(fp, L"%llu ( %.8lf GB )\r\n", Computer.CDiskInfo.TotalSpace.QuadPart, (double)Computer.CDiskInfo.TotalSpace.QuadPart / 1024 / 1024 / 1024);
	fwprintf(fp, L"���� C����̺� ���� �뷮\r\n");
	fwprintf(fp, L"%llu ( %.8lf GB )\r\n", Computer.CDiskInfo.TotalFree.QuadPart, (double)Computer.CDiskInfo.TotalFree.QuadPart / 1024 / 1024 / 1024);
	fwprintf(fp, L"���� D����̺� �뷮\r\n");
	fwprintf(fp, L"%llu ( %.8lf GB )\r\n", Computer.DDiskInfo.TotalSpace.QuadPart, (double)Computer.DDiskInfo.TotalSpace.QuadPart / 1024 / 1024 / 1024);
	fwprintf(fp, L"���� D����̺� ���� �뷮\r\n");
	fwprintf(fp, L"%llu ( %.8lf GB )\r\n", Computer.DDiskInfo.TotalFree.QuadPart, (double)Computer.DDiskInfo.TotalFree.QuadPart / 1024 / 1024 / 1024);
	fwprintf(fp, L"�ü��\r\n");
	fwprintf(fp, L"%s ���� %d ��Ʈ�Դϴ�.\r\n", Computer.Window.WindowVersion, Computer.Window.WindowBits);
	fwprintf(fp, L"����� �̸�\r\n");
	fwprintf(fp, L"%s\r\n", Computer.UserName);
	fwprintf(fp, L"��ǻ�� �̸�\r\n");
	fwprintf(fp, L"%s\r\n", Computer.ComputerName);
	fwprintf(fp, L"�ػ�\r\n");
	fwprintf(fp, L"%d x %d\r\n", Computer.Display.x, Computer.Display.y);
	fwprintf(fp, L"����� ����\r\n");
	fwprintf(fp, L"%d\r\n", Computer.MonitorInfo.MonitorNumber);
	fwprintf(fp, L"����� ��Ʈ ����\r\n");
	if(Computer.MonitorInfo.BitPixels[1] != NULL)
		fwprintf(fp, L"%d ��Ʈ ���� �����, %d ��Ʈ ���� �����\r\n", Computer.MonitorInfo.BitPixels[0], Computer.MonitorInfo.BitPixels[1]);
	else
		fwprintf(fp, L"%d ��Ʈ ���� �����, 2��° ����Ͱ� �������� �ʽ��ϴ�!. (��� ����� ȯ���� �ƴմϴ�!.)\r\n", Computer.MonitorInfo.BitPixels[0]);
	fwprintf(fp, L"����� �̸�\r\n");
	if (lstrlen(Computer.MonitorInfo.DeviceName2) != NULL)
		fwprintf(fp, L"%s, %s\r\n", Computer.MonitorInfo.DeviceName, Computer.MonitorInfo.DeviceName2);
	else
		fwprintf(fp, L"%s, 2��° ����Ͱ� ���� ���� �ʽ��ϴ�!. (��� ����� ȯ���� �ƴմϴ�!.)\r\n", Computer.MonitorInfo.DeviceName);
	fclose(fp);
	MessageBox(hDlgMain, L"���� �����Ϸ�!", L"�˸�", MB_OK | MB_ICONINFORMATION);
}