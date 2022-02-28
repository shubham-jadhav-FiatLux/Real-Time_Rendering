
cl.exe /c /EHsc Win32_Assignment01_BySir.c

rc.exe Window.rc

link.exe Win32_Assignment01_BySir.obj Window.res user32.lib gdi32.lib /SUBSYSTEM:WINDOWS

Win32_Assignment01_BySir.exe
