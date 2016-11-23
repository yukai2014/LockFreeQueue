######################################################################
#    makefile 模版 
#
######################################################################
 
###################项目路径和程序名称#################################
DIR=$(shell pwd)
BIN_DIR=$(DIR)/build
LIB_DIR=$(DIR)
SRC_DIR=$(DIR)
INCLUDE_DIR=$(DIR)
OBJ_DIR=$(DIR)/build
DEPS_DIR=$(DIR)/deps
#PROGRAM=$(BIN_DIR)/test
PROGRAM=$(BIN_DIR)/FixSizeLockFreeQueueTest $(BIN_DIR)/FixSizeLockFreeQueuePerfTest
 
###################OBJ文件及路径############################################
EXTENSION=cpp
OBJS=$(patsubst $(SRC_DIR)/%.$(EXTENSION), $(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.$(EXTENSION)))
DEPS=$(patsubst $(OBJ_DIR)/%.o, $(DEPS_DIR)/%.d, $(OBJS))

###################include头文件路径##################################
INCLUDE=\
        -I$(INCLUDE_DIR)
         
###################lib文件及路径######################################
 
###################编译选项及编译器###################################
CC=g++
LDFLAGS= -pthread -std=c++11
CXXFLAGS =	-O2 -g -Wall -fmessage-length=0 -std=c++11 
 
###################编译目标###########################################
.PHONY: all clean rebuild
 
all:$(PROGRAM)

$(BIN_DIR)/FixSizeLockFreeQueueTest:$(OBJS) 
	$(CC) -o $(BIN_DIR)/FixSizeLockFreeQueueTest $(OBJ_DIR)/FixSizeLockFreeQueueTest.o $(LDFLAGS) 
	
$(BIN_DIR)/FixSizeLockFreeQueuePerfTest:$(OBJS) 
	$(CC) -o $(BIN_DIR)/FixSizeLockFreeQueuePerfTest $(OBJ_DIR)/FixSizeLockFreeQueuePerfTest.o $(LDFLAGS) 
 
#$(DEPS_DIR)/%.d: $(SRC_DIR)/%.$(EXTENSION)
#	$(CC) -MM $(INCLUDE) $(CFLAGS) $< | sed -e 1's,^,$(OBJ_DIR)/,' > $@
 
#sinclude $(DEPS)
 
$(OBJ_DIR)/%.o:$(SRC_DIR)/%.$(EXTENSION) 
	$(CC) $< -o $@ -c $(CXXFLAGS) $(INCLUDE) 
 
rebuild: clean all
 
clean:
	rm -rf $(OBJS)  $(PROGRAM)