
cl.exe /c /EHsc /I C:\freeglut\include Clock_ByGokhaleMam.cpp

link.exe Clock_ByGokhaleMam.obj /LIBPATH:C:\freeglut\lib freeglut.lib /SUBSYSTEM:CONSOLE

Clock_ByGokhaleMam.exe