// stdafx.cpp : source file that includes just the standard includes
// CWOpenCvCamera.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#pragma	comment(lib ,"vfw32.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\ippicvmt.lib")

#ifdef _DEBUG
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\IlmImfd.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\ippiwd.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\ittnotifyd.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\libjasperd.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\libjpegd.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\libpngd.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\libprotobufd.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\libtiffd.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\libprotobufd.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\libwebpd.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\zlibd.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\opencv_world341d.lib")
#else
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\IlmImf.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\ippiw.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\ittnotify.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\libjasper.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\libjpeg.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\libpng.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\libprotobuf.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\libtiff.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\libprotobuf.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\libwebp.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\zlib.lib")
#pragma	comment(lib ,"D:\\opencv\\static_build\\install\\x86\\vc15\\staticlib\\opencv_world341.lib")
#endif // DEBUG