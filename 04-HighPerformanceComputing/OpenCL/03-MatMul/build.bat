
cl.exe /c /EHsc MatMul.cpp /I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.6\include"

link.exe MatMul.obj /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.6\lib\x64" OpenCL.lib

MatMul.exe
