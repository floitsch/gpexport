include ../Makefile.inc

PLUGIN = MS3DTranslator.so
EXECUTABLE = maya2ms3d

INSTALL_PROGRAM = $(INSTALL)
INSTALL_PLUGIN = $(INSTALL) -m 644

CC_FLAGS = -Wall -Wno-deprecated -D_BOOL -DLINUX
C++_FLAGS = $(CC_FLAGS)

BIN_PATH = $(PREFIX)/bin

MAYA_LIB_PATH = $(MAYA_LOCATION)/lib
MAYA_BIN_PATH = $(MAYA_LOCATION)/bin
MAYA_PLUGIN_PATH = $(MAYA_BIN_PATH)/plug-ins

INCLUDES = -I$(MAYA_LOCATION)/include
MAYA_LIBS = -L$(MAYA_LIB_PATH) \
-lAnimEngine -lCommandEngine -lDependEngine \
-lGeometryEngine -lNurbsEngine -lImf -lImage -lDependCommand \
-lExtensionLayer -lDataModel -lPolyEngine -lSubdivEngine \
-lSubdivGeom -l3dGraphics -lNurbs -lRenderModel -lPoly \
-lShared -lModelSlice -lAnimSlice -lPolySlice -lSubdiv \
-lKinSlice -lNurbsSlice -lDeformSlice -lStringCatalog \
 -lOpenMaya -lOpenMayaAnim -lFoundation
MAYA_LIBS_EXE = $(MAYA_LIBS) -lOpenMayalib -lTranslators

SOURCES =  animationExtraction.cpp bindPoseTool.cpp conversion.cpp exporterConfig.cpp \
fileWriter.cpp materialExtraction.cpp\
meshExtraction.cpp optimization.cpp sceneExtraction.cpp\
gpExporter.cpp skeletonExtraction.cpp \
ms3dExporter.cpp ms3dWriter.cpp

PLUGIN_SOURCES = plugin.cpp ms3dPlugin.cpp
EXEC_SOURCES = executable.cpp ms3dExecutable.cpp

ALL_SOURCES = $(SOURCES) $(PLUGIN_SOURCES) $(EXEC_SOURCES)

OBJS = $(SOURCES:.cpp=.o)
PLUGIN_OBJS = $(PLUGIN_SOURCES:.cpp=.o)
EXEC_OBJS = $(EXEC_SOURCES:.cpp=.o)

%.o: %.cpp
	$(C++) $(C++_FLAGS) $(INCLUDES) -c $< -o $@

# note, that we don't put the $(INCLUDES) to limit the dependencies.
%.d: %.cpp
	$(C++) -MM $(C++_FLAGS)  $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

all: $(PLUGIN) $(EXECUTABLE)

ifneq ($(MAKECMDGOALS),clean)
-include $(ALL_SOURCES:.cpp=.d)
endif

$(PLUGIN): $(OBJS) $(PLUGIN_OBJS)
	$(LD) -shared $(INCLUDES) $^ -o $@ $(MAYA_LIBS)

# the -u __ctype.... are symbols, that are now hidden in glibc3.4.
# If you are using glibc3.3, you can savely remove them.
$(EXECUTABLE): $(OBJS) $(EXEC_OBJS)
	$(LD) $(INCLUDES) $^ -o $@ $(MAYA_LIBS_EXE) -u __ctype_b -u __ctype_toupper -u __ctype_tolower


.PHONY : clean all install

install: $(PLUGIN) $(EXECUTABLE)
	$(INSTALL_PLUGIN) $(PLUGIN) $(MAYA_PLUGIN_PATH)
	$(INSTALL_PROGRAM) $(EXECUTABLE) $(BIN_PATH)

clean:
	rm -f *.o *.d
	rm -f $(PLUGIN)
	rm -f $(EXECUTABLE)
