
cl.exe /c /EHsc HelloOpenCL.c /I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.6\include"

link.exe HelloOpenCL.obj /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.6\lib\x64" OpenCL.lib

HelloOpenCL.exe
