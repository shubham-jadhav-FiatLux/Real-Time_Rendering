#ifndef __LOGGER_H__
#define __LOGGER_H__
#include<stdio.h>

#define MIN_LOG_LEVEL Error
#define MAX_LOG_LEVEL Trace

#define ErrorLog(fileHandle, formattedString, ...) \
			errorLog( __FILE__, __LINE__, fileHandle, formattedString, __VA_ARGS__)
#define WarningLog(fileHandle, formattedString, ...) \
			warningLog( __FILE__, __LINE__, fileHandle, formattedString, __VA_ARGS__)
#define InfoLog(fileHandle, formattedString, ...) \
			infoLog( __FILE__, __LINE__, fileHandle, formattedString, __VA_ARGS__)
#define DebugLog(fileHandle, formattedString, ...) \
			debugLog( __FILE__, __LINE__, fileHandle, formattedString, __VA_ARGS__)
#define TraceLog(fileHandle, formattedString, ...) \
			traceLog( __FILE__, __LINE__, fileHandle, formattedString, __VA_ARGS__)

#define LOG_FILE_NAME "Log.txt"

enum LogLevel
{
	Error = 0,
	Warn,
	Info,
	Debug,
	Trace
};

// TODO: Initialize global log level. Find a place to do this.
const LogLevel gLogLevel = Debug;

/// <summary>
/// This function initializes the logger.
/// </summary>
/// <param name=""></param>
/// <returns>true in case logger initialization succeeds, false otherwise</returns>
bool InitializeLogger(void);

/// <summary>
/// This function uninitializes the logger.
/// </summary>
/// <param name=""></param>
void UninitializeLogger(void);

/// <summary>
/// This function writes log to the file if the global log level is set to 'Error' or above. The log is written to the file whose handle is passed else to the file whose handle is present in global file handle.
/// </summary>
/// <param name="format">Format of the string to be written.</param>
/// <param name="file">Handle of file to which log is to be written.</param>
/// <param name=""></param>
void errorLog(const char* fileName, int lineNo, FILE* file, const char* format, ...);

/// <summary>
/// This function writes log to the file if the global log level is set to 'Warn' or above. The log is written to the file whose handle is passed else to the file whose handle is present in global file handle.
/// </summary>
/// <param name="file">Handle of file to which log is to be written.</param>
/// <param name="format">Format of the string to be written.</param>
/// <param name=""></param>
void warningLog(const char* fileName, int lineNo, FILE* file, const char* format, ...);

/// <summary>
/// This function writes log to the file if the global log level is set to 'Info' or above. The log is written to the file whose handle is passed else to the file whose handle is present in global file handle.
/// </summary>
/// <param name="file">Handle of file to which log is to be written.</param>
/// <param name="format">Format of the string to be written.</param>
/// <param name=""></param>
void infoLog(const char* fileName, int lineNo, FILE* file, const char* format, ...);

/// <summary>
/// This function writes log to the file if the global log level is set to 'Debug' or above. The log is written to the file whose handle is passed else to the file whose handle is present in global file handle.
/// </summary>
/// <param name="file">Handle of file to which log is to be written.</param>
/// <param name="format">Format of the string to be written.</param>
/// <param name=""></param>
void debugLog(const char* fileName, int lineNo, FILE* file, const char* format, ...);

/// <summary>
/// This function writes log to the file if the global log level is set to 'Trace' or above. The log is written to the file whose handle is passed else to the file whose handle is present in global file handle.
/// </summary>
/// <param name="file">Handle of file to which log is to be written.</param>
/// <param name="format">Format of the string to be written.</param>
/// <param name=""></param>
void traceLog(const char* fileName, int lineNo, FILE* file, const char* format, ...);

#endif // !__LOGGER_H__
