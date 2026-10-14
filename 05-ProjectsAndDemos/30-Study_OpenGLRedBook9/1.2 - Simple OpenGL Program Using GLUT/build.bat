
cls

cl.exe /c /EHsc /I c:/freeglut/include hello.c

link.exe hello.obj /LIBPATH:C:\freeglut\lib freeglut.lib /SUBSYSTEM:CONSOLE

hello.exe
