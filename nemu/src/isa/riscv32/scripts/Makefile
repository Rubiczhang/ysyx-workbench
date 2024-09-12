YSYX_HOME = $(NEMU_HOME)/..

THIRD_PART=$(YSYX_HOME)/third_party
MAVIS_HOME=$(THIRD_PART)/mavis

ISA = rv32i rv32m
ISA_JSON = $(patsubst %, isa_%.json, $(ISA))

$(ISA_JSON): 
	cp $(MAVIS_HOME)/json/$@ $@

.PHONY: decode_table
decode_table: $(ISA_JSON)
	python3 gen_decode_table.py $^ > decode_table.txt
#	python3 gen_decode_table.py isa_rv32i.json > decode_table.txt

