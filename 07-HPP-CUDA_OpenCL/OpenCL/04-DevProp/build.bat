
cl.exe /c /EHsc DevProp.c /I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.6\include"

link.exe DevProp.obj /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.6\lib\x64" OpenCL.lib

DevProp.exe
