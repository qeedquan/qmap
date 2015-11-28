CFLAGS = /zq /d1 /w3 /oetax

OBJS = main.obj poly.obj render.obj 3d.obj mode.obj bsp.obj \
       tmap3d.obj clippoly.obj tm_c.obj surface.obj readbsp.obj

qmap.exe: $(OBJS)
	wlink @qmap.lnk

qmsrc.zip: $(OBJS)
	pkzip qmsrc.zip *.c *.h *.txt makefile qmap.lnk

qmexe.zip: qmap.exe
        pkzip qmexe qmap.exe readme.txt qmap.txt bug.txt redist.txt

.c.obj:
	wcl386 $(CFLAGS) -c $<

main.obj: main.c
clippoly.obj: clippoly.c
render.obj: render.c
3d.obj: 3d.c
mode.obj: mode.c
bsp.obj: bsp.c
readbsp.obj: readbsp.c
tmap3d.obj: tmap3d.c
poly.obj: poly.c
tm_c.obj: tm_c.c
surface.obj: surface.c
