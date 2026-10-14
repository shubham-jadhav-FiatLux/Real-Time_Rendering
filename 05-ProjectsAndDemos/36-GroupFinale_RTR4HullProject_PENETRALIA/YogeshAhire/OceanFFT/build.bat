cls
del OGL.exe
del OGL.obj
del OGL.res

rc.exe OGL.rc

nvcc -I C:\glew\include -I "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\bin\Hostx64" -L "C:\glew\lib\Release\x64" -L "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\bin\Hostx64" user32.lib gdi32.lib -o OGL.exe OGL.res OGL.cu


OGL.exe


