#pragma once
#include "rapidjson/document.h"     // rapidjson's DOM-style API
using namespace rapidjson;


class CameraWnd;
class CameraImpl
{
public:
	CameraImpl();
	virtual ~CameraImpl();

public:
	void Init();
	bool DoProcess(char *cmd);
	void Uninit();

private:
	bool doProcess(const char* type, Document* document);
	bool doCameraStart(Document* document);
	bool doCameraStop();
	bool doCameraCapture(Document* document);
	bool doCameraPause();
	bool doCameraResume();
	bool doCameraSetProperty();
	bool doCameraSetMask(Document* document);

	int stringToInt(const std::string &Text);
	int getIntValue(const char* name, Document* pDocument, const int defaultValue = 0);

private:
	CameraWnd * m_pCameraWnd;
	std::shared_ptr<spdlog::logger> _pLog;

};

