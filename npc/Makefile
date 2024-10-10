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
DIRS-y = 


VERILOG_INCPATH = $(abspath ./vsrc)
VERILOG_INCFLAGS = $(addprefix -I, $(VERILOG_INCPATH))

VERILATOR_FLAGS = --cc --exe --build 
VERILATOR_TRACE_FLAGS = --trace-fst 
TOP = top


############## sim_utils
SIM_UTIL_PATH = ../sim-utils
-include $(NPC_HOME)/include/config/auto.conf
-include $(NPC_HOME)/include/config/auto.conf.cmd

remove_quote = $(patsubst "%",%,$(1))
# Extract variabls from menuconfig
GUEST_ISA ?= $(call remove_quote,$(CONFIG_ISA))

# Include rules for menuconfig
include $(SIM_UTIL_PATH)/scripts/config.mk
# Include all filelist.mk to merge file lists
FILELIST_MK += $(shell find -L ../sim-utils -name "filelist.mk")
include $(FILELIST_MK)

# INC_PATH += $(abspath $(SIM_UTIL_PATH)/$(UTILS_INC_PATH))
INC_PATH += $(abspath include)
CSRCS += $(abspath $(shell find -L $(DIRS-y) -name "*.c"))
###############



# rules for verilator
# $(info ###############$(INC_PATH))
INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS)
CXXFLAGS += -D__GUEST_ISA__=$(GUEST_ISA)



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

NPCEXEC= $(OBJ_DIR)/Vtop $(IMG) $(ARGS)

sim: $(VSRCS) $(CSRCS) $(SRC_AUTO_BIND) $(NVBOARD_ARCHIVE) 
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	verilator $(VERILATOR_BASEOPTION) 

run: sim
	$(NPCEXEC)

veri_debug: VERILATOR_BASEOPTION += $(VERILATOR_DEBUG)
veri_debug: sim


trace: CXXFLAGS += -DTRACE_ENABLE
trace: VERILATOR_FLAGS += $(VERILATOR_TRACE_FLAGS)
trace: sim
	$(NPCEXEC) +trace

board: $(SRC_AUTO_BIND)
board: VERILATOR_BASEOPTION += $(SRC_AUTO_BIND)

reg_test:
	echo "### clean nemu"
	make -C $(NEMU_HOME) clean
	echo "### Test am-add"
	make -C ../am-kernels/tests/cpu-tests run_bach ALL=add ARCH=riscv32-nemu
	echo "### Test npc"
	make sim

# wave:
# 	gtkwave $(TRACE_DIR)/wave.fst
clean:
	rm -rf build obj_dir $(OBJ_DIR)

include ../Makefile
