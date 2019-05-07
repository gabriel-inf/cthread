#
# Makefile para testes
# 

CC=gcc
LIB_DIR=../lib
SRC_DIR=../src
BIN_DIR=../bin
TEST_BIN_DIR=./bin
TEST_DIR=../testes

SET_PRIO=test_set_prio
YIELD=test_yield
SEMAPHORE=test_semaphore

all: $(SET_PRIO) $(YIELD) $(SEMAPHORE)

$(SET_PRIO): $(SET_PRIO).c library
	$(CC) -o $(TEST_BIN_DIR)/$(SET_PRIO).o $(SET_PRIO).c -L$(LIB_DIR) -lcthread -Wall

$(YIELD): $(YIELD).c library
	$(CC) -o $(TEST_BIN_DIR)/$(YIELD).o $(YIELD).c -L$(LIB_DIR) -lcthread -Wall

$(SEMAPHORE): $(SEMAPHORE).c library
	$(CC) -o $(TEST_BIN_DIR)/$(SEMAPHORE).o $(SEMAPHORE).c -L$(LIB_DIR) -lcthread -Wall

clean:
	rm -rf $(TEST_BIN_DIR)/*.o *~

execute_tests: clean clean_lib all
	clear
	./bin/$(SET_PRIO).o
	./bin/$(YIELD).o
	./bin/$(SEMAPHORE).o

library:
	cd .. && make all

clean_lib:
	cd .. && make clean