# Target: make TOP=mux
# command: verilator $(VSRCS) $(CSRCS) \
# 		top=TOP TRACE_ENABLE=--trace 

#the "." means the directionary makefile lies
BUILD_DIR = ./build


VSRCS = $(shell find $(abspath ./vsrcs) -name "*.v")
CSRCS = $(shell find $(abspath ./csrcs) -name "*.c" -o -name "*.cpp" -not -path "*/test/*")
INC_PATH += $(abspath ./csrcs/include)
INC_PATH += $(abspath ./csrcs/test)

VERILATOR_FLAGS = --cc --exe --build
VERILATOR_TRACE_FLAGS = --trace-fst 

TOP ?= 
CXXFLAGS ?= 

include $(NVBOARD_HOME)/scripts/nvboard.mk
CXXFLAGS += -DTOP_NAME=$(TOP)
INC_FLAGS += $(addprefix -I, $(INC_PATH))

# nvboard
NXDC_FILES = constr/$(TOP).nxdc
# LDFLAGS += $(abspath $(NVBOARD_ARCHIVE))

SRC_AUTO_BIND = $(abspath $(BUILD_DIR)/auto_bind.cpp)
$(SRC_AUTO_BIND): $(NXDC_FILES)
	mkdir $(abspath $(BUILD_DIR)) -p
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

SRC_TEST_H = $(abspath ./csrcs)/test/$(TOP)_test.h
$(SRC_TEST_H):
	@rm $(SRC_TEST_H) -f
	@echo "#include \"general.h\" \n\
void TOP_TEST_FUNC(VTOP* top);" >> $(SRC_TEST_H)

CSRCS +=  $(abspath ./csrcs)/test/$(TOP)_test.cpp

## end nvboard

VERILATOR_BASEOPTION = $(VSRCS) $(CSRCS) $(SRC_AUTO_BIND) $(abspath $(NVBOARD_ARCHIVE)) \
					$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
					$(addprefix -CFLAGS , $(INC_FLAGS)) \
					--top $(TOP)  \
					$(VERILATOR_FLAGS)
VERILATOR_DEBUG = --debug --gdbbt

# VERILATOR_BASEOPTION = $(VSRCS) $(CSRCS) $(SRC_AUTO_BIND)  \
# 					$(addprefix -CFLAGS , $(CXXFLAGS)) $(addprefix -LDFLAGS , $(LDFLAGS)) \
# 					$(addprefix -CFLAGS , $(INC_FLAGS)) \
# 					--top $(TOP)  $(VERILATOR_FLAGS)

.PHONY: sim board clean

sim: $(VSRCS) $(CSRCS) $(SRC_AUTO_BIND) $(NVBOARD_ARCHIVE) $(SRC_TEST_H)
	verilator $(VERILATOR_BASEOPTION)  

veri_debug: VERILATOR_BASEOPTION += $(VERILATOR_DEBUG)
veri_debug: sim

trace: CXXFLAGS += -DTRACE_ENABLE
trace: VERILATOR_FLAGS += $(VERILATOR_TRACE_FLAGS)
trace: sim
	./obj_dir/V$(TOP) +trace


board: $(SRC_AUTO_BIND)
board: VERILATOR_BASEOPTION += $(SRC_AUTO_BIND)


clean:
	rm -rf obj_dir logs build
