
cl.exe /c /EHsc ActiveWindow.c

link.exe ActiveWindow.obj user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

ActiveWindow.exe
