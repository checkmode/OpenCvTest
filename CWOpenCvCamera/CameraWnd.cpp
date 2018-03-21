#include "stdafx.h"
#include "CameraWnd.h"
#include <opencv2\opencv.hpp>
#include <vector>

#define MAX_LOADSTRING 100
#define IDT_TIMER1 1001
#define IDB_PAUSE   3301    
#define IDB_RESUME  3302    
#define IDB_QUIT   3303 

#define WM_CW_CAMERA_MASK     (WM_USER + 233)
#define WM_CW_CAMERA_START    (WM_USER + 234)
#define WM_CW_CAMERA_STOP     (WM_USER + 235)
#define WM_CW_CAMERA_CAPTURE  (WM_USER + 236)
#define WM_CW_CAMERA_PAUSE    (WM_USER + 237)
#define WM_CW_CAMERA_RESUME   (WM_USER + 238)
#define WM_CW_CAMERA_PROPERTY (WM_USER + 239)


using namespace cv;

const TCHAR CameraWnd::_mainClassName[32] = TEXT("CWCamMainCls");  // the main window class name
const TCHAR CameraWnd::_previewClassName[32] = TEXT("CWCamPreviewCls");  // the main window class name

CameraWnd::CameraWnd(std::shared_ptr<spdlog::logger> pLog)
	: _pLog(pLog)
	, _hInst(NULL)
	, _hParent(NULL)
	, m_hWnd(NULL)
	, m_hPreviewWnd(NULL)
	, _mask(false)
	, _show(true)
	, _gray(true)
	, _preview(false)
	, _matPreview(false)
	, _angle(0)
	, _run(true)
	, _id(-1)
    , _frameW(640)
    , _frameH(480)
	, m_hAsyncEventHandle(INVALID_HANDLE_VALUE)
{
	_rectROI = cv::Rect(0, 0, 200, 200);
	m_hAsyncEventHandle = CreateEvent(NULL, FALSE, FALSE, _T("CameraOperationDone"));
#ifdef DEBUG
	_matPreview = false;
#endif // DEBUG
}

CameraWnd::~CameraWnd()
{
	UninitWnd();
}

void CameraWnd::InitWnd()
{
	_pLog->info("InitWnd begin");
	_hInst = NULL;
	_hParent = NULL;
	m_hWnd = NULL;
	m_hPreviewWnd = NULL;
	_mask = false;
	_preview = false;
	_id = -1;
	_angle = 0;
	_gray = false;
	_show = false;
	_run = false;

	ResetEvent(m_hAsyncEventHandle);

	// 启动界面线程
	DWORD dwThreadId = 0;
	HANDLE hThread = CreateThread(NULL, 0, AsyncConstructCameraWnd, this, 0, &dwThreadId);
	_pLog->info("AsyncConstructCameraWnd thread start! threadID={}", dwThreadId);
	DWORD waitRet = WaitForSingleObject(m_hAsyncEventHandle, 10000);

	// 启动处理视频帧线程
	DWORD dwThreadId1 = 0;
	//HANDLE hThreadWork = CreateThread(NULL, 0, AsyncWork, this, 0, &dwThreadId1);

	_pLog->info("InitWnd end");
}

void CameraWnd::UninitWnd()
{
	_pLog->info("UninitWnd begin");
	if (NULL != m_hWnd)
	{
		_pLog->info("UninitWnd do");

		KillTimer(m_hWnd, IDT_TIMER1);
		ResetEvent(m_hAsyncEventHandle);
		PostMessage(m_hWnd, WM_DESTROY, NULL, NULL);
		WaitForSingleObject(m_hAsyncEventHandle, INFINITE);
		m_hWnd = NULL;
	}
	_pLog->info("UninitWnd end");
}

bool CameraWnd::CameraMask(LPCAM_MASK lpCamMask)
{
	return CameraDo(WM_CW_CAMERA_MASK, (LPARAM)lpCamMask);
}

bool CameraWnd::CameraStart(LPCAM_START lpCamStart)
{
	_pLog->info("CameraStart begin");
	bool bOk = CameraDo(WM_CW_CAMERA_START, (LPARAM)lpCamStart);
	_pLog->info("CameraStart end");
	return bOk;
}

