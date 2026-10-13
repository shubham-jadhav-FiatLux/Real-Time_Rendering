

--- RENAMING Window.c TO OGL.cpp ---

	

-----------------------------------------------------

--- RENAMING "Window.h" TO "OGL.h" IN RESOURSE FILE

	#include "OGL.h"

-----------------------------------------------------

case ERASEBKGND:
	return 0;

Here onwards use return(0) instead of break.


-----------------------------------------------------

Here onwards because of inclusion of 'vmath.h' header file ,which is .cpp file, 
we give extension of our OGL source code file as .cpp instead of .c .