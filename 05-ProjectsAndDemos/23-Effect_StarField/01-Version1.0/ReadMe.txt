steps

1. copy sb7ktx.cpp and sb7ktx.h from super bible github code folder
2. compile sb7ktx.cpp and link sb7ktx.obj to your exe application

command:

Sample Format :

cl.exe /c /EHsc sb7ktx.cpp /I "C:\glew\include"

cl.exe /c /EHsc Starfield_KTX_fileformat.cpp /I "C:\glew\include"

rc.exe Starfield_KTX_fileformat.rc

link.exe Starfield_KTX_fileformat.obj sb7ktx.obj Starfield_KTX_fileformat.res /LIBPATH:"C:\glew\lib\Release\x64" user32.lib gdi32.lib kernel32.lib glew32.lib

Starfield_KTX_fileformat.exe

del Starfield_KTX_fileformat.exe Starfield_KTX_fileformat.res Starfield_KTX_fileformat.obj