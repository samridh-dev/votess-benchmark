
VOTESS_CPU_COMPILER       ?= icpx
VOTESS_GPU_COMPILER       ?= icpx
VOROPP_COMPILER           ?= g++
CGAL_COMPILER             ?= g++
QHULL_COMPILER            ?= g++

BUILD_DIR          := cmake/build

.PHONY: all configure build clean

all: configure build

configure:
	mkdir -p $(BUILD_DIR)
	cmake -S . -B $(BUILD_DIR) 				              \
	  -DVOTESS_CPU_COMPILER=$(shell command -v ${VOTESS_CPU_COMPILER})    \
	  -DVOTESS_GPU_COMPILER=$(shell command -v ${VOTESS_GPU_COMPILER})    \
	  -DVOROPP_COMPILER=$(shell command -v ${VOROPP_COMPILER})            \
	  -DCGAL_COMPILER=$(shell command -v ${CGAL_COMPILER})                \
	  -DQHULL_COMPILER=$(shell command -v ${QHULL_COMPILER})

build:
	cmake --build $(BUILD_DIR) --target build_all

clean:
	rm -rf bin/*
	rm -rf $(BUILD_DIR)
	rm -rf votess-cpu/build
	rm -rf votess-gpu/build
	rm -rf voropp/build
	rm -rf cgal/build
	rm -rf qhull/build
