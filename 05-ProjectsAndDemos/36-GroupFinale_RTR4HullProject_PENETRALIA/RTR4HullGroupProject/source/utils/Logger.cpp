// Windows header files - start
#include<windows.h>
// Windows header files - end

// C header files - start
#include <cstdarg>
// C header files - end

// User defined header files - start
#include "utils/Logger.h"
// User defined header files - end

FILE* gpFile = NULL;

bool InitializeLogger(void)
{
	// Code
	if (fopen_s(&gpFile, LOG_FILE_NAME, "w") != 0)
	{
		return(false);
	}

	return(true);
}

void UninitializeLogger(void)
{
	// Code
	if (gpFile)
	{
		fclose(gpFile);
		gpFile = NULL;
	}
}

void errorLog(const char* fileName, int lineNo, FILE* file, const char* format, ...)
{
	// Local function declarations
	void logLine(const char*, int, FILE*, LogLevel, const char*, va_list);

	// Variable declarations
	va_list vargs;

	// Code
	// Get variable arguments
	va_start(vargs, format);

	// Write log line
	logLine(fileName, lineNo, file, Error, format, vargs);

	// End variable arguments
	va_end(vargs);
}

void warningLog(const char* fileName, int lineNo, FILE* file, const char* format, ...)
{
	// Local function declarations
	void logLine(const char*, int, FILE*, LogLevel, const char*, va_list);

	// Variable declarations
	va_list vargs;

	// Code
	// Get variable arguments
	va_start(vargs, format);

	// Write log line
	logLine(fileName, lineNo, file, Warn, format, vargs);

	// End variable arguments
	va_end(vargs);
}

void infoLog(const char* fileName, int lineNo, FILE* file, const char* format, ...)
{
	// Local function declarations
	void logLine(const char*, int, FILE*, LogLevel, const char*, va_list);

	// Variable declarations
	va_list vargs;

	// Code
	// Get variable arguments
	va_start(vargs, format);

	// Write log line
	logLine(fileName, lineNo, file, Info, format, vargs);

	// End variable arguments
	va_end(vargs);
}

void debugLog(const char* fileName, int lineNo, FILE* file, const char* format, ...)
{
	// Local function declarations
	void logLine(const char*, int, FILE*, LogLevel, const char*, va_list);

	// Variable declarations
	va_list vargs;

	// Code
	// Get variable arguments
	va_start(vargs, format);

	// Write log line
	logLine(fileName, lineNo, file, Debug, format, vargs);

	// End variable arguments
	va_end(vargs);
}

void traceLog(const char* fileName, int lineNo, FILE* file, const char* format, ...)
{
	// Local function declarations
	void logLine(const char*, int, FILE*, LogLevel, const char*, va_list);

	// Variable declarations
	va_list vargs;

	// Code
	// Get variable arguments
	va_start(vargs, format);

	// Write log line
	logLine(fileName, lineNo, file, Trace, format, vargs);

	// End variable arguments
	va_end(vargs);
}

/// <summary>
/// This function writes log to the file if the global log level that is set is equal or above the log level which is passed. The log is written to the file whose handle is passed else to the file whose handle is present in global file handle.
/// </summary>
/// <param name="file">Handle of file to which log is to be written.</param>
/// <param name="ll">Level of the log to be written.</param>
/// <param name="format">Format of the string to be written.</param>
/// <param name="vargs">List of variable arguments to be used with the format (3st argument) while writing the log.</param>
void logLine(const char* fileName, int line, FILE* file, LogLevel ll, const char* format, va_list vargs)
{
	// Local function declarations
	BOOL shouldWriteLog(LogLevel);

	// Variable declarations
	FILE* fileToWrite = NULL;

	// Code
	if (shouldWriteLog(ll))
	{
		fileToWrite = file ? file : gpFile;

		switch (ll)
		{
		case Error:
			fprintf(fileToWrite, "|Error\t| ");
			break;

		case Warn:
			fprintf(fileToWrite, "|Warn\t| ");
			break;

		case Info:
			fprintf(fileToWrite, "|Info\t| ");
			break;

		case Debug:
			fprintf(fileToWrite, "|Debug\t| ");
			break;

		case Trace:
			fprintf(fileToWrite, "|Trace\t| ");
			break;

		default:
			fprintf(fileToWrite, "|Unknown\t| ");
			break;
		}

		fprintf(fileToWrite, "%s (%d)\t| ", fileName, line);
		vfprintf(fileToWrite, format, vargs);
		fputc('\n', fileToWrite);
		fflush(fileToWrite);

		fileToWrite = NULL;
	}
}

/// <summary>
/// This function checks if the log can be written or not based on the passed logging level.
/// </summary>
/// <param name="ll">Level of the log to be written.</param>
BOOL shouldWriteLog(LogLevel ll)
{
	return(ll >= MIN_LOG_LEVEL && ll <= MAX_LOG_LEVEL && ll <= gLogLevel);
}
