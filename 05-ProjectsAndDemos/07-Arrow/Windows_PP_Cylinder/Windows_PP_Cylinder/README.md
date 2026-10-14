How to use OGLCylinder.dll?

01. Include "OGLCylinder.h" file in your application.
02. Keep the "OGLCylinder.dll" in system file path or the current directory of your application.
03. Link "OGLCylinder.lib" in your application using your preferred but suitable way of linking.
04. Create pointers for storing addresses of arrays of vertices, normals, tex coords and elements of cylinder.
05. While initializing your application, get the count of cylinder vertices and elements from the functions provided by OGLCylinder.dll. You need to specify the number of slices and stacks you want for your cylinder.
06. Allocate the memory to the pointers created in Step 04.
07. Decide the base radius and top radius of your cylinder. Try to use values between 0 - 1 to keep the co-ordinates of the vertices between -1 to 1. You can scale the cylinder later to whatever size you want.
08. Generate cylinder data by calling getCylinderVerticesData() function offered by the OGLCylinder.dll. Pass the required parameters by referring to the prototypes given in OGLCylinder.h file.
09. Pass the required data out of generated data to the shader using array buffers and element array buffer.
10. Draw the cylinder using your preferred method of drawing (using vertices, elements, etc.).
11. Finally unallocate the memory allocated to the pointers that are created in Step 04 while uninitializing your application.

Sample application details

01. Open x64 Native Tools Command Prompt for VS.
02. Go to the path where you kept the sample application.
03. Run "build.bat" provided in that folder.
04. It should generate the "OGL.exe" file.
05. Execute the "OGL.exe" file by double-clicking or simply typing its name in the command prompt.
06. There are few options provided to view the cylinder:
    a. Press L/l to toggle light.
    b. Press T/t to toggle texture.
    c. Press 1-6 numeric keys to change the camera position to see the cylinder from different angles.
    d. Hold down x, y or z keys to rotate the cylinder in clockwise direction about the respective axis.
    e. Hold down X, Y or Z keys to rotate the cylinder in anti-clockwise direction about the respective axis.
07. You can also change the values of base radius, top radius, slices, stacks, etc. to understand the working of OGLCylinder.dll better. Change the values you want, rebuild, execute and use the provided options to understand the usage.
