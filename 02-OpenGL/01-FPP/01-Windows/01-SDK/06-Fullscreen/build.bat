
cl.exe /c /EHsc Fullscreen.c

link.exe Fullscreen.obj user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Fullscreen.exe
