###############################################################################
#
# Makefile for rogue
#
# Rogue: Exploring the Dungeons of Doom
# Copyright (C) 1980-1983, 1985, 1999 Michael Toy, Ken Arnold and Glenn Wichman
# All rights reserved.
#       
# See the file LICENSE.TXT for full copyright and licensing information.
#
###############################################################################

DISTNAME = rogue5.3-4.4.2020.1
PACKAGE_TARNAME = rogue-5.3-4.4.2020.1
PROGRAM = rogue

SRC=src

#CC=gcc
CC = gcc

#CFLAGS=-O2
CFLAGS = -g -O2

INCLUDES = 	-IF:\lib\SDL2-2.0.12\x86_64-w64-mingw32\include\SDL2 \
			-IF:\lib\SDL2_ttf-2.0.15\x86_64-w64-mingw32\include\SDL2 \
			-IF:\lib\SDL2_image-2.0.5\x86_64-w64-mingw32\include\SDL2 \

LIBS = 	-LF:\lib\SDL2-2.0.12\x86_64-w64-mingw32\lib \
		-LF:\lib\SDL2_ttf-2.0.15\x86_64-w64-mingw32\lib \
		-LF:\lib\SDL2_image-2.0.5\x86_64-w64-mingw32\lib \
		-lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image

#RM=rm -f
RM    = rm -f

#SED=sed
SED   = sed

#SCOREFILE=rogue54.scr
SCOREFILE = rogue.scr

#LOCKFILE=rogue54.lck
LOCKFILE = rogue.lck

#GROUPOWNER=games
# GROUPOWNER = 

#CPPFLAGS=-DHAVE_CONFIG_H
# CPPFLAGS =-DHAVE_CONFIG_H 

#DISTFILE = $(PROGRAM)
DISTFILE = $(DISTNAME)-x86_64-unknown-linux-gnu

INSTALL=./install-sh

MKDIR=mkdir

RMDIR=rmdir

DESTDIR=

CFILES 	= $(wildcard $(SRC)/*.c)
HDRS	= $(wildcard $(SRC)/*.h)
OBJS	= $(patsubst %.c,%.o,$(CFILES))

$(SRC)/%.o: $(SRC)/%.c
	@echo $*.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(INCLUDES) -c $(SRC)/$*.c -o $@

$(info $(HDRS) $(OBJS))

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

clean:
	$(RM) *.o $(SRC)/*.o
	$(RM) -rf $(DISTNAME)

dist.src:
	$(MAKE) $(MAKEFILE) clean
	mkdir $(DISTNAME)
	cp $(CFILES) $(HDRS) $(MISC) $(AFILES) $(DISTNAME)
	tar cf $(DISTNAME)-src.tar $(DISTNAME)
	gzip -f $(DISTNAME)-src.tar
	rm -fr $(DISTNAME)

dist: clean $(PROGRAM)
	tar cf $(DISTFILE).tar $(PROGRAM) LICENSE.TXT $(DOCS)
	gzip -f $(DISTFILE).tar

install: $(PROGRAM)
	-$(TOUCH) test
	-if test ! -f $(DESTDIR)$(SCOREFILE) ; then $(INSTALL) -m 0664 test $(DESTDIR)$(SCOREFILE) ; fi
	-$(INSTALL) -m 0755 $(PROGRAM) $(DESTDIR)$(bindir)/$(PROGRAM)
	-if test "x$(GROUPOWNER)" != "x" ; then \
	    $(CHGRP) $(GROUPOWNER) $(DESTDIR)$(SCOREFILE) ; \
	    $(CHGRP) $(GROUPOWNER) $(DESTDIR)$(bindir)/$(PROGRAM) ; \
	    $(CHMOD) 02755 $(DESTDIR)$(bindir)/$(PROGRAM) ; \
	    $(CHMOD) 0464 $(DESTDIR)$(SCOREFILE) ; \
         fi
	-if test -d $(man6dir) ; then $(INSTALL) -m 0644 rogue.6 $(DESTDIR)$(man6dir)/$(PROGRAM).6 ; fi
	-if test ! -d $(man6dir) ; then $(INSTALL) -m 0644 rogue.6 $(DESTDIR)$(mandir)/$(PROGRAM).6 ; fi
	-$(INSTALL) -m 0644 rogue.doc $(DESTDIR)$(docdir)/$(PROGRAM).doc
	-$(INSTALL) -m 0644 rogue.html $(DESTDIR)$(docdir)/$(PROGRAM).html
	-$(INSTALL) -m 0644 rogue.cat $(DESTDIR)$(docdir)/$(PROGRAM).cat
	-$(INSTALL) -m 0644 LICENSE.TXT $(DESTDIR)$(docdir)/LICENSE.TXT
	-$(INSTALL) -m 0644 rogue.me $(DESTDIR)$(docdir)/$(PROGRAM).me
	-if test ! -f $(DESTDIR)$(LOCKFILE)  ; then $(INSTALL) -m 0666 test $(DESTDIR)$(LOCKFILE)  ; $(RM) $(DESTDIR)$(LOCKFILE) ; fi
	-$(RM) test

uninstall:	
	-$(RM) $(DESTDIR)$(bindir)/$(PROGRAM)
	-$(RM) $(DESTDIR)$(man6dir)/$(PROGRAM).6
	-$(RM) $(DESTDIR)$(docdir)$(PROGRAM)/$(PROGRAM).doc
	-$(RM) $(DESTDIR)$(LOCKFILE)
	-$(RMDIR) $(DESTDIR)$(docdir)$(PROGRAM)
    
reinstall: uninstall install