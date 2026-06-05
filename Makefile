CFLAGS = -Wall -O $(shell pkg-config --cflags gtk4)
CFLAGS += $(shell pkg-config --cflags glycin-2)
CFLAGS += $(shell pkg-config --cflags glycin-gtk4-2)
LDLIBS != pkg-config --libs gtk4
LDLIBS += $(shell pkg-config --libs glycin-2)
LDLIBS += $(shell pkg-config --libs glycin-gtk4-2)
LDLIBS += -lcrypto -lm
LDFLAGS=-Wl,--export-dynamic

all: gen viewer

viewer: main.o callbacks.o init.o show.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

main.o: main.c viewer.h

init.o: init.c gtkbuilder.h viewer.h

callbacks.o: callbacks.c viewer.h

show.o: show.c viewer.h

gen: gen.c
	$(CC) -o gen gen.c

gtkbuilder.h: gen Viewer-gtk4.ui
	./gen < Viewer-gtk4.ui > gtkbuilder.h

clean:
	$(RM) gen gtkbuilder.h *.o viewer
