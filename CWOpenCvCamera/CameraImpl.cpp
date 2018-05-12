#include "stdafx.h"
#include "CameraImpl.h"
#include "CameraWnd.h"

#include <iostream> // for standard I/O
#include <string>   // for strings
#include <iomanip>  // for controlling float print precision
#include <sstream>  // string to number conversion
using namespace std;

#include "rapidjson/prettywriter.h" // for stringify JSON

CameraImpl::CameraImpl()
	: m_pCameraWnd(NULL)
{
	size_t q_size = 4096; //queue size must be power of 2
	spdlog::set_async_mode(q_size, spdlog::async_overflow_policy::block_retry,
		nullptr,
		std::chrono::seconds(2));
	spdlog::set_pattern("[%H:%M:%S.%e] [%L][thread %t] %v");
	CreateDirectory(TEXT("D:\\logs\\"), NULL);
	_pLog = spdlog::rotating_logger_mt("CWOpenCvCameraLog", "D:\\logs\\CWOpenCvCameraLog.txt", 1048576 * 5, 3);
	_pLog->flush_on(spdlog::level::debug);
	_pLog->info("=============================================================================================");
	m_pCameraWnd = new CameraWnd(_pLog);
}

CameraImpl::~CameraImpl()
{
	delete m_pCameraWnd;
	m_pCameraWnd = NULL;
	_pLog->flush();
	spdlog::drop_all();
}

void CameraImpl::Init()
{
	_pLog->info("      ");
	_pLog->info("-->[Entry] InitCamera()");
	m_pCameraWnd->InitWnd();
	_pLog->info("<--[Leave] InitCamera()");
}


bool CameraImpl::DoProcess(char * cmd)
{
	_pLog->info("      ");
	_pLog->info("-->[Entry] DoProcess({})", cmd);

	bool doOk = false;
	////////////////////////////////////////////////////////////////////////////
	// 1. Parse a JSON text string to a document.
	
	printf("Original JSON:\n %s\n", cmd);
	
	// Default template parameter uses UTF8 and MemoryPoolAllocator.
	Document document;
	if (document.Parse(cmd).HasParseError())
	{
		_pLog->error("Document HasParseError, ParseErrorCode={}, error pos={}", document.GetParseError(), document.GetErrorOffset());
		doOk = false;
	}
	else
	{
		printf("\nParsing to document succeeded.\n");

		////////////////////////////////////////////////////////////////////////////
		// 2. Access values in document. 

		printf("\nAccess values in document:\n");
		assert(document.IsObject());    // Document is a JSON value represents the root of DOM. Root can be either an object or array.

		assert(document.HasMember("type"));
		assert(document["type"].IsString());
		printf("type = %s\n", document["type"].GetString());

		// Since version 0.2, you can use single lookup to check the existing of member and its value:
		Value::MemberIterator typeObj = document.FindMember("type");
		if (typeObj != document.MemberEnd() && typeObj->value.IsString())
			doOk = doProcess(typeObj->value.GetString(), &document);
		else
			_pLog->error("Document FindMember 'type' failed!");
	}
	_pLog->info("<--[Leave] DoProcess");
	return doOk;
}

void CameraImpl::Uninit()
{
	_pLog->info("      ");
	_pLog->info("-->[Entry] Uninit");
	m_pCameraWnd->UninitWnd();
	_pLog->info("<--[Leave] Uninit");
}

bool CameraImpl::doProcess(const char * type, Document* pDoc)
{
	_pLog->info("doProcess type: {}", type);
	bool doOk = false;

	// 预览窗口相关控制
	if (strcmp("start", type) == 0)
	{
		if (pDoc->HasMember("left") && pDoc->HasMember("top") && pDoc->HasMember("width") && pDoc->HasMember("height"))
			doOk = doCameraStart(pDoc);
		else
			_pLog->error("doProcess {} some required member not found!", type);
	}

	// 停止预览摄像头
	if (strcmp("stop", type) == 0)
		doOk = doCameraStop();

	// 拍照(无论是否有预览都应拍照成功)
	if (strcmp("capture", type) == 0)
	{
		if (pDoc->HasMember("path"))
			doOk = doCameraCapture(pDoc);
		else
			_pLog->error("doProcess {} some required member not found!", type);
	}

	// 暂停预览画面
	if (strcmp("pause", type) == 0)
		doOk = doCameraPause();

	// 恢复预览画面
	if (strcmp("resume", type) == 0)
		doOk = doCameraResume();

	// 显示摄像头的属性控制面板
	if (strcmp("setProperty", type) == 0)
		doOk = doCameraSetProperty();

	// 设置 ROI 区域控制
	if (strcmp("setMask", type) == 0)
		doOk = doCameraSetMask(pDoc);

	return doOk;
}