bool CameraWnd::CameraStop()
{
	return CameraDo(WM_CW_CAMERA_STOP, NULL);
}

bool CameraWnd::CameraCapture(LPCAM_CAPTURE lpCamCapture)
{
	return CameraDo(WM_CW_CAMERA_CAPTURE, (LPARAM)lpCamCapture);
}

bool CameraWnd::CameraPause()
{
	return CameraDo(WM_CW_CAMERA_PAUSE, NULL);
}

bool CameraWnd::CameraResume()
{
	return CameraDo(WM_CW_CAMERA_RESUME, NULL);
}

bool CameraWnd::CameraProperty()
{
	return CameraDo(WM_CW_CAMERA_PROPERTY, NULL);
}

DWORD WINAPI CameraWnd::AsyncConstructCameraWnd(LPVOID lpParam)
{
	CameraWnd* pImpl = (CameraWnd*)lpParam;
	pImpl->ConstructCameraWnd();
	return 0;
}

DWORD WINAPI CameraWnd::AsyncWork(LPVOID lpParam)
{
	CameraWnd* pImpl = (CameraWnd*)lpParam;
	pImpl->StartWorking();
	return 0;
}

int CameraWnd::StartWorking()
{
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

int CameraWnd::ConstructCameraWnd()
{
	HINSTANCE hinstance = NULL;
	MyRegisterClass(hinstance);

	int cmdShow = SW_HIDE;
#ifdef _DEBUG
	cmdShow = SW_SHOW;
#endif
	InitInstance(hinstance, cmdShow);

	MSG msg;
	BOOL fGotMessage;
	while ((fGotMessage = GetMessage(&msg, (HWND)NULL, 0, 0)) != 0 && fGotMessage != -1)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	msg.wParam;
	SetEvent(m_hAsyncEventHandle);

	_pLog->info("AsyncConstructCameraWnd thread end");
	return 0;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM CameraWnd::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcx;

	// Fill in the window class structure with parameters 
	// that describe the main window. 

	wcx.cbSize = sizeof(wcx);          // size of structure 
	wcx.style = CS_HREDRAW |
		CS_VREDRAW;                    // redraw if size changes 
	wcx.lpfnWndProc = WndProc;     // points to window procedure 
	wcx.cbClsExtra = 0;                // no extra class memory 
	wcx.cbWndExtra = 0;                // no extra window memory 
	wcx.hInstance = hInstance;         // handle to instance 
	wcx.hIcon = LoadIcon(NULL,IDI_APPLICATION); // predefined app. icon 
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);    // predefined arrow 
	wcx.hbrBackground = (HBRUSH)GetStockObject(
		WHITE_BRUSH);                  // white background brush 
	wcx.lpszMenuName = _T("MainMenu");    // name of menu resource 
	wcx.lpszClassName = _mainClassName;  // name of window class 
	wcx.hIconSm = (HICON)LoadImage(hInstance, // small class icon 
		MAKEINTRESOURCE(5),
		IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CYSMICON),
		LR_DEFAULTCOLOR);

	ATOM reg = RegisterClassEx(&wcx);
	if (TRUE)
	{
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = PreviewDlgProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION); // predefined app. icon 
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);    // predefined arrow 
		wcex.hbrBackground = (HBRUSH)GetStockObject(
			WHITE_BRUSH);                  // white background brush 
		wcex.lpszMenuName = _T("MainMenu");    // name of menu resource 
		wcex.lpszClassName = _previewClassName;  // name of window class 
		wcex.hIconSm = (HICON)LoadImage(hInstance, // small class icon 
			MAKEINTRESOURCE(5),
			IMAGE_ICON,
			GetSystemMetrics(SM_CXSMICON),
			GetSystemMetrics(SM_CYSMICON),
			LR_DEFAULTCOLOR);

		reg = RegisterClassEx(&wcex);
	}

	return reg;
}

namespace // anonymous
{
	struct PaintLocker final
	{
		explicit PaintLocker(HWND handle)
			: handle(handle)
		{
			// disallow drawing on the window
			LockWindowUpdate(handle);
		}

