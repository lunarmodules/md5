# $Id: Makefile,v 1.5 2007/07/23 19:41:48 mascarenhas Exp $

T= md5
V= 1.0.2
CONFIG= ./config

include $(CONFIG)

ifeq "$(LUA_VERSION_NUM)" "500"
COMPAT_O= $(COMPAT_DIR)/compat-5.1.o
endif

OBJS= src/md5.o src/md5lib.o $(COMPAT_O)
LUAS= src/md5.lua

DES56_OBJS= src/des56.o src/ldes56.o
DES56_LIBNAME= des56.so

all: src/$(LIBNAME) src/$(DES56_LIBNAME)

src/$(LIBNAME) : $(OBJS)
	export MACOSX_DEPLOYMENT_TARGET="10.3"; $(CC) $(CFLAGS) $(LIB_OPTION) -o src/$(LIBNAME) $(OBJS)

src/$(DES56_LIBNAME) : $(DES56_OBJS)
	export MACOSX_DEPLOYMENT_TARGET="10.3"; $(CC) $(CFLAGS) $(LIB_OPTION) -o src/$(DES56_LIBNAME) $(DES56_OBJS)

$(COMPAT_DIR)/compat-5.1.o: $(COMPAT_DIR)/compat-5.1.c
	$(CC) -c $(CFLAGS) -o $@ $(COMPAT_DIR)/compat-5.1.c

install: src/$(LIBNAME) src/$(DES56_LIBNAME)
	mkdir -p $(LUA_LIBDIR)/md5
	cp src/$(LIBNAME) $(LUA_LIBDIR)/md5/core.so
	mkdir -p $(LUA_DIR)
	cp $(LUAS) $(LUA_DIR)
	cp src/$(DES56_LIBNAME) $(LUA_LIBDIR)

clean:
	rm -f $(OBJS) src/$(LIBNAME) $(DES56_OBJS) $(DES56_LIBNAME)
