cls

cl.exe /c /EHsc OGL.cpp /I "C:\glew\include"

rc.exe OGL.rc

link.exe OGL.obj OGL.res /LIBPATH:"C:\glew\lib\Release\X64" user32.lib gdi32.lib kernel32.lib glew32.lib opengl32.lib

OGL.exe