		~PaintLocker()
		{
			// re-allow drawing for the window
			LockWindowUpdate(NULL);

			// force re-draw
			InvalidateRect(handle, nullptr, TRUE);
			RedrawWindow(handle, nullptr, NULL, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE);
		}

		HWND handle;
	};

} // anonymous namespace

  //
  //   FUNCTION: InitInstance(HINSTANCE, int)
  //
  //   PURPOSE: Saves instance handle and creates main window
  //
  //   COMMENTS:
  //
  //        In this function, we save the instance handle in a global variable and
  //        create and display the main program window.
  //
BOOL CameraWnd::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	_hInst = hInstance; // Store instance handle in our global variable

	m_hWnd = ::CreateWindowEx(
		WS_EX_ACCEPTFILES,
		_mainClassName,
		TEXT("CameraControl"),
		(WS_OVERLAPPEDWINDOW),
		// CreateWindowEx bug : set all 0 to walk around the pb
		0, 0, 0, 0,
		_hParent, nullptr, _hInst,
		(LPVOID)this); // pass the ptr of this instantiated object
					   // for retrieve it in WndProc from
					   // the CREATESTRUCT.lpCreateParams afterward.

	if (NULL == m_hWnd)
	{
		return FALSE;
	}

	PaintLocker paintLocker{ m_hWnd };

	::MoveWindow(m_hWnd, 800, 100, 300, 200, TRUE);

	ShowWindow(m_hWnd, nCmdShow);
	SetTimer(m_hWnd, IDT_TIMER1, 50, NULL);

	return TRUE;
}

void CameraWnd::addControl(HWND hWnd)
{
	HWND hWndPauseBtn = CreateWindow(TEXT("Button"), TEXT("Pause"), WS_CHILD,
		10, 100, 60, 30, hWnd, (HMENU)IDB_PAUSE, _hInst, nullptr);
	ShowWindow(hWndPauseBtn, SW_SHOW);
	UpdateWindow(hWndPauseBtn);

	HWND hWndResumeBtn = CreateWindow(TEXT("Button"), TEXT("Resume"), WS_CHILD,
		75, 100, 60, 30, hWnd, (HMENU)IDB_RESUME, _hInst, nullptr);

	ShowWindow(hWndResumeBtn, SW_SHOW);
	UpdateWindow(hWndResumeBtn);

	if (true)
	{
		m_hPreviewWnd = CreateWindow(_previewClassName, TEXT("Preview"), WS_POPUP,
			0, 50, 700, 600, hWnd, nullptr, _hInst, this);

		if (!m_hPreviewWnd)
		{
			return;
		}
	}
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK CameraWnd::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (hWnd == NULL)
		return FALSE;

	switch (message)
	{
	case WM_NCCREATE:
	{
		// First message we get the ptr of instantiated object
		// then stock it into GWLP_USERDATA index in order to retrieve afterward
		CameraWnd *pM30ide = static_cast<CameraWnd *>((reinterpret_cast<LPCREATESTRUCT>(lParam))->lpCreateParams);
		pM30ide->m_hWnd = hWnd;
		::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pM30ide));
		return TRUE;
	}

	default:
	{
		return (reinterpret_cast<CameraWnd *>(::GetWindowLongPtr(hWnd, GWLP_USERDATA))->runProc(hWnd, message, wParam, lParam));
	}
	}
}

//
//  FUNCTION: DlgProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK CameraWnd::PreviewDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


