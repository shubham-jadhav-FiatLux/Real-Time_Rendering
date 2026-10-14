
cls

cl.exe /c /EHsc SolarSystem.cpp

rc.exe OGL.rc

link.exe SolarSystem.obj OGL.res user32.lib gdi32.lib kernel32.lib /SUBSYSTEM:WINDOWS

SolarSystem.exe
