Extra Credit Stuff:
-Mouse scroll wheel zooms in and out (FOV change and projection update)
-Hold B to barrel roll (note that rotating camera after rolling will snap the up vector to the y axis, rather than keeping the current up. This can be changed by uncommenting and commenting the last 2 lines of the rotation function respectively, but this will cause the very first right-click after startup to wobble the camera)

Notes:
- Camera tutorial referenced in this assignment: https://learnopengl.com/Getting-started/Camera
- My code is heavily based off this tutorial as well as the starter code, so my solution will likely look a bit different in it's methods (no use of target), but functionality should be the same