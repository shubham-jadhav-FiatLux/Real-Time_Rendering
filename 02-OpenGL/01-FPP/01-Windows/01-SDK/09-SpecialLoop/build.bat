
cl.exe /c /EHsc GameLoop.c

link.exe GameLoop.obj user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

GameLoop.exe
