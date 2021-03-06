// OpenCVTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../CWOpenCvCamera/CWOpenCvCamera.h"

typedef void(*LPInitCamera)();
typedef bool(*LPDoProcess)(char*);
typedef void(*LPUninitCamera)();

HANDLE m_hAsyncEventHandle;
HMODULE gHandle = NULL;
LPInitCamera lpInitCamera = NULL;
LPDoProcess lpDoProcess = NULL;
LPUninitCamera lpUninitCamera = NULL;

static DWORD WINAPI AsyncConstructCameraWnd111(LPVOID lpParam);
void InitWnd();
int StartWorking();

DWORD WINAPI AsyncConstructCameraWnd111(LPVOID lpParam)
{
	StartWorking();
	return 0;
}

void InitWnd()
{
	ResetEvent(m_hAsyncEventHandle);

	// 启动界面线程
	DWORD dwThreadId = 0;
	HANDLE hThread = CreateThread(NULL, 0, AsyncConstructCameraWnd111, NULL, 0, &dwThreadId);

	DWORD waitRet = WaitForSingleObject(m_hAsyncEventHandle, 10000);

}

int StartWorking()
{
	while (true)
	{

	}
	MSG msg;
	BOOL fGotMessage;
	while ((fGotMessage = GetMessage(&msg, (HWND)NULL, 0, 0)) != 0 && fGotMessage != -1)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	msg.wParam;
	return 0;
}

