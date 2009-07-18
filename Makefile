
LIBTST_PATH = $(shell pwd)/../tst1.4
#LIBTST_LIB_PATH = /usr/local/lib
LIBTST_LIB_PATH = $(LIBTST_PATH)/.libs

PYTHON_PATH = /opt/local/Library/Frameworks/Python.framework/Versions/2.6/include/python2.6
PYTHON_LIB_PATH = /opt/local/lib

CFLAGS += -O3

OBJECTS = tstpy.o

INCLUDES = -I$(LIBTST_PATH) -I$(PYTHON_PATH)

LDFLAGS = -L$(LIBTST_LIB_PATH) -Wl,-rpath,$(LIBTST_LIB_PATH) $(LIBTST_LIB_PATH)/libtst.a -L$(PYTHON_LIB_PATH) -lpython2.6

tst.so: $(OBJECTS)
	$(LD) -dynamiclib $(OBJECTS) $(LDFLAGS) -o $@

%.o : %.c
	$(CXX) $(INCLUDES) -c $^ -o $@

clean:
	-rm $(OBJECTS)

LD = g++
