
** Depth Related Changes **
-----------------------------
Globally
1 - Declare DSV Interface 
    No Changes in initialize

-----------------------------
Resize()
2 - Depth buffer related steps

    a - Initialize texture 2D descripter
    a - Create 2D texture by using above local depth buffer
3 - 
    a - Uninitialize old DSV
    b - Initialize depth stensile view descripter
    c - Now create depth stencil view using above depth buffer and descripter

4 - Uninitialize local depth buffer

5 - Use this DSV as third parameter

6 - Clear the depth stencil view

7 - Release the DSV