void CameraWnd::OnTimer(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case IDT_TIMER1:
	{
		if (_videoCapture.isOpened())
		{
            // 英文系统的摄像头属性面板窗口名称为 "USB Camera Properties"
			HWND hwndProperty = FindWindow(TEXT("#32770"), TEXT("USB Camera Properties"));
            if (NULL == hwndProperty)
            {
                // 中文系统的摄像头属性面板窗口名称为 "USB Camera 属性"
                hwndProperty = FindWindow(TEXT("#32770"), TEXT("USB Camera 属性"));
            }
            if (NULL != hwndProperty)
            {
                HWND hForeWnd = ::GetForegroundWindow();
                DWORD dwForeID = ::GetWindowThreadProcessId(hForeWnd, NULL);
                DWORD dwCurID = ::GetCurrentThreadId();
                ::AttachThreadInput(dwCurID, dwForeID, TRUE);
                ::ShowWindow(hwndProperty, SW_SHOWNORMAL);
                ::SetForegroundWindow(hwndProperty);
                ::SetWindowPos(hwndProperty, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
                ::AttachThreadInput(dwCurID, dwForeID, FALSE);
            }

			//_pLog->info("OnTimer read begin");
			bool bSuccess = _videoCapture.read(_frame);
			//_pLog->info("OnTimer read end");
			if (bSuccess)
			{
				if (_preview)
				{
					//_pLog->info("OnTimer handle img begin");

					if (_matPreview)
						imshow("live", _frame);
					
					_frameROI = _frame;
					if (!_rectROI.empty())
					{
						if (_mask)
						{
							// 标记 ROI 区域
							int lineType = LINE_AA; // change it to LINE_8 to see non-antialiased graphics
							rectangle(_frame, _rectROI, Scalar(0, 0, 255), 3);
						}
						_frameROI = _frame(_rectROI);
						if (_matPreview)
							imshow("ROI", _frameROI);
					}

					Mat dstImg;
					Mat tmpImg;
					//_pLog->info("OnTimer focus img begin");
					if (_mask)
					{
						// 显示视频所有区域，ROI 区域完全显示，非 ROI 区域以遮罩的形式显示
						// do focus
						_frame.copyTo(tmpImg);
						focusImg(tmpImg, _rectROI, dstImg, 0.2);
						if (_matPreview)
							imshow("focus", dstImg);
					}
					else
					{
						// 仅仅显示 ROI 区域，非 ROI 区域不显示
						_frameROI.copyTo(dstImg);
					}
					dstImg.copyTo(tmpImg);
					//_pLog->info("OnTimer focus img end");

					// rotate image with angle
					if (0 != _angle)
					{
						//_pLog->info("OnTimer rotateImg img begin");
						rotateImg(tmpImg, dstImg, _angle);
						dstImg.copyTo(tmpImg);
						if (_matPreview)
							imshow("rotate", dstImg);
						//_pLog->info("OnTimer rotateImg img end");
					}

					// convert to gray color
					if (_gray)
					{
						//_pLog->info("OnTimer _gray img begin");
						cvtColor(tmpImg, dstImg, CV_BGR2GRAY);
						dstImg.copyTo(tmpImg);
						if (_matPreview)
							imshow("gray", dstImg);
						//_pLog->info("OnTimer _gray img end");
					}
					//_pLog->info("OnTimer copyMatToHwnd img begin");
					copyMatToHwnd(dstImg, m_hPreviewWnd);
					dstImg.copyTo(_finalImg);
					//_pLog->info("OnTimer copyMatToHwnd img end");
					//_pLog->info("OnTimer handle img end");
				}
				else
				{
					ShowWindow(m_hPreviewWnd, SW_HIDE);
				}
			}
			else
			{
				_pLog->error("read frame failed! auto release camera");
				_videoCapture.release();

                _pLog->info("prepare to reopen camera and setFrame");
                int retryCount = 0;
                bool open = false;
                while (!open && retryCount < 20)
                {
                    _pLog->info("retryCount={}", retryCount);
                    open = openCamera(_id);
                    if (open)
                        setFrame(_frameW, _frameH);
                    else
                        Sleep(500);
                    ++retryCount;
                }
			}
		}
		else
		{
			if (_id != -1)
			{
				ShowWindow(m_hPreviewWnd, SW_HIDE);
			}
		}
		break;
	}
	default:
		break;
	}
}

void CameraWnd::OnCameraStart(LPCAM_START lpCamStart)
{
	_pLog->info("OnCameraStart");

	if (openCamera(lpCamStart->id))
	{
		// 设置摄像头的硬件 FRAM 属性
		setFrame(lpCamStart->frameW, lpCamStart->frameH);
	}

	_angle = lpCamStart->angle;
	_gray = (lpCamStart->color == 0) ? false : true;

	// set ROI area
	if (_mask)
		setROI(_camMask.x, _camMask.y, _camMask.width, _camMask.height, false);
	else
		setROI(0, 0, lpCamStart->roiW, lpCamStart->roiH, true);

	SetWindowPos(m_hPreviewWnd, HWND_TOPMOST, lpCamStart->left, lpCamStart->top, lpCamStart->width, lpCamStart->height, NULL);
	if (0 == lpCamStart->hide)
		_show = true;
	else
		_show = false;
}

