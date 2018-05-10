#pragma once

#ifdef __cplusplus
extern "C" {
#endif

	// The following ifdef block is the standard way of creating macros which make exporting 
	// from a DLL simpler. All files within this DLL are compiled with the NATIVEDLL_EXPORTS
	// symbol defined on the command line. this symbol should not be defined on any project
	// that uses this DLL. This way any other project whose source files include this file see 
	// WINDUP_API functions as being imported from a DLL, wheras this DLL sees symbols
	// defined with this macro as being exported.

#ifdef CWOPENCVCAMERA_EXPORTS
#define CWOPENCVCAMERA_API __declspec(dllexport)
#else
#define CWOPENCVCAMERA_API __declspec(dllimport)
#endif
	void CWOPENCVCAMERA_API InitCamera();
	bool CWOPENCVCAMERA_API DoProcess(char *cmd);
	void CWOPENCVCAMERA_API UninitCamera();


#ifdef __cplusplus 
}
#endif