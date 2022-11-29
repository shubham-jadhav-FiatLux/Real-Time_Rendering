
cls

del OGL.exe

del OGL.obj

del OGL.res

rc.exe OGL.rc

nvcc.exe -I -gencode=arch=compute_37,code=\"sm_37,compute_37\" -gencode=arch=compute_50,code=\"sm_50,compute_50\" -I "C:\glew\include" -I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.6\include" -L "C:\glew\lib\Release\x64" -L "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.6\lib\x64" user32.lib gdi32.lib -o OGL.exe OGL.res OGL.cu

OGL.exe
