
cls

cl.exe /c /EHsc /I c:/freeglut/include double.c

link.exe double.obj /LIBPATH:C:\freeglut\lib freeglut.lib /SUBSYSTEM:CONSOLE

double.exe
