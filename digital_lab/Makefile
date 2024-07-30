# Target: make TOP=mux
# command: verilator $(VSRCS) $(CSRCS) \
# 		top=TOP TRACE_ENABLE=--trace 

#the "." means the directionary makefile lies
BUILD_DIR = ./build


VSRCS = $(shell find $(abspath ./vsrcs) -name "*.v")
CSRCS = $(shell find $(abspath ./csrcs) -name "*.c" -o -name "*.cpp" )
INC_PATH += $(abspath ./csrcs/include)
INC_PATH += $(abspath ./csrcs/test)

VERILATOR_FLAGS = --cc --exe --build
VERILATOR_TRACE_FLAGS = --trace-fst 

TOP ?= 
CXXFLAGS ?= 

include $(NVBOARD_HOME)/scripts/nvboard.mk
CXXFLAGS += -DTOP_NAME=$(TOP)
INC_FLAGS += $(addprefix -I, $(INC_PATH))
LDFLAGS += $(NVBOARD_ARCHIVE)

# nvboard
NXDC_FILES = constr/$(TOP).nxdc


SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	mkdir $(abspath $(BUILD_DIR))
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

## end nvboard

VERILATOR_BASEOPTION = $(VSRCS) $(CSRCS) \
					$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
					$(addprefix -CFLAGS , $(INC_FLAGS)) \
					--top $(TOP)  $(VERILATOR_FLAGS)

.PHONY: sim board clean

sim: $(VSRCS) $(CSRCS) $(SRC_AUTO_BIND) $(NVBOARD_ARCHIVE)
	verilator $(VERILATOR_BASEOPTION)  

trace: CXXFLAGS += -DTRACE_ENABLE
trace: VERILATOR_FLAGS += $(VERILATOR_TRACE_FLAGS)
trace: sim
	./obj_dir/V$(TOP) +trace


board: $(SRC_AUTO_BIND)
board: VERILATOR_BASEOPTION += $(SRC_AUTO_BIND)


clean:
	rm -rf obj_dir logs