void CameraWnd::OnCameraMask(LPCAM_MASK lpCamMask)
{
	_pLog->info("OnCameraMask");
	if (1 == lpCamMask->enable)
	{
		_mask = true;
		_camMask.x = lpCamMask->x;
		_camMask.y = lpCamMask->y;
		_camMask.width = lpCamMask->width;
		_camMask.height = lpCamMask->height;
	}
	else
	{
		_mask = false;
	}
}

void CameraWnd::OnCameraCapture(LPCAM_CAPTURE lpCamCapture)
{
	_pLog->info("OnCameraStart");
	bool bOk = false;
	if (openCamera(lpCamCapture->id))
	{
		// 设置摄像头的硬件 FRAM 属性
		setFrame(lpCamCapture->frameW, lpCamCapture->frameH);

		bOk = saveImg(lpCamCapture->path, _finalImg, lpCamCapture->quality);
	}

	// [TODO] handle text
	// [TODO] handle hPixel and vPixel
}

LRESULT CameraWnd::workingProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = FALSE;

	switch (message)
	{

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return result;

}

LRESULT CameraWnd::process(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = FALSE;

	switch (message)
	{
	case WM_CW_CAMERA_MASK:
	{
		OnCameraMask((LPCAM_MASK)lParam);
		SetEvent(m_hAsyncEventHandle);
		break;
	}
	case WM_CW_CAMERA_START:
	{
		OnCameraStart((LPCAM_START)lParam);
		_preview = true;
		SetEvent(m_hAsyncEventHandle);
		break;
	}
	case WM_CW_CAMERA_STOP:
	{
		_pLog->info("WM_CW_CAMERA_STOP");
		ShowWindow(m_hPreviewWnd, SW_HIDE);
		_preview = false;
		if (_videoCapture.isOpened())
			_videoCapture.release();

		SetEvent(m_hAsyncEventHandle);
		break;
	}
	case WM_CW_CAMERA_CAPTURE:
	{
		OnCameraCapture((LPCAM_CAPTURE)lParam);
		SetEvent(m_hAsyncEventHandle);
		break;
	}
	case WM_CW_CAMERA_PAUSE:
	{
		_pLog->info("WM_CW_CAMERA_PAUSE");
		_preview = false;

		SetEvent(m_hAsyncEventHandle);
		break;
	}
	case WM_CW_CAMERA_RESUME:
	{
		_pLog->info("WM_CW_CAMERA_RESUME");
		_preview = true;

		SetEvent(m_hAsyncEventHandle);
		break;
	}
	case WM_CW_CAMERA_PROPERTY:
	{
		_pLog->info("WM_CW_CAMERA_PROPERTY");
		if (_videoCapture.isOpened())
			_videoCapture.set(CV_CAP_PROP_SETTINGS, 0);
		SetEvent(m_hAsyncEventHandle);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return result;
}

LRESULT CameraWnd::runProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDB_PAUSE:
			::PostMessage(hWnd, WM_CW_CAMERA_PAUSE, NULL, NULL);
			break;
		case IDB_RESUME:
			::PostMessage(hWnd, WM_CW_CAMERA_RESUME, NULL, NULL);
			break;
		case IDB_QUIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	case WM_TIMER:
	{
		OnTimer(wParam, lParam);
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_CREATE:
	{
		addControl(hWnd);
		SetEvent(m_hAsyncEventHandle);
		break;
	}
	case WM_DESTROY:
	{
        _pLog->info("WM_DESTROY");

		PostQuitMessage(0);
		break;
	}
	default:
		return process(hWnd, message, wParam, lParam);
	}
	return 0;
}

void CameraWnd::setROI(int x, int y, int w, int h, bool center)
{
	_pLog->info("setROI({}, {}, {}, {}, {})", x, y, w, h, center);
	double dx = (double)x;
	double dy = (double)y;
	double dw = (double)w;
	double dh = (double)h;
	double frameW = _videoCapture.get(CV_CAP_PROP_FRAME_WIDTH);
	double frameH = _videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT);
	_pLog->info("frameW={}, frameH={}", frameW, frameH);
	if (center)
	{
		dw = (w == 0) ? frameW : MIN(w, frameW);
		dx = (frameW - dw) / 2;
		dh = (h == 0) ? frameH : MIN(h, frameH);
		dy = (frameH - dh) / 2;
	}

	double x1 = MAX(dx, 0);
	double y1 = MAX(dy, 0);

	double x2 = MIN(dx + dw, frameW);
	double y2 = MIN(dy + dh, frameH);
	if (x2 > x1 && y2 > y1)
	{
		_rectROI.x = (int)x1;
		_rectROI.y = (int)y1;
		_rectROI.width = (int)(x2 - x1);
		_rectROI.height = (int)(y2 - y1);
	}
	else
	{
		_rectROI.x = 0;
		_rectROI.y = 0;
		_rectROI.width = (int)frameW;
		_rectROI.height = (int)frameH;
	}
	_pLog->info("_rectROI({}, {}, {}, {})", _rectROI.x, _rectROI.y, _rectROI.width, _rectROI.height);
}

