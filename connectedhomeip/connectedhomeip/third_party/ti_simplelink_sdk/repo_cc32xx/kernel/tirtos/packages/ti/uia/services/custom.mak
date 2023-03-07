#include $(XDCROOT)/packages/xdc/bld/xdc_java.mak
#
#CLASSES = $(patsubst %.java,%,$(wildcard *.java))
#JARFILE = java/package.jar
#
#JCOPTS := -Xlint:unchecked
#
#.libraries: $(JARFILE)
#$(JARFILE): $(patsubst %,$(CLASSDIR)/%.class,$(CLASSES))
#
#clean::
#	$(RM) $(JARFILE)
#
