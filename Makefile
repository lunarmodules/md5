# $Id: Makefile,v 1.1 2006/02/02 21:11:12 tomas Exp $

T= md5
V= 1.0
CONFIG= ./config

include $(CONFIG)

COMPAT_O= $(COMPAT_DIR)/compat-5.1.o
OBJS= src/md5.o src/md5lib.o $(COMPAT_O)


src/$(LIBNAME) : $(OBJS)
	export MACOSX_DEPLOYMENT_TARGET="10.3"; $(CC) $(CFLAGS) $(LIB_OPTION) -o src/$(LIBNAME) $(OBJS)

$(COMPAT_DIR)/compat-5.1.o: $(COMPAT_DIR)/compat-5.1.c
	$(CC) -c $(CFLAGS) -o $@ $(COMPAT_DIR)/compat-5.1.c

install: src/$(LIBNAME)
	mkdir -p $(LUA_LIBDIR)
	cp $(LIBNAME) $(LUA_LIBDIR)
	cd $(LUA_LIBDIR); ln -f -s $(LIBNAME) $T.so

clean:
	rm -f $(OBJS) src/$(LIBNAME)
