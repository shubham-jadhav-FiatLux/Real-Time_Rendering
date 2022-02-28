
cl.exe /c /EHsc FileIO.c

link.exe FileIO.obj user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

FileIO.exe
