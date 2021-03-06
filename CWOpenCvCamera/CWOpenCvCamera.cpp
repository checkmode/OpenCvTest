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
	if (NULL != pImpl)
	{
        pImpl->Uninit();
        // 不要删除该指针
		//delete pImpl;
		//pImpl = NULL;
	}
}
