# Copyright (c) 2019-2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

.PHONY: default
default: tfz;

edit:
	$(EDITOR) template/template_line.hpp \
	template/sst_template_line.hpp template/crypto_template_line.hpp \
	template/secure_template_line.hpp calls/psa_call.hpp calls/sst_call.hpp \
	calls/crypto_call.hpp calls/security_call.hpp assets/psa_asset.hpp \
	assets/sst_asset.hpp assets/crypto_asset.hpp  utility/data_blocks.hpp \
	utility/gibberish.hpp utility/randomization.hpp \
	utility/find_or_create_asset.hpp utility/string_ops.hpp \
	utility/compute.hpp boilerplate/boilerplate.hpp \
	utility/find_or_create_asset.hpp class_forwards.hpp tf_fuzz.hpp \
	parser/tf_fuzz_grammar.l parser/tf_fuzz_grammar.y \
	template/template_line.cpp \
	template/sst_template_line.cpp template/crypto_template_line.cpp \
	template/secure_template_line.cpp calls/psa_call.cpp calls/sst_call.cpp \
	calls/crypto_call.cpp calls/security_call.cpp assets/psa_asset.cpp \
	assets/sst_asset.cpp assets/crypto_asset.cpp utility/data_blocks.cpp \
	utility/gibberish.cpp utility/randomization.cpp utility/string_ops.cpp \
	utility/compute.cpp \
	boilerplate/boilerplate.cpp tf_fuzz.cpp \
	tests/example_template tests/sstSets tests/sstReads \
	lib/tfm_boilerplate.txt boilerplate/boilerplate.hpp \
	Makefile README assets/README \
	boilerplate/README calls/README demo/README lib/README parser/README \
	template/README tests/README regression/README utility/README \
	visualStudio/README &

edit3:
	$(EDITOR) template/template_line.hpp \
	template/sst_template_line.hpp template/crypto_template_line.hpp \
	template/secure_template_line.hpp calls/psa_call.hpp calls/sst_call.hpp \
	calls/crypto_call.hpp calls/security_call.hpp assets/psa_asset.hpp \
	assets/sst_asset.hpp assets/crypto_asset.hpp  utility/data_blocks.hpp \
	utility/gibberish.hpp utility/randomization.hpp \
	utility/find_or_create_asset.hpp utility/string_ops.hpp \
	utility/compute.hpp boilerplate/boilerplate.hpp \
	utility/find_or_create_asset.hpp class_forwards.hpp tf_fuzz.hpp &
	$(EDITOR) parser/tf_fuzz_grammar.l parser/tf_fuzz_grammar.y \
	template/template_line.cpp \
	template/sst_template_line.cpp template/crypto_template_line.cpp \
	template/secure_template_line.cpp calls/psa_call.cpp calls/sst_call.cpp \
	calls/crypto_call.cpp calls/security_call.cpp assets/psa_asset.cpp \
	assets/sst_asset.cpp assets/crypto_asset.cpp utility/data_blocks.cpp \
	utility/gibberish.cpp utility/randomization.cpp utility/string_ops.cpp \
	utility/compute.cpp \
	boilerplate/boilerplate.cpp tf_fuzz.cpp &
	$(EDITOR) tests/example_template tests/sstSets tests/sstReads \
	lib/tfm_boilerplate.txt boilerplate/boilerplate.hpp \
	Makefile README assets/README \
	boilerplate/README calls/README demo/README lib/README parser/README \
	template/README tests/README regression/README utility/README \
	visualStudio/README &

includes = -I . -I ./parser -I ./template -I ./assets -I ./calls -I ./boilerplate -I ./utility

parser/tf_fuzz_grammar.tab.hpp:  parser/tf_fuzz_grammar.y class_forwards.hpp \
boilerplate/boilerplate.hpp utility/gibberish.hpp utility/string_ops.hpp \
tf_fuzz.hpp utility/data_blocks.hpp utility/find_or_create_asset.hpp assets/psa_asset.hpp \
assets/sst_asset.hpp assets/crypto_asset.hpp calls/psa_call.hpp \
template/template_line.hpp Makefile
	rm -f parser/tf_fuzz_grammar.tab.hpp parser/tf_fuzz_grammar.tab.cpp \
	parser/tf_fuzz_grammar.tab.o
	bison --verbose -d parser/tf_fuzz_grammar.y -o \
	parser/tf_fuzz_grammar.tab.cpp

