
.PRECIOUS: %.hex %.bin
%.hex: %.elf
	$(OBJCOPY) $< $@ -O ihex \
			--remove-section .ARM.exidx \
			--remove-section .ARM.extab

%.bin: %.elf
	$(OBJCOPY) $< $@ -O binary \
			--remove-section .ARM.exidx \
			--remove-section .ARM.extab

bin: $(WORKDIR)/$(APPNAME).bin

XML_INFO_TEMPL=$(ENV_PATH)/gppy/generation/programmer/templates/programmer_info_k8a.tmpl
XML_GEN=$(ENV_PATH)/gppy/generation/programmer/generate_programmer.py

PYTHON_BIN = /usr/bin/python3

ifneq (,$(filter $(ARMCHIP), K8A))
# Do not try to compute a CRC for RAM program images.
ifneq ($(PROGRAMMER), ramprogram)
# Start of CRC calculations for k8a. Must be 256-byte aligned.
# Skip the license page which contains the CRC value itself.
CRC_OPTION = --crcstart 0x04000100
endif
endif

$(APPFIRMWARE):
	#To keep make happy and avoid the default %.xml rule is triggered for application firmware
	@$(ECHO) "Nothing to be done for $@"
# Build final XML file used by product programmer tooling, containing reference to HEX file and CRC.

%.xml: %.hex $(APPFIRMWARE)
ifneq (,$(APPFIRMWARE))
	cp -f $(APPFIRMWARE) $(WORKDIR)
endif
ifneq (,$(PRODUCTID))
	@$(ECHO) $(PYTHON_BIN)
	$(PYTHON_BIN) $(XML_GEN) --output $@ --input $(XML_INFO_TEMPL) --hexlink $(notdir $<) $(addprefix --hexlink , $(notdir $(APPFIRMWARE))) --gpproductid $(PRODUCTID) $(CRC_OPTION)
else
	@$(ECHO) "WARNING: No PRODUCTID specified, BOGUS XML generated for $(notdir $<)"
	$(PYTHON_BIN) $(XML_GEN) --output $@ --input $(XML_INFO_TEMPL) --hexlink $(notdir $<) $(addprefix --hexlink , $(notdir $(APPFIRMWARE))) $(CRC_OPTION)
endif

.PRECIOUS: %.xml

###### Create c file with ram routines #####
ramroutines: $(APPFILE)
    #the script counts on ".routineX" sections specified in linkerfile for each function to be extracted */
	$(PYTHON_BIN) $(ENV_PATH)/gppy/generation/ramprogram/hex2ramroutines.py $(<:%.xml=%)

help::
	$(ECHO) "$(BWhite)--- arm gcc specific ---$(Color_Off)\n\
bin               - create a full binary output file\n\
ramroutines       - created c-file with extracted ram routines "
