#pragma once
#include <opencv2/videoio.hpp>

#include <string>
typedef struct _CAM_START
{
	int left;
	int top;
	int width;
	int height;

	int id;
	int angle;
	int color;
	int frameW;
	int frameH;
	int roiW;
	int roiH;
	HWND hwnd;
	int hide;
}CAM_START, *LPCAM_START;

typedef struct __CAM_CAPTURE
{
	std::string path;

	int id;
	int quality;
	std::string text;
	int hPixel;
	int vPixel;
	int frameW;
	int frameH;
}CAM_CAPTURE, *LPCAM_CAPTURE;

typedef struct __CAM_MASK
{
	int enable;  // 0: normal, 1: mask
	int x;
	int y;
	int width;
	int height;
}CAM_MASK, *LPCAM_MASK;

class CameraWnd
{
public:
	CameraWnd(std::shared_ptr<spdlog::logger> pLog);
	virtual ~CameraWnd();

public:
	void InitWnd();
	void UninitWnd();
	bool CameraMask(LPCAM_MASK lpCamMask);
	bool CameraStart(LPCAM_START lpCamStart);
	bool CameraStop();
	bool CameraCapture(LPCAM_CAPTURE lpCamCapture);
	bool CameraPause();
	bool CameraResume();
	bool CameraProperty();


private:
	int ConstructCameraWnd();
	int StartWorking();
	
	void addControl(HWND hWnd);
	ATOM MyRegisterClass(HINSTANCE hInstance);
	BOOL InitInstance(HINSTANCE, int);
	static DWORD WINAPI AsyncConstructCameraWnd(LPVOID lpParam);
	static DWORD WINAPI AsyncWork(LPVOID lpParam);
	static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK PreviewDlgProc(HWND, UINT, WPARAM, LPARAM);

	LRESULT workingProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT process(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT runProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	void setROI(int x, int y, int w, int h, bool center = false);
	bool CameraDo(UINT message, LPARAM lParam);
	bool saveImg(std::string path, cv::Mat& img, int quality);
	void rotateImg(cv::Mat& img, cv::Mat& dest, int angle);
	void focusImg(cv::Mat& src, cv::Rect& rect, cv::Mat& dest, double alpha);
	void copyMatToHwnd(cv::Mat& src, HWND hWnd);
	bool openCamera(int id);
	void setFrame(int w, int h);

	void OnTimer(WPARAM wParam, LPARAM lParam);
	void OnCameraStart(LPCAM_START lpCamStart);
	void OnCameraMask(LPCAM_MASK lpCamMask);
	void OnCameraCapture(LPCAM_CAPTURE lpCamCapture);

protected:
	HINSTANCE _hInst;
	HWND _hParent;
	HWND m_hWnd;
	HWND m_hPreviewWnd;

private:
	std::shared_ptr<spdlog::logger> _pLog;
	cv::Mat _frame;
	cv::Mat _frameROI;
	cv::Mat _finalImg;
	cv::Rect _rectROI;
	Mutex _imgMutex;
	int _angle;
	bool _mask;
	bool _show;
	CAM_MASK _camMask;
	bool _gray;
	bool _preview;
	bool _matPreview;
	bool _run;
	int _id;
	static const TCHAR _mainClassName[32];
	static const TCHAR _previewClassName[32];
	HANDLE m_hAsyncEventHandle;
	cv::VideoCapture _videoCapture;
};

