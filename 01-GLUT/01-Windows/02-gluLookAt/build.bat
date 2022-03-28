
cls

cl.exe /c /EHsc /I c:/freeglut/include OGL.cpp

link.exe OGL.obj /LIBPATH:C:\freeglut\lib freeglut.lib /SUBSYSTEM:CONSOLE

OGL.exe
