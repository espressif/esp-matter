
XML_SRC = middleware/third_party/xml

C_FILES  += $(XML_SRC)/src/mxml-attr.c     \
               $(XML_SRC)/src/mxml-entity.c   \
               $(XML_SRC)/src/mxml-file.c     \
               $(XML_SRC)/src/mxml-get.c      \
               $(XML_SRC)/src/mxml-index.c    \
               $(XML_SRC)/src/mxml-node.c     \
               $(XML_SRC)/src/mxml-private.c  \
               $(XML_SRC)/src/mxml-search.c   \
               $(XML_SRC)/src/mxml-set.c      \
               $(XML_SRC)/src/mxml-string.c
			 

#################################################################################
#include path
CFLAGS     += -I$(SOURCE_DIR)/middleware/third_party/xml/inc
CFLAGS     += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
