# $Id: Makefile,v 1.2 2006/03/03 15:04:49 tomas Exp $

T= md5
V= 1.0
CONFIG= ./config

include $(CONFIG)

COMPAT_O= $(COMPAT_DIR)/compat-5.1.o
OBJS= src/md5.o src/md5lib.o $(COMPAT_O)
LUAS= src/md5.lua


src/$(LIBNAME) : $(OBJS)
	export MACOSX_DEPLOYMENT_TARGET="10.3"; $(CC) $(CFLAGS) $(LIB_OPTION) -o src/$(LIBNAME) $(OBJS)

$(COMPAT_DIR)/compat-5.1.o: $(COMPAT_DIR)/compat-5.1.c
	$(CC) -c $(CFLAGS) -o $@ $(COMPAT_DIR)/compat-5.1.c

install: src/$(LIBNAME)
	mkdir -p $(LUA_LIBDIR)/md5
	cp src/$(LIBNAME) $(LUA_LIBDIR)/md5/core.so
	mkdir -p $(LUA_DIR)
	cp $(LUAS) $(LUA_DIR)

clean:
	rm -f $(OBJS) src/$(LIBNAME)