bool CameraWnd::CameraDo(UINT message, LPARAM lParam)
{
	ResetEvent(m_hAsyncEventHandle);
	SendMessage(m_hWnd, message, NULL, lParam);

	DWORD waitRet = WaitForSingleObject(m_hAsyncEventHandle, 20000);
	if (WAIT_OBJECT_0 == waitRet)
		return true;
	return false;
}

bool CameraWnd::saveImg(std::string path, Mat & img, int quality)
{
	_pLog->info("saveImg path = {}, quality = {}", path.c_str(), quality);
	// 检查图片是否为空
	if (img.empty())
	{
		_pLog->error("saveImg the src image is empty!");
		return false;
	}

	Mat dst;
	if (_mask && !_rectROI.empty())
	{
		Mat roi = img(_rectROI);
		roi.copyTo(dst);
	}
	else
	{
		img.copyTo(dst);
	}

	bool bOk = false;

	// 获取目标文件的扩展名
	size_t dotPos = path.find_last_of('.');
	if (dotPos == std::string::npos || dotPos == path.length())
		return false;
	int extBeginPos = dotPos + 1;
	int extLen = path.length() - extBeginPos;
	std::string ext = path.substr(extBeginPos, extLen);

	if (0 == ext.compare("jpg"))
	{
		// param about to change image quality to make picture smaller.
		std::vector<int> compression_params;
		compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
		compression_params.push_back(quality);

		// save the picture on hard disk.
		bOk = cv::imwrite(path.data(), dst, compression_params);
	}
	else if (0 == ext.compare("raw"))
	{
		FILE* pFile = NULL;
		errno_t openErr = fopen_s(&pFile, path.c_str(), "wb");
		if (!pFile)
		{
			_pLog->error("saveImg open file failed({})", openErr);
			return false;
		}

		// accept only char type matrices
		CV_Assert(dst.depth() == CV_8U);
		int channels = dst.channels();
		int nRows = dst.rows;
		int nCols = dst.cols * channels;
		if (dst.isContinuous())
		{
			nCols *= nRows;
			nRows = 1;
		}
		int i;
		uchar* p;
		long pos = 0;
		for (i = 0; i < nRows; ++i)
		{
			p = dst.ptr<uchar>(i);
			fwrite(p, sizeof(uchar), nCols, pFile);
		}
		fclose(pFile);
		bOk = true;
	}
	else
	{
		// save the picture on hard disk.
		bOk = cv::imwrite(path.data(), dst);
	}
	return bOk;
}

