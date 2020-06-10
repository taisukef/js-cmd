#-------------------------------------------------------------------------------------------------------
# Copyright (C) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE file in the project root for full license information.
#-------------------------------------------------------------------------------------------------------

# for MacOS only
PLATFORM=darwin

LIBRARY_PATH=../ChakraCore/out/Release/
LDIR=$(LIBRARY_PATH)/lib/libChakraCoreStatic.a
IDIR=$(LIBRARY_PATH)/include

CC=g++
SRC=js.cpp
OUTPUT=js

#LIBRARY_PATH=../out/$(BUILD_TYPE)


ifeq (darwin, ${PLATFORM})
ICU4C_LIBRARY_PATH ?= /usr/local/opt/icu4c
CFLAGS=-lstdc++ -std=c++11 -I$(IDIR)
FORCE_STARTS=-Wl,-force_load,
FORCE_ENDS=
LIBS=-framework CoreFoundation -framework Security -lm -ldl -Wno-c++11-compat-deprecated-writable-strings \
	-Wno-deprecated-declarations -Wno-unknown-warning-option -o $(OUTPUT)
LDIR+=$(ICU4C_LIBRARY_PATH)/lib/libicudata.a \
	$(ICU4C_LIBRARY_PATH)/lib/libicuuc.a \
	$(ICU4C_LIBRARY_PATH)/lib/libicui18n.a
else
CFLAGS=-lstdc++ -std=c++0x -I$(IDIR)
FORCE_STARTS=-Wl,--whole-archive
FORCE_ENDS=-Wl,--no-whole-archive
LIBS=-pthread -lm -ldl -licuuc -Wno-c++11-compat-deprecated-writable-strings \
	-Wno-deprecated-declarations -Wno-unknown-warning-option -o $(OUTPUT)
endif

testmake:
	$(CC) $(SRC) $(CFLAGS) $(FORCE_STARTS) $(LDIR) $(FORCE_ENDS) $(LIBS)

.PHONY: clean

clean:
	rm $(OUTPUT)
