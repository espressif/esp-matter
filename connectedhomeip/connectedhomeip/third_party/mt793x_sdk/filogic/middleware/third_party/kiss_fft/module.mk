
###################################################
# Sources
KISS_FFT_SRC = middleware/third_party/kiss_fft

C_FILES   += $(KISS_FFT_SRC)/kiss_fft.c           


###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/middleware/third_party/kiss_fft



