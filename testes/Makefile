#
# Makefile para testes
# 

CC=gcc
LIB_DIR=../lib
SRC_DIR=../src
BIN_DIR=../bin
TEST_BIN_DIR=./bin
TEST_DIR=../testes

all: ccreate_test

ccreate_test: ccreate_test.c $(LIB_DIR)/libcthread.a
	$(CC) -o $(TEST_BIN_DIR)/ccreate_test.o ccreate_test.c -L$(LIB_DIR) -lcthread -Wall

clean:
	rm -rf $(TEST_BIN_DIR)/*.o *~

execute_tests: clean clean_lib dependencies ccreate_test
	clear
	./bin/ccreate_test.o

# THIS IS FROM THE OTHER MAKEFILE

dependencies: cthread libcthread

cthread: scheduler 
	$(CC) -c $(SRC_DIR)/cthread.c -o $(BIN_DIR)/cthread.o
	

scheduler:
	$(CC) -c $(SRC_DIR)/scheduler.c -o $(BIN_DIR)/scheduler.o

libcthread:
	ar rcs $(LIB_DIR)/libcthread.a $(BIN_DIR)/cthread.o $(BIN_DIR)/scheduler.o $(BIN_DIR)/support.o

clean_lib:
	find $(BIN_DIR)/*.o ! -name 'support.o' -type f -exec rm -f {} +
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*~ $(SRC_DIR)/*~ $(INC_DIR)/*~ *~
	
linker:
	clear
	$(CC) -c $(SRC_DIR)/cthread.c -o $(BIN_DIR)/cthread.o
	$(CC) -c $(SRC_DIR)/scheduler.c -o $(BIN_DIR)/scheduler.o
	$(CC) -o $(TEST_DIR)/ccreate_test $(TEST_DIR)/ccreate_test.c $(BIN_DIR)/support.o $(BIN_DIR)/scheduler.o $(BIN_DIR)/cthread.o
	./$(TEST_DIR)/ccreate_test
