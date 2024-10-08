#***************************************************************************************
# Copyright (c) 2014-2022 Zihao Yu, Nanjing University
#
# NEMU is licensed under Mulan PSL v2.
# You can use this software according to the terms and conditions of the Mulan PSL v2.
# You may obtain a copy of Mulan PSL v2 at:
#          http://license.coscl.org.cn/MulanPSL2
#
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
# EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
# MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
#
# See the Mulan PSL v2 for more details.
#**************************************************************************************/

# SIM_UTIL_PATH =  ../sim-utils

# include $(SIM_UTIL_PATH)/filelist.mk
# DIRS-y += $(addprefix $(SIM_UTIL_PATH)/, $(UTILS_SRCS_DIR))

SRCS-y += src/nemu-main.c
DIRS-y += src/cpu src/monitor src/utils
DIRS-$(CONFIG_UT) += src/test
DIRS-$(CONFIG_MODE_SYSTEM) += src/memory
DIRS-BLACKLIST-$(CONFIG_TARGET_AM) += src/monitor/sdb

# temp modification, src/monitor and src/monitor/sdb would be deleted.
DIRS-BLACKLIST-y += src/monitor src/monitor/sdb				
SRCS-BLACKLIST-y += src/utils/timer.c

# INC_PATH += $(abspath $(SIM_UTIL_PATH)/$(UTILS_INC_PATH))

SHARE = $(if $(CONFIG_TARGET_SHARE),1,0)
LIBS += $(if $(CONFIG_TARGET_NATIVE_ELF),-lreadline -ldl -pie,)

ifdef mainargs
ASFLAGS += -DBIN_PATH=\"$(mainargs)\"
endif
SRCS-$(CONFIG_TARGET_AM) += src/am-bin.S
.PHONY: src/am-bin.S
