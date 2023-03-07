
SPEEX_SRC = middleware/third_party/speex

C_FILES  += $(SPEEX_SRC)/src/framing.c                      
C_FILES  += $(SPEEX_SRC)/src/bitwise.c                      


C_FILES  += $(SPEEX_SRC)/libspeex/bits.c                     
C_FILES  += $(SPEEX_SRC)/libspeex/cb_search.c                
C_FILES  += $(SPEEX_SRC)/libspeex/exc_5_64_table.c           
C_FILES  += $(SPEEX_SRC)/libspeex/exc_5_256_table.c          
C_FILES  += $(SPEEX_SRC)/libspeex/exc_8_128_table.c          
C_FILES  += $(SPEEX_SRC)/libspeex/exc_10_16_table.c          
C_FILES  += $(SPEEX_SRC)/libspeex/exc_10_32_table.c          
C_FILES  += $(SPEEX_SRC)/libspeex/exc_20_32_table.c          
C_FILES  += $(SPEEX_SRC)/libspeex/filters.c                  
C_FILES  += $(SPEEX_SRC)/libspeex/gain_table.c               
C_FILES  += $(SPEEX_SRC)/libspeex/gain_table_lbr.c           
C_FILES  += $(SPEEX_SRC)/libspeex/hexc_10_32_table.c         
C_FILES  += $(SPEEX_SRC)/libspeex/hexc_table.c               
C_FILES  += $(SPEEX_SRC)/libspeex/high_lsp_tables.c          
#C_FILES  += $(SPEEX_SRC)/libspeex/kiss_fft.c                 
#C_FILES  += $(SPEEX_SRC)/libspeex/kiss_fftr.c                
C_FILES  += $(SPEEX_SRC)/libspeex/lpc.c                      
C_FILES  += $(SPEEX_SRC)/libspeex/lsp.c                      
C_FILES  += $(SPEEX_SRC)/libspeex/lsp_tables_nb.c            
C_FILES  += $(SPEEX_SRC)/libspeex/ltp.c                      
C_FILES  += $(SPEEX_SRC)/libspeex/modes.c                    
C_FILES  += $(SPEEX_SRC)/libspeex/modes_wb.c                 
C_FILES  += $(SPEEX_SRC)/libspeex/nb_celp.c                  
C_FILES  += $(SPEEX_SRC)/libspeex/quant_lsp.c                
C_FILES  += $(SPEEX_SRC)/libspeex/sb_celp.c                  
C_FILES  += $(SPEEX_SRC)/libspeex/smallft.c                  
C_FILES  += $(SPEEX_SRC)/libspeex/speex.c                    
C_FILES  += $(SPEEX_SRC)/libspeex/speex_callbacks.c          
C_FILES  += $(SPEEX_SRC)/libspeex/stereo.c                               
C_FILES  += $(SPEEX_SRC)/libspeex/vbr.c                      
C_FILES  += $(SPEEX_SRC)/libspeex/vorbis_psy.c               
C_FILES  += $(SPEEX_SRC)/libspeex/vq.c                       
C_FILES  += $(SPEEX_SRC)/libspeex/window.c                   

#################################################################################
# include path
CFLAGS  += -DHAVE_CONFIG_H
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/util/include
CFLAGS 	+= -I$(SOURCE_DIR)/middleware/MTK/minicli/inc
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/include
CFLAGS  += -I$(SOURCE_DIR)/kernel/rtos/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS  += -I$(SOURCE_DIR)/kernel/service/inc
CFLAGS  += -I$(SOURCE_DIR)/driver/chip/inc
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/speex
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/speex/libspeex
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/speex/src
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/speex/include
CFLAGS  += -I$(SOURCE_DIR)/middleware/third_party/speex/include/ogg
