
cl.exe /c /EHsc CenteredWindow.c

link.exe CenteredWindow.obj user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

CenteredWindow.exe