parser/tf_fuzz_grammar.tab.cpp:  parser/tf_fuzz_grammar.y class_forwards.hpp \
boilerplate/boilerplate.hpp utility/gibberish.hpp utility/string_ops.hpp \
tf_fuzz.hpp utility/data_blocks.hpp utility/find_or_create_asset.hpp assets/psa_asset.hpp \
assets/sst_asset.hpp assets/crypto_asset.hpp calls/psa_call.hpp \
template/template_line.hpp Makefile
	rm -f parser/tf_fuzz_grammar.tab.hpp parser/tf_fuzz_grammar.tab.cpp \
	parser/tf_fuzz_grammar.tab.o
	bison --verbose -d parser/tf_fuzz_grammar.y -o \
	parser/tf_fuzz_grammar.tab.cpp

parser/tf_fuzz_grammar.lex.c:  parser/tf_fuzz_grammar.l \
parser/tf_fuzz_grammar.tab.hpp Makefile
	lex --outfile=parser/tf_fuzz_grammar.lex.c parser/tf_fuzz_grammar.l
parser/tf_fuzz_grammar.lex.o:  parser/tf_fuzz_grammar.lex.c Makefile
	g++ -std=gnu++11 -O0 -g -c -I /usr/include $(includes) -o parser/tf_fuzz_grammar.lex.o \
	parser/tf_fuzz_grammar.lex.c

parser/tf_fuzz_grammar.tab.o:  parser/tf_fuzz_grammar.lex.o \
parser/tf_fuzz_grammar.tab.cpp parser/tf_fuzz_grammar.tab.hpp Makefile
	g++ -Wall -std=c++11 -O0 -g -c -I /usr/include $(includes) -o \
	parser/tf_fuzz_grammar.tab.o parser/tf_fuzz_grammar.tab.cpp

utility/data_block.o:  utility/data_blocks.hpp utility/data_blocks.cpp  Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o utility/data_block.o \
	utility/data_blocks.cpp

template/template_line.o:  utility/randomization.hpp template/template_line.cpp \
class_forwards.hpp boilerplate/boilerplate.hpp tf_fuzz.hpp calls/psa_call.hpp \
assets/psa_asset.hpp utility/data_blocks.hpp template/template_line.hpp assets/sst_asset.hpp \
assets/crypto_asset.hpp  Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o template/template_line.o \
	template/template_line.cpp

template/sst_template_line.o:  template/sst_template_line.cpp class_forwards.hpp \
boilerplate/boilerplate.hpp tf_fuzz.hpp calls/psa_call.hpp assets/psa_asset.hpp \
utility/data_blocks.hpp template/template_line.hpp template/sst_template_line.hpp assets/sst_asset.hpp \
assets/crypto_asset.hpp Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o template/sst_template_line.o \
	template/sst_template_line.cpp

template/crypto_template_line.o:  template/crypto_template_line.cpp \
class_forwards.hpp boilerplate/boilerplate.hpp tf_fuzz.hpp calls/psa_call.hpp \
assets/psa_asset.hpp utility/data_blocks.hpp template/template_line.hpp assets/sst_asset.hpp \
assets/crypto_asset.hpp template/crypto_template_line.hpp Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o template/crypto_template_line.o \
	template/crypto_template_line.cpp

template/secure_template_line.o:  utility/randomization.hpp class_forwards.hpp \
boilerplate/boilerplate.hpp tf_fuzz.hpp calls/psa_call.hpp \
assets/psa_asset.hpp utility/data_blocks.hpp template/secure_template_line.hpp assets/sst_asset.hpp \
assets/crypto_asset.hpp  Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o template/secure_template_line.o \
	template/secure_template_line.cpp

assets/psa_asset.o:  assets/psa_asset.cpp class_forwards.hpp \
boilerplate/boilerplate.hpp tf_fuzz.hpp utility/data_blocks.hpp calls/psa_call.hpp assets/psa_asset.hpp \
template/template_line.hpp Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o assets/psa_asset.o \
	assets/psa_asset.cpp

assets/sst_asset.o:  assets/sst_asset.cpp class_forwards.hpp \
boilerplate/boilerplate.hpp utility/data_blocks.hpp tf_fuzz.hpp calls/psa_call.hpp assets/psa_asset.hpp \
template/template_line.hpp assets/sst_asset.hpp  Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o assets/sst_asset.o \
	assets/sst_asset.cpp

assets/crypto_asset.o:  utility/randomization.hpp assets/crypto_asset.cpp \
class_forwards.hpp boilerplate/boilerplate.hpp tf_fuzz.hpp utility/data_blocks.hpp calls/psa_call.hpp \
assets/psa_asset.hpp assets/crypto_asset.hpp Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o assets/crypto_asset.o \
	assets/crypto_asset.cpp

calls/psa_call.o:  calls/psa_call.cpp class_forwards.hpp \
boilerplate/boilerplate.hpp utility/data_blocks.hpp tf_fuzz.hpp calls/psa_call.hpp Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o calls/psa_call.o calls/psa_call.cpp

