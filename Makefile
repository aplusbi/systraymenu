include config.mk

all: systray

systray: systray.c Makefile config.mk
	${CC} -Wno-deprecated `pkg-config --cflags gtk+-3.0` -o systray systray.c `pkg-config --libs gtk+-3.0`

clean:
	rm systray
