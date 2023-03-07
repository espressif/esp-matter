import string
import sys
import getopt
import os
import re

def findMinHeapSize(pathToSizeFile):
     # The heap value is taken from sl_memory_config.h
     # If there is a problem, the default value is 2048
     default_value = 2048

     path = pathToSizeFile[0:pathToSizeFile.find("out/") + len("out/")]
     path = path + "/config/sl_memory_config.h"

     try:
       f = open(path,"r")
     except IOError:
       return default_value

     content = f.read()
     content = content.rstrip("\n\r")
     input_lines = content.splitlines()

     for line in input_lines:
       if "#define SL_HEAP_SIZE" in line:
         cols = line.split()
         return int(cols[2])

     return default_value

def main(argv):
     input_file = ""
     printEnabled = False
     
     try:
       opts, args = getopt.getopt(argv,"phi:",["ifile="])
     except getopt.GetoptError:
       print("size_info_gen.py -i <inputfile>")
       sys.exit(2)

     for opt, arg in opts:
       if opt == '-h':
         print("size_info_gen.py -i <inputfile>")
         sys.exit()
       elif opt == '-p':
         printEnabled = True
       elif opt in ("-i", "--ifile"):
         input_file = arg

     # Only print the input file, if the printing of the results are disabled.
     if printEnabled == False:
       print("input file for code size info is: %s" % input_file)

     f= open(input_file,"r+")
     contents = f.read()
     contents = contents.rstrip("\n\r")
     input_lines = contents.splitlines()
     
     # Initialize counters
     flash_binary_size = 0
     flash_storage_size = 0
     sram_size = 0

     # The following value is taken from sl_memory_config.h
     defined_heap_size = findMinHeapSize(input_file)

     # Do the summation line-by-line
     for line in input_lines:
       cols = line.split()
       if (len(cols) == 3) and (cols[0] != 'section') :
         if cols[0] in ['.zwavenvm', '.simee', '.nvm', '.zwave_nvm']:
           flash_storage_size = flash_storage_size + int(cols[1], 16)
         elif cols[0] in ['.text']:
           flash_binary_size = flash_binary_size + int(cols[1], 16)
         elif cols[0] in ['.data']:
           flash_binary_size = flash_binary_size + int(cols[1], 16)
           sram_size = sram_size + int(cols[1], 16)
         elif cols[0] in [ '.internal_storage']:
           # skip this section
           continue
         elif cols[0] in ['.heap']:
           sram_size = sram_size + defined_heap_size
         else :
           addr = int(cols[2], 16)
           if (addr & int('0x20000000', 16)) != 0:
             sram_size = sram_size + int(cols[1], 16)
           elif addr != 0:
             flash_binary_size = flash_binary_size + int(cols[1], 16)
       elif (len(cols) < 3) and (cols[0] != 'Total'):
         # Name of the file.
         filename_org = cols[0]

     # Remove path in the file name, if any.
     contents = re.sub(filename_org, os.path.basename(filename_org), contents, flags=re.DOTALL)

     # Write to the output file: (This modifies the input file)
     f.seek(0)
     f.write("\n")
     f.write("==========================================================\n")
     f.write("The output of the size tool: (e.g. arm-none-ambi-size.exe)\n")
     f.write("==========================================================\n")
     f.write("\n")
     f.write(contents)
     f.write("\n")
     f.write("\n")
     f.write("The calculated FLASH and SRAM usage summary:\n")
     f.write("============================================\n")
     f.write("FLASH used as program memory:  (Including only the sections: .text, .ARM.exidx, .data, _cc_handlers_v3)\n")
     f.write("   " + str(flash_binary_size) + "\n")
     f.write("FLASH used for storage: (Including only the sections: .zwavenvm, .simee, .nvm, .zwave_nvm)\n")
     f.write("   " + str(flash_storage_size) + "\n")
     f.write("SRAM usage:             (Including only the sections: .data, .bss, .heap (limited to " + str(defined_heap_size) + " per sl_memory_config.h), .stack_dummy, .reset_info)\n")
     f.write("   " + str(sram_size) + "\n")
     f.write("\n")

     # Print generated file content
     if printEnabled == True:
       f.seek(0)
       contents = f.read()
       print(contents)

     f.close()
if __name__== "__main__":
  main(sys.argv[1:])