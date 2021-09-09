
TARGET = blocksThatFall
OBJS = main.o particle.o inflate.o save.o io.o gfx.o game.o common/mp3player.o sound.o random.o common/callbacks.o common/vram.o texture.o sfx_flip.o sfx_hit.o sfx_remove_row.o

INCDIR =
CFLAGS = -G0 -O0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS =
LIBS= -lpspgum -lpspgu -lm -lmad -lpspaudiolib -lpspaudio -lpsppower -lpsprtc

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Blocks that Fall v1.5
PSP_EBOOT_ICON = icon.png
PSP_EBOOT_PIC1 = background.png

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

texture.o: texture.dtt
	bin2o -i texture.dtt texture.o texture

sfx_flip.o: sfx_flip.raw
	bin2o -i sfx_flip.raw sfx_flip.o sfx_flip

sfx_hit.o: sfx_hit.raw
	bin2o -i sfx_hit.raw sfx_hit.o sfx_hit

sfx_remove_row.o: sfx_remove_row.raw
	bin2o -i sfx_remove_row.raw sfx_remove_row.o sfx_remove_row

