// CWOpenCvCamera.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "CWOpenCvCamera.h"
#include "CameraImpl.h"

CameraImpl* pImpl = NULL;
void  InitCamera()
{
	if (NULL == pImpl)
		pImpl = new CameraImpl();
	pImpl->Init();
}

bool  DoProcess(char * cmd)
{
	bool optRet = false;
	if (NULL != pImpl)
		optRet = pImpl->DoProcess(cmd);
	return optRet;
}

void  UninitCamera()
{
	pImpl->Uninit();
	if (NULL != pImpl)
	{
		delete pImpl;
		pImpl = NULL;
	}
}
