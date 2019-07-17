CC = gcc

CC_FLAGS := -std=c11 
CC_FLAGS += -g
CC_FLAGS += -pthread

SRV_TARGET = sparkle-vs
CLI_TARGET = sparkle-cli

TOP_DIR = $(PWD)
OBJ_DIR = $(TOP_DIR)/build
BIN_DIR = $(TOP_DIR)/bin
INC_DIR = -I$(TOP_DIR)/include

SRC_FS = $(TOP_DIR)/fs
SRC_CLI = $(TOP_DIR)/cli
SRC_COMMON = $(TOP_DIR)/common
SRC_NETWORK = $(TOP_DIR)/network

OBJ_LIB := $(OBJ_DIR)/logger.o
OBJ_LIB += $(OBJ_DIR)/mutex.o

export CC CC_FLAGS
export SRV_TARGET CLI_TARGET
export TOP_DIR OBJ_DIR BIN_DIR INC_DIR
export SRC_FS SRC_CLI SRC_COMMON SRC_NETWORK
export OBJ_LIB

all: CHECK_DIR $(SRC_FS) $(SRC_CLI) $(SRC_COMMON) $(SRC_NETWORK) $(SRV_TARGET) $(CLI_TARGET)

CHECK_DIR:
    mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(SRC_FS): ECHO
    $(MAKE) -C $@
$(SRC_CLI) : ECHO
    $(MAKE) - C $@
$(SRC_COMMON) : ECHO
    $(MAKE) - C $@
$(SRC_NETWORK) : ECHO
    $(MAKE) - C $@

$(SRV_TARGET):
    $(CC) $(CC_FLAGS) -o $(addprefix $(BINDIR)/, $(SRV_TARGET)) $$(find ./${OBJ_DIR} -name '*.o')
$(CLI_TARGET):
    $(CC) $(CC_FLAGS) -o $(addprefix $(BINDIR)/, $(SRV_TARGET)) $$(find ./${SRC_CLI} -name '*.o')

.PHONY : clean

clean :
    $(MAKE) -C $(SRC_FS) clean
    $(MAKE) -C $(SRC_CLI) clean
    $(MAKE) -C $(SRC_COMMON) clean
    $(MAKE) -C $(SRC_NETWORK) clean
    rm -rf $(OBJ_DIR)

