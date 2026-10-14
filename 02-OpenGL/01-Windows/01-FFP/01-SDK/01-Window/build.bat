
cl.exe /c /EHsc Window.c

link.exe Window.obj user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Window.exe