int main(int, char*[])
{
	m_hAsyncEventHandle = CreateEvent(NULL, FALSE, FALSE, _T("CameraOperationDone11111"));
#ifdef _DEBUG
	gHandle = LoadLibrary(TEXT("..\\Debug\\CWOpenCvCamera.dll"));
#else
	gHandle = LoadLibrary(TEXT("..\\Release\\CWOpenCvCamera.dll"));
#endif

	lpInitCamera = (LPInitCamera)GetProcAddress(gHandle, "InitCamera");
	lpDoProcess = (LPDoProcess)GetProcAddress(gHandle, "DoProcess");
	lpUninitCamera = (LPUninitCamera)GetProcAddress(gHandle, "UninitCamera");

	char step1_open[] = "{\"id\":\"0\",\"wnd\":\"0\",\"type\":\"start\",\"left\":\"400\",\"top\":\"0\",\"width\":\"400\",\"height\":\"800\", \"roiW\":\"600\", \"roiH\":\"200\"}";
	char step2_open[] = "{\"id\":\"0\",\"wnd\":\"0\",\"type\":\"setProperty\",\"left\":\"-800\",\"top\":\"0\",\"width\":\"600\",\"height\":\"900\", \"roiW\":\"800\", \"roiH\":\"200\"}";
	

	char setProperty[] = R"({"id":"0", "type":"setProperty"})";

	char captureJpg[] = R"({"type":"capture", "path":"d:/logs/1.jpg", "quality":"80"})";
	char captureBmp[] = R"({"type":"capture", "path":"d:/logs/1.bmp", "quality":"80"})";
	char captureRaw[] = R"({"type":"capture", "path":"d:/logs/1.raw", "quality":"80"})";


	char start1[] = R"({"type":"setMask","enable":"0","x":"0","y":"0","width":"0","height":"0"})";
	char start2[] = R"({"id":"0","wnd":"0","type":"start","left":"800","top":"0","width":"500","height":"281","angle":"0","color":"0","FrameW":"1600","FrameH":"1200","ROIW":"1600","ROIH":"1200","hide":"0"})";

	char start3[] = R"({"type":"setMask","enable":"0","x":"0","y":"0","width":"0","height":"0"})";
	char start4[] = R"({"id":"0","wnd":"0","type":"stop","left":"0","top":"0","width":"500","height":"281","angle":"90","color":"0","FrameW":"1600","FrameH":"1200","ROIW":"1600","ROIH":"1200","hide":"0"})";


	char start5[] = R"({"type":"setMask","enable":"0","x":"0","y":"0","width":"0","height":"0"})";
	char start6[] = R"({"id":"0","wnd":"0","type":"start","left":"0","top":"0","width":"500","height":"281","angle":"90","color":"0","FrameW":"1600","FrameH":"1200","ROIW":"1376","ROIH":"972","hide":"0"})";

	char start7[] = R"({"type":"setMask","enable":"0","x":"0","y":"0","width":"0","height":"0"})";
	char start8[] = R"({"id":"0","wnd":"0","type":"start","left":"0","top":"0","width":"500","height":"281","angle":"90","color":"0","FrameW":"1600","FrameH":"1200","ROIW":"1600","ROIH":"1200","hide":"0"})";


	char start9[] = R"({"type":"setMask","enable":"0","x":"0","y":"0","width":"0","height":"0"})";
	char start10[] = R"({"id":"0","wnd":"0","type":"start","left":"0","top":"0","width":"960","height":"540","angle":"0","color":"0","FrameW":"1600","FrameH":"1200","ROIW":"1600","ROIH":"1200","hide":"0"})";

	char start11[] = R"({"type":"setMask","enable":"0","x":"0","y":"0","width":"0","height":"0"})";
	char start12[] = R"({"id":"0","wnd":"0","type":"start","left":"0","top":"0","width":"960","height":"540","angle":"0","color":"0","FrameW":"1600","FrameH":"1200","ROIW":"1600","ROIH":"1200","hide":"1"})";


	char start13[] = R"({"type":"setMask","enable":"1","x":"522","y":"425","width":"555","height":"350"})";
	char start14[] = R"({"id":"0","wnd":"0","type":"start","left":"0","top":"0","width":"500","height":"281","angle":"90","color":"0","FrameW":"1600","FrameH":"1200","ROIW":"1600","ROIH":"1200","hide":"0"})";


	char start15[] = R"({"type":"setMask","enable":"1","x":"522","y":"425","width":"555","height":"350"})";
	char start16[] = R"({"id":"0","wnd":"0","type":"start","left":"0","top":"0","width":"500","height":"281","angle":"90","color":"0","FrameW":"1600","FrameH":"1200","ROIW":"1600","ROIH":"1200","hide":"1"})";


	char start17[] = R"({"type":"setMask","enable":"1","x":"522","y":"425","width":"555","height":"350"})";
	char start18[] = R"({"id":"0","wnd":"0","type":"start","left":"0","top":"0","width":"500","height":"281","angle":"90","color":"0","FrameW":"1600","FrameH":"1200","ROIW":"1600","ROIH":"1200","hide":"0"})";


	char start19[] = R"({"type":"setMask","enable":"1","x":"522","y":"425","width":"555","height":"350"})";
	char start20[] = R"({"id":"0","wnd":"0","type":"start","left":"0","top":"0","width":"500","height":"281","angle":"90","color":"1","FrameW":"1600","FrameH":"1200","ROIW":"1600","ROIH":"1200","hide":"0"})";
	
	size_t testCount = 1;

	int waiteTime = 2000;
	for (size_t i = 0; i < testCount; i++)
	{
		lpInitCamera();
		
		lpDoProcess(start1);
		lpDoProcess(start2);
		Sleep(waiteTime);

		lpDoProcess(start3);
		lpDoProcess(start4);
		Sleep(waiteTime);

		lpDoProcess(start5);
		lpDoProcess(start6);
		Sleep(waiteTime);

		lpDoProcess(start7);
		lpDoProcess(start8);
		Sleep(waiteTime);

		lpDoProcess(start9);
		lpDoProcess(start10);
		Sleep(waiteTime);

		lpDoProcess(start11);
		lpDoProcess(start12);
		Sleep(waiteTime);

		lpDoProcess(start13);
		lpDoProcess(start14);
		Sleep(waiteTime);

		lpDoProcess(start15);
		lpDoProcess(start16);
		Sleep(waiteTime);

		lpDoProcess(start17);
		lpDoProcess(start18);
		Sleep(waiteTime);

		lpDoProcess(start19);
		lpDoProcess(start20);
		Sleep(waiteTime);

	}

	int a = 0;
	//lpUninitCamera();
	int b = 0;
	while (true)
	{
		Sleep(100);
	}
	return 0;
}

