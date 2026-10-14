#include "Common.h"

const char gszLogFileName[] = "Log.txt";
HWND ghwnd = NULL;
unsigned int bLight = 0;
int currentScene = SCENE_1_OPENING_SCENE;

//void FileLog(const char* fileName, int lineNo, const char* formattedString, ...)
//{
//	FILE* gpFile = NULL;
//	va_list args;
//
//	fopen_s(&gpFile, gszLogFileName, "a+");
//
//	if (gpFile)
//	{
//		va_start(args, formattedString);
//		fprintf(gpFile, "[%s (%d)] : ", fileName, lineNo);
//		vfprintf(gpFile, formattedString, args);
//		va_end(args);
//		fclose(gpFile);
//		gpFile = NULL;
//	}
//}
