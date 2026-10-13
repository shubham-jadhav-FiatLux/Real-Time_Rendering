
del D3D.exe
del D3D.obj
del D3D.res

cl.exe /c /EHsc D3D.cpp

rc.exe D3D.rc

link.exe D3D.obj D3D.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

D3D.exe