calls/sst_call.o:  calls/sst_call.cpp class_forwards.hpp \
boilerplate/boilerplate.hpp tf_fuzz.hpp calls/psa_call.hpp assets/psa_asset.hpp \
template/template_line.hpp utility/data_blocks.hpp calls/sst_call.hpp assets/sst_asset.hpp \
assets/crypto_asset.hpp utility/string_ops.hpp Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o calls/sst_call.o \
	calls/sst_call.cpp

calls/crypto_call.o:  utility/randomization.hpp calls/crypto_call.cpp \
class_forwards.hpp boilerplate/boilerplate.hpp utility/string_ops.hpp \
tf_fuzz.hpp calls/psa_call.hpp utility/data_blocks.hpp assets/psa_asset.hpp template/template_line.hpp \
calls/crypto_call.hpp assets/sst_asset.hpp assets/crypto_asset.hpp Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o calls/crypto_call.o \
	calls/crypto_call.cpp

calls/security_call.o:  utility/randomization.hpp calls/security_call.hpp \
calls/security_call.cpp class_forwards.hpp boilerplate/boilerplate.hpp \
utility/string_ops.hpp utility/data_blocks.hpp tf_fuzz.hpp calls/psa_call.hpp assets/psa_asset.hpp \
template/template_line.hpp calls/security_call.hpp assets/sst_asset.hpp \
Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o calls/security_call.o \
	calls/security_call.cpp

boilerplate/boilerplate.o:  boilerplate/boilerplate.cpp class_forwards.hpp \
boilerplate/boilerplate.hpp tf_fuzz.hpp calls/psa_call.hpp assets/psa_asset.hpp \
template/template_line.hpp Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o boilerplate/boilerplate.o \
	boilerplate/boilerplate.cpp

utility/gibberish.o:  utility/gibberish.cpp class_forwards.hpp \
utility/gibberish.hpp Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o utility/gibberish.o \
	utility/gibberish.cpp

utility/string_ops.o:  utility/string_ops.cpp utility/string_ops.hpp Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o utility/string_ops.o \
	utility/string_ops.cpp

utility/randomization.o:  utility/randomization.cpp utility/randomization.hpp \
Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o utility/randomization.o \
	utility/randomization.cpp

utility/compute.o:  utility/compute.cpp utility/compute.hpp Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o utility/compute.o \
	utility/compute.cpp

tf_fuzz.o:  tf_fuzz.cpp class_forwards.hpp boilerplate/boilerplate.hpp tf_fuzz.hpp \
calls/psa_call.hpp assets/psa_asset.hpp utility/data_blocks.hpp template/template_line.hpp \
parser/tf_fuzz_grammar.tab.hpp Makefile
	g++ -Wall -std=c++11 -O0 -g -c $(includes) -o tf_fuzz.o tf_fuzz.cpp

tfz:  parser/tf_fuzz_grammar.lex.o parser/tf_fuzz_grammar.tab.o \
template/secure_template_line.o template/template_line.o \
template/sst_template_line.o template/crypto_template_line.o utility/data_block.o \
assets/psa_asset.o assets/sst_asset.o assets/crypto_asset.o utility/gibberish.o \
utility/string_ops.o calls/psa_call.o calls/sst_call.o calls/crypto_call.o \
utility/randomization.o utility/compute.o boilerplate/boilerplate.o \
calls/security_call.o tf_fuzz.o \
Makefile
	g++ -Wall -std=c++11 -O0 -g -o tfz parser/tf_fuzz_grammar.lex.o \
	parser/tf_fuzz_grammar.tab.o template/secure_template_line.o \
	template/template_line.o template/sst_template_line.o utility/data_block.o \
	template/crypto_template_line.o assets/psa_asset.o assets/sst_asset.o \
	assets/crypto_asset.o utility/gibberish.o utility/string_ops.o \
	utility/randomization.o utility/compute.o calls/psa_call.o \
	calls/sst_call.o calls/crypto_call.o calls/security_call.o \
	boilerplate/boilerplate.o tf_fuzz.o

clean:
	rm -f ./*.o parser/*.o assets/*.o calls/*.o template/*.o utility/*.o \
	boilerplate/*.o utility/gibberish.o tfz tfz.exe \
	parser/tf_fuzz_grammar.lex.* parser/tf_fuzz_grammar.tab.* \
	parser/tf_fuzz_grammar.output
	rm -f `find regression -name "stdout_stderr"`
	rm -f `find regression -name "test.c"`
	rm -f `find regression -name "diff_*"`
	rm -f demo/tossThis*