void CameraWnd::rotateImg(cv::Mat & src, cv::Mat& dest, int angle)
{
	double dAngle = (double)angle;

	float radian = (float)(angle / 180.0 * CV_PI);

	// 填充图像使其符合旋转要求
	int uniSize = (int)(MAX(src.cols, src.rows)* 1.414);
	int dx = (int)(uniSize - src.cols) / 2;
	int dy = (int)(uniSize - src.rows) / 2;

	_pLog->info("copyMakeBorder begin");
	Mat tempImg;
	copyMakeBorder(src, tempImg, dy, dy, dx, dx, BORDER_CONSTANT);
	_pLog->info("copyMakeBorder end");

	// 旋转中心
	Point2f center((float)(tempImg.cols / 2), (float)(tempImg.rows / 2));
	Mat affine_matrix = getRotationMatrix2D(center, dAngle, 1.0);

	_pLog->info("warpAffine begin");
	// 旋转
	cv::Mat rImg;
	warpAffine(tempImg, rImg, affine_matrix, tempImg.size());
	_pLog->info("warpAffine end");

	// 计算旋转后的矩形大小
	float sinVal = fabs(sin(radian));
	float cosVal = fabs(cos(radian));

	Size targetSize((int)(src.cols * cosVal + src.rows * sinVal),
		(int)(src.cols * sinVal + src.rows * cosVal));

	//剪掉四周边框
	int x = (rImg.cols - targetSize.width) / 2;
	int y = (rImg.rows - targetSize.height) / 2;

	Rect rect(x, y, targetSize.width, targetSize.height);
	Mat rotateROIImg = Mat(rImg, rect);
	
	rotateROIImg.copyTo(dest);
}

void CameraWnd::focusImg(cv::Mat & src, cv::Rect & rect, cv::Mat& dest, double alpha)
{
	// 填充掩码图像
	cv::Mat mask = cv::Mat::zeros(src.size(), src.type());
	rectangle(mask, rect, Scalar(255, 255, 255), -1);

	cv::Mat focus_img = cv::Mat::zeros(src.size(), src.type());
	src.copyTo(focus_img, mask);

	double beta = (1 - alpha);
	_pLog->info("addWeighted begin");
	addWeighted(src, alpha, focus_img, beta, 0.0, dest);
	_pLog->info("addWeighted end");
}

