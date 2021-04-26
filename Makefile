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
        
###############################################################################
# Site configuration occurs beneath this comment
# Typically ./configure (autoconf tools) configures this section
# This section could be manually configured if autoconf/configure fails
###############################################################################

DISTNAME = rogue5.3-4.4.2020.1
PACKAGE_TARNAME = rogue-5.3-4.4.2020.1
PROGRAM = rogue

O=o

#CC=gcc
CC = gcc

#CFLAGS=-O2
CFLAGS = -g -O2

#LIBS=-lcurses
LIBS = 	-LF:\lib\SDL2-2.0.12\x86_64-w64-mingw32\lib \
		-LF:\lib\SDL2_ttf-2.0.15\x86_64-w64-mingw32\lib \
		-LF:\lib\SDL2_image-2.0.5\x86_64-w64-mingw32\lib \
		-lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image

#RM=rm -f
RM    = rm -f

#GROFF=groff
GROFF = groff

#NROFF=nroff
NROFF = nroff

#TBL=tbl
TBL   = tbl

#COLCRT=colcrt
COLCRT = colcrt

#SED=sed
SED   = sed

#SCOREFILE=rogue54.scr
SCOREFILE = rogue.scr

#LOCKFILE=rogue54.lck
LOCKFILE = rogue.lck

#GROUPOWNER=games
GROUPOWNER = 

#CPPFLAGS=-DHAVE_CONFIG_H
CPPFLAGS =-DHAVE_CONFIG_H 

#DISTFILE = $(PROGRAM)
DISTFILE = $(DISTNAME)-x86_64-unknown-linux-gnu

INSTALL=./install-sh

#INSTGROUP=-g games
INSTGROUP=
#INSTOWNER=-u root
INSTOWNER=

CHGRP=chgrp

MKDIR=mkdir

TOUCH=touch

RMDIR=rmdir

CHMOD=chmod

DESTDIR=

prefix=/usr/local
exec_prefix=${prefix}
datarootdir=${prefix}/share
datadir=${datarootdir}
bindir=${exec_prefix}/bin
mandir=${datarootdir}/man
docdir=${datarootdir}/doc/${PACKAGE_TARNAME}
man6dir = $(mandir)/man6

###############################################################################
# Site configuration occurs above this comment
# It should not be necessary to change anything below this comment
############################################################################### 

HDRS     = rogue.h extern.h score.h basics.h util.h
OBJS1    = vers.$(O) extern.$(O) armor.$(O) basics.$(O) chase.$(O) command.$(O) \
           daemon.$(O) daemons.$(O) fight.$(O) init.$(O) io.$(O) list.$(O) \
           mach_dep.$(O) main.$(O) mdport.$(O) misc.$(O) monsters.$(O) \
           move.$(O) new_level.$(O)
OBJS2    = options.$(O) pack.$(O) passages.$(O) potions.$(O) rings.$(O) \
           rip.$(O) rooms.$(O) save.$(O) scrolls.$(O) state.$(O) sticks.$(O) \
           things.$(O) weapons.$(O) wizard.$(O) xcrypt.$(O)
OBJS     = $(OBJS1) $(OBJS2) dmalloc.$(O)
CFILES   = vers.c extern.c armor.c basics.c chase.c command.c daemon.c \
           daemons.c fight.c init.c io.c list.c mach_dep.c \
           main.c  mdport.c misc.c monsters.c move.c new_level.c \
           options.c pack.c passages.c potions.c rings.c rip.c \
           rooms.c save.c scrolls.c state.c sticks.c things.c \
           weapons.c wizard.c xcrypt.c \
           dmalloc.c

.SUFFIXES: .obj

.c.obj:
	$(CC) $(CFLAGS) $(CPPFLAGS) /c $*.c
    
.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) \
	-IF:\lib\SDL2-2.0.12\x86_64-w64-mingw32\include\SDL2 \
	-IF:\lib\SDL2_ttf-2.0.15\x86_64-w64-mingw32\include\SDL2 \
	-IF:\lib\SDL2_image-2.0.5\x86_64-w64-mingw32\include\SDL2 \
	-c $*.c
    
$(PROGRAM): $(HDRS) $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) $(LIBS) -o $@
    
clean:
	$(RM) $(OBJS1)
	$(RM) $(OBJS2)
	$(RM) core a.exe a.out a.exe.stackdump $(PROGRAM) $(PROGRAM).exe
	$(RM) $(PROGRAM).tar $(PROGRAM).tar.gz $(PROGRAM).zip
	$(RM) *.log *.out
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
