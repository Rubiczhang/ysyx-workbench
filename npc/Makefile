INC_PATH ?=
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir


NXDC_FILES = constr/top.nxdc

include $(NVBOARD_HOME)/scripts/nvboard.mk

TRACE_ENABLE :=
RUN_TRACE :=
TRACE_FILE ?= $(BUILD_DIR)/logs/trace.fst


$(shell mkdir -p $(BUILD_DIR))

# constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

CSRCS := $(wildcard csrc/*)
CSRCS += $(SRC_AUTO_BIND)
VSRCS := $(wildcard vsrc/*)

# rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS)



all:

	@echo "Write this Makefile by your self."

build: $(CSRCS) $(VSRCS) $(NVBOARD_ARCHIVE)
	verilator --cc --exe --build $(TRACE_ENABLE) -j 0 \
	--top top  $^ \
	$(addprefix -CFLAGS , $(CXXFLAGS)) \
	$(addprefix -LDFLAGS , $(LDFLAGS))  \
	-DTRACE_FILE=$(TRACE_FILE)

sim: build
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!

run: sim
	./obj_dir/Vtop ${RUN_TRACE}

trace: TRACE_ENABLE=--trace-fst
trace: RUN_TRACE=+trace
trace: run
	gtkwave $(TRACE_FILE)

clean:
	rm -rf build obj_dir

include ../Makefile
