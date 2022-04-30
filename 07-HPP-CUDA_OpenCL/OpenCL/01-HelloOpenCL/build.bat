
cl.exe /c /EHsc HelloCUDA.c /I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.6\include"

link.exe HelloCUDA.obj /libpath "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.6\lib\x64\OpenCL.lib" /SUBSYSTEM:WINDOWS