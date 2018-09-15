CXX = g++
CXXFLAGS = -g -Wall -std=c++11
INCLUDES = -I/usr/local/include
LDFLAGS = -L/usr/local/lib
ARCH := $$(getconf LONG_BIT)
CUDA_PATH = /opt/cuda
CUDA_LDFLAGS = -L$(CUDA_PATH)/lib$(ARCH)
CUDA_INCLUDES = -I$(CUDA_PATH)/include
OPENCV_LDFLAGS = $(CUDA_LDFLAGS)
OPENCV_LIBS = $$(pkg-config --libs opencv)
OPENCV_INCLUDES = $$(pkg-config --cflags opencv) $(CUDA_INCLUDES)
PYTHON_VER = 2
ifeq ($(PYTHON_VER), 2)
  BOOST_LIBS = -lboost_python
else
  BOOST_LIBS = -lboost_python$(PYTHON_VER)
endif
PYTHON_LIBS = $$(pkg-config --libs python$(PYTHON_VER))
PYTHON_INCLUDES = $$(pkg-config --cflags python$(PYTHON_VER))

TARGET = examples

all: $(TARGET).so

$(TARGET).so: $(TARGET).o conversion.o
	$(CXX) -shared  -Wl,--export-dynamic $(LDFLAGS) \
	$(OPENCV_LDFLAGS) $(OPENCV_LIBS) $(BOOST_LIBS) $(PYTHON_LIBS) \
	$(TARGET).o conversion.o -o $(TARGET).so

$(TARGET).o: $(TARGET).cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPENCV_INCLUDES) $(PYTHON_INCLUDES) \
	-fPIC -c $(TARGET).cpp

conversion.o: conversion.cpp conversion.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OPENCV_INCLUDES) $(PYTHON_INCLUDES) \
	-fPIC -c conversion.cpp

clean:
	rm -f *.o *.so

test:
	env python$(PYTHON_VER) -m test
