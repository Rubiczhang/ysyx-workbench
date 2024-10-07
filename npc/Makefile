INC_PATH ?=
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir


NXDC_FILES = constr/top.nxdc

include $(NVBOARD_HOME)/scripts/nvboard.mk

TRACE_ENABLE :=
RUN_TRACE :=
TRACE_DIR ?= $(BUILD_DIR)/logs


$(shell mkdir -p $(BUILD_DIR))

# constraint file
SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

PROJECT_ROOT := $(abspath .)
CSRCS := $(wildcard $(PROJECT_ROOT)/csrc/*.cpp)
CSRCS += $(SRC_AUTO_BIND)
VSRCS := $(wildcard $(PROJECT_ROOT)/vsrc/*)


VERILOG_INCPATH = $(abspath ./vsrc)
VERILOG_INCFLAGS = $(addprefix -I, $(VERILOG_INCPATH))

VERILATOR_FLAGS = --cc --exe --build 
VERILATOR_TRACE_FLAGS = --trace-fst 
TOP = top

# rules for verilator
INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS)


VERILATOR_BASEOPTION = $(VSRCS) $(CSRCS) $(SRC_AUTO_BIND) $(abspath $(NVBOARD_ARCHIVE)) \
					$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
					$(addprefix -CFLAGS , $(INC_FLAGS)) \
					$(VERILOG_INCFLAGS) 		\
					--Mdir $(OBJ_DIR)   \
					--top $(TOP)  \
					$(VERILATOR_FLAGS)
VERILATOR_DEBUG = --debug --gdbbt
IMG ?=
ARGS ?=

all:

	@echo "Write this Makefile by your self."

.PHONY: sim clean board run


sim: $(VSRCS) $(CSRCS) $(SRC_AUTO_BIND) $(NVBOARD_ARCHIVE) 
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	verilator $(VERILATOR_BASEOPTION) 

run: sim
	$(OBJ_DIR)/Vtop $(IMG) $(ARGS)

veri_debug: VERILATOR_BASEOPTION += $(VERILATOR_DEBUG)
veri_debug: sim


trace: CXXFLAGS += -DTRACE_ENABLE
trace: VERILATOR_FLAGS += $(VERILATOR_TRACE_FLAGS)
trace: sim
	$(OBJ_DIR)/V$(TOP) +trace

board: $(SRC_AUTO_BIND)
board: VERILATOR_BASEOPTION += $(SRC_AUTO_BIND)

# wave:
# 	gtkwave $(TRACE_DIR)/wave.fst
clean:
	rm -rf build obj_dir $(OBJ_DIR)

include ../Makefile
