include Makefile.inc

SHELL = /bin/sh

SUBDIRS = src scripts

all clean install: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: all clean install $(SUBDIRS)
