
cl.exe /c /EHsc VecAdd.cpp /I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.6\include"

link.exe VecAdd.obj /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.6\lib\x64" OpenCL.lib

VecAdd.exe
