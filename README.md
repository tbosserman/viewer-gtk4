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

### KNOWN BUGS
1. Not all the accelerators work. You can press 'N', 'P', and 'Q', but the
arrows don't work. Haven't figured out why yet.
