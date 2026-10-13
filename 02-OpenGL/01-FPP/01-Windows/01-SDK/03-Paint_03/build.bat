
cl.exe /c /EHsc Paint.c

link.exe Paint.obj user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Paint.exe
