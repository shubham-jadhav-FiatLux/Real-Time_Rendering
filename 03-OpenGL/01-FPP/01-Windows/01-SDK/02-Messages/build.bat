
cl.exe /c /EHsc Messages.c

link.exe Messages.obj user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Messages.exe
