include ../Makefile.inc

SCRIPT = MS3D_opts.mel

INSTALL_SCRIPT = $(INSTALL) -m 644

MAYA_SCRIPTS_PATH = $(MAYA_LOCATION)/scripts

all clean:

.PHONY : clean all install

install: $(SCRIPT)
	$(INSTALL_SCRIPT) $^ $(MAYA_SCRIPTS_PATH)/others
