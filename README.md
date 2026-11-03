Viewer-gtk4: a simple GTK-4 image viewer.

Nothing too interesting here. I used this as an excuse to start learning
Cambalache and GTK-4. Someone might find this useful for some of the
examples it provides.

I developed this on OpenSUSE Tumbleweed. I'm new to Tumbleweed, and I'm using
it because I wanted to use the very latest packages and kernels.

### DEVELOPMENT ENVIRONMENT

This list is off the top of my head. I'm probably forgetting some.

1. Cambalache
The easiest way is to just use flatpak. I personally am not all that fond
of flatpak, but sometimes it's just WAY easier. This is one of those times.  
flatpak install ar.xjuan.Cambalache
2. gtk4-devel
3. libglycin-devel
4. libglycin-gtk4-devel

### RANDOM NOTES
1. The libglycin-2 shipped with OpenSUSE Tumbleweed doesn't include support
for HEIC images. Really, the problem is the the libheic delivered with
tumbleweed. Anyway, I built both glycin and libheif from source. It was kinda
painful. I need to document how I did it.
2. As of "Version 2.0" pinch-to-zoom works! I also completely rewrite the
event handler(s) so all the accelerator keys work:
    a. 1 - Scale the image 1:1
    b. f - Fit the image to the window
    c. n or right arrow - next image
    d. p or left arrow - previous image
    e. q - quit
    f. \+ - scale up
    g. \- - scale down
