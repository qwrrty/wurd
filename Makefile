CFLAGS	= -g -Wall
UISRC	= ui-curses.c
UIOBJ	= ui-curses.o
UIHDR	= ui-curses.h

LIBS	= -lncurses

all:	wurd

wurd:	dict.o main.o $(UIOBJ)
	$(CC) -o wurd $^ $(LIBS)

main.o:		main.c dict.h $(UIHDR)
dict.o:		dict.c dict.h
ui-curses.o:	ui-curses.c ui-curses.h

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	-rm wurd *.o
