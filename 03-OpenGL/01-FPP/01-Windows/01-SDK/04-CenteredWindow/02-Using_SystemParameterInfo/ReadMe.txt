
SystemParameterInfo()

- Retrieves or sets the value of one of the system-wide parameters. 
- This function can also update the user profile while setting a parameter.

-----------------------------------------------------------------------------

SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);

SPI_GETWORKAREA

- Retrieves the size of the work area on the primary display monitor. 
  The work area is the portion of the screen not obscured by the system
  taskbar or by application desktop toolbars. 
  The pvParam parameter must point to a RECT structure that receives 
  the coordinates of the work area, expressed in physical pixel size.
  Any DPI virtualization mode of the caller has no effect on this output.

--------------------------------------------------------------------------------