void CameraWnd::copyMatToHwnd(cv::Mat& src, HWND hWnd)
{
	// 获取窗口大小，并计算 Mat 图像显示在窗口上时将要缩放的比率
	RECT rect = {};
	GetWindowRect(hWnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	// 宽度比率
	double dW = (double)width / src.cols;
	// 高度比率
	double dH = (double)height / src.rows;

	// 对 Mat 图像进行缩放，比便后面将其显示在 hWnd 窗口上
	// 尽可能的按照和原图相一致的宽高比来缩放
	double scale = MIN(dW, dH);  // 保持相同的宽高比
	Size dsize = Size((int)(src.cols*scale), (int)(src.rows*scale));
	Mat dst = Mat(dsize, CV_8U);
	resize(src, dst, dsize);

	// 利用输入图像生成一个4通道的图像
	Mat dst4Channels(dst.rows, dst.cols, CV_8UC4);
	Mat alpha(dst.rows, dst.cols, CV_8UC1);
	Mat in[4] = { dst, alpha, dst, dst };
	int inputCount = 2;
	if (1 == dst.channels())
	{
		inputCount = 4;
		in[1] = dst;
	}
	int from_to[] = { 0, 0, 1, 1, 2, 2, 3, 3 };
	mixChannels(in, inputCount, &dst4Channels, 1, from_to, 4);
	if (_matPreview)
		imshow("dst4Channels", dst4Channels);

	// 计算将要画在 hWnd 窗口的位置
	int x = (width - dst4Channels.cols) / 2;
	x = (x > 0) ? x : 0;
	int y = (height - dst4Channels.rows) / 2;
	y = (y > 0) ? y : 0;

	
	Mat tempImg;
	copyMakeBorder(dst4Channels, tempImg, y, y, x, x, BORDER_CONSTANT, Scalar(255, 255, 255, 255));

	int channels = dst4Channels.channels();
	BITMAPINFO bmi;
	int bmihsize = sizeof(BITMAPINFOHEADER);
	memset(&bmi, 0, bmihsize);

	BITMAPINFOHEADER &h = bmi.bmiHeader;
	h.biSize = bmihsize;
	h.biWidth = tempImg.cols;
	h.biHeight = -tempImg.rows;
	h.biPlanes = 1;
	h.biBitCount = 8 * tempImg.channels();
	h.biCompression = BI_RGB;

	HDC hdc = GetDC(hWnd);
	StretchDIBits(
		hdc,
		0, 0, tempImg.cols, tempImg.rows,
		0, 0, tempImg.cols, tempImg.rows,
		tempImg.data,
		(BITMAPINFO*)&bmi,
		DIB_RGB_COLORS,
		SRCCOPY);

	ReleaseDC(hWnd, hdc);
	if (_show)
		ShowWindow(m_hPreviewWnd, SW_SHOW);
	else
		ShowWindow(m_hPreviewWnd, SW_HIDE);
	UpdateWindow(m_hPreviewWnd);
}

bool CameraWnd::openCamera(int id)
{
	bool bOk = true;
	if (_id != id)
	{
		if (_videoCapture.isOpened())
		{
			_videoCapture.release();
			_pLog->info("_videoCapture release id = {}", _id);
		}

		bOk = _videoCapture.open(id);
		_pLog->info("_videoCapture open({}) = {}", id, bOk);
		if (bOk)
			_id = id;
	}
	else
	{
		if (!_videoCapture.isOpened())
		{
			bOk = _videoCapture.open(_id);
			_pLog->info("_videoCapture.open({}) = {}", _id, bOk);
		}
	}
    if (bOk)
    {
        Mat frame;
        bool read = _videoCapture.read(frame);
        _pLog->info("_videoCapture read 1 frame ater open");
        if (frame.empty() || !read)
            _pLog->error("_videoCapture read 1 frame failed, read={}", read);
    }
	return bOk;
}

void CameraWnd::setFrame(int w, int h)
{
	_pLog->info("prepare setFrame({}, {})", w, h);
	if (w > 0 && h > 0)
	{
		int setCount = 0;
		bool setOk = false;
        // make sure the setting works.
		while (!setOk && setCount < 10)
		{
            _frameW = _videoCapture.get(CV_CAP_PROP_FRAME_WIDTH);
            _frameH = _videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT);
			if (w == _frameW && h == _frameH)
				setOk = true;

			if (!setOk)
			{
                // Get Codec Type- Int form
                int ex = static_cast<int>(_videoCapture.get(CV_CAP_PROP_FOURCC));     
                // Transform from int to char via Bitwise operators
                char EXT[] = { (char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0 };
                _pLog->info("get CV_CAP_PROP_FOURCC={}", EXT);

                int fps = _videoCapture.get(CV_CAP_PROP_FPS);
                _pLog->info("get CV_CAP_PROP_FPS={}", fps);
                //if (0 != memcmp(EXT, "YUY2", 4))
                {
                    _videoCapture.set(CV_CAP_PROP_FOURCC,CV_FOURCC('Y','U','Y','2'));
                    _videoCapture.set(CV_CAP_PROP_FPS, 30);

                    int ex1 = static_cast<int>(_videoCapture.get(CV_CAP_PROP_FOURCC));
                    // Transform from int to char via Bitwise operators
                    char EXT1[] = { (char)(ex1 & 0XFF) , (char)((ex1 & 0XFF00) >> 8),(char)((ex1 & 0XFF0000) >> 16),(char)((ex1 & 0XFF000000) >> 24), 0 };
                    _pLog->info("ater set CV_CAP_PROP_FOURCC={}", EXT1);
                    fps = _videoCapture.get(CV_CAP_PROP_FPS);
                    _pLog->info("ater set CV_CAP_PROP_FPS={}", fps);
                }

				_videoCapture.set(CV_CAP_PROP_FRAME_WIDTH, w);
				_videoCapture.set(CV_CAP_PROP_FRAME_HEIGHT, h);
				_pLog->info("setCount={}, CV_CAP_PROP_FRAME_WIDTH={}, CV_CAP_PROP_FRAME_HEIGHT={}",
					setCount, _videoCapture.get(CV_CAP_PROP_FRAME_WIDTH), _videoCapture.get(CV_CAP_PROP_FRAME_HEIGHT));
                Sleep(100);
			}
			++setCount;
		}
	}
	else
	{
		_pLog->error("setFrame parms is invalid");
	}
}

