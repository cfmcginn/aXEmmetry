MKDIR_BIN = mkdir -p $(PWD)/bin

ROOT = `root-config --cflags --glibs`
CXX = g++
CXXFLAGS = -Wall -O2 -Wextra -Wno-unused-local-typedefs  -Werror -Wno-deprecated-declarations -std=c++11
ifeq "$(GCCVERSION)" "1"
  CXXFLAGS += -Wno-error=misleading-indentation
endif

#programs to make

all: mkdirBin jetSkim.exe 

mkdirBin:
	$(MKDIR_BIN)

jetSkim.exe: src/jetSkim.C
	$(CXX) $(CXXFLAGS) $(ROOT)  -I $(PWD) -o bin/jetSkim.exe src/jetSkim.C

clean:
	rm -f *~
	rm -f \#*.*#
	rm -f $(PWD)/src/#*.*#
	rm -f $(PWD)/src/*~
	rm -f $(PWD)/include/#*.*#
	rm -f $(PWD)/include/*~
	rm -f $(PWD)/bin/*.exe

	rmdir bin
.PHONY: all