bool CameraImpl::doCameraStart(Document* pDocument)
{
	CAM_START cameraStart;

	cameraStart.left = getIntValue("left", pDocument);
	cameraStart.top = getIntValue("top", pDocument);
	cameraStart.width = getIntValue("width", pDocument);
	cameraStart.height = getIntValue("height", pDocument);

	cameraStart.id = getIntValue("id", pDocument);
	cameraStart.angle = getIntValue("angle", pDocument);
	cameraStart.color = getIntValue("color", pDocument, 1);
	cameraStart.frameW = getIntValue("FrameW", pDocument);
	cameraStart.frameH = getIntValue("FrameH", pDocument);
	cameraStart.roiW = getIntValue("ROIW", pDocument);
	cameraStart.roiH = getIntValue("ROIH", pDocument);
	cameraStart.hwnd = (HWND)getIntValue("hwnd", pDocument);
	cameraStart.hide = getIntValue("hide", pDocument);

	return m_pCameraWnd->CameraStart(&cameraStart);
}

bool CameraImpl::doCameraStop()
{
	return m_pCameraWnd->CameraStop();
}

bool CameraImpl::doCameraCapture(Document * pDocument)
{
	CAM_CAPTURE cameraCapture;

	Value::MemberIterator itemObj = pDocument->FindMember("path");
	if (itemObj != pDocument->MemberEnd() && itemObj->value.IsString())
		cameraCapture.path = itemObj->value.GetString();

	cameraCapture.id = getIntValue("id", pDocument);
	cameraCapture.quality = getIntValue("quality", pDocument, 50);
	itemObj = pDocument->FindMember("text");
	if (itemObj != pDocument->MemberEnd() && itemObj->value.IsString())
		cameraCapture.text = itemObj->value.GetString();
	else
		cameraCapture.text = string("");

	cameraCapture.hPixel = getIntValue("hPixel", pDocument);
	cameraCapture.vPixel = getIntValue("vPixel", pDocument);
	cameraCapture.frameW = getIntValue("FrameW", pDocument);
	cameraCapture.frameH = getIntValue("FrameH", pDocument);

	return m_pCameraWnd->CameraCapture(&cameraCapture);
}

bool CameraImpl::doCameraPause()
{
	return m_pCameraWnd->CameraStop();
}

bool CameraImpl::doCameraResume()
{
	return m_pCameraWnd->CameraStop();
}

bool CameraImpl::doCameraSetProperty()
{
	return m_pCameraWnd->CameraProperty();
}

bool CameraImpl::doCameraSetMask(Document * pDocument)
{
	CAM_MASK cameraMask;

	cameraMask.enable = getIntValue("enable", pDocument);
	cameraMask.x = getIntValue("x", pDocument);
	cameraMask.y = getIntValue("y", pDocument);
	cameraMask.width = getIntValue("width", pDocument);
	cameraMask.height = getIntValue("height", pDocument);

	return m_pCameraWnd->CameraMask(&cameraMask);
}

int CameraImpl::stringToInt(const string &Text)
{
	stringstream ss(Text);
	int result;
	return (ss >> result) ? result : 0;
}

int CameraImpl::getIntValue(const char* name, Document* pDocument, const int defaultValue)
{
	int value = defaultValue;
	if (NULL == name || NULL == pDocument)
		return value;
	Value::MemberIterator itemObj = pDocument->FindMember(name);
	if (itemObj != pDocument->MemberEnd())
	{
		if (itemObj->value.IsString())
			value = stringToInt(itemObj->value.GetString());
		else if (itemObj->value.IsInt())
			value = itemObj->value.GetInt();
	}
	return value;
}
