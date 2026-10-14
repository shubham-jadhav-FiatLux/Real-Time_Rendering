
cl.exe /c /EHsc 05-HelloWorld_win32.cpp

link.exe kernel32.lib gdi32.lib user32.lib 05-HelloWorld_win32.obj

05-HelloWorld_win32.exe
