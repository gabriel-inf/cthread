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

execute_$(SET_PRIO): $(SET_PRIO)
	./bin/$(SET_PRIO).o

execute_$(YIELD): $(YIELD)
	./bin/$(YIELD).o

execute_$(SEMAPHORE): $(SEMAPHORE)
	./bin/$(SEMAPHORE).o

execute_tests: full_clean all
	clear
	make execute_$(SET_PRIO)
	make execute_$(YIELD)
	make execute_$(SEMAPHORE)

library:
	cd .. && make all

full_clean:
	cd .. && make clean
