#
# Makefile de EXEMPLO
#
# OBRIGATÓRIO ter uma regra "all" para geração da biblioteca e de uma
# regra "clean" para remover todos os objetos gerados.
#
# É NECESSARIO ADAPTAR ESSE ARQUIVO de makefile para suas necessidades.
#  1. Cuidado com a regra "clean" para não apagar o "support.o"
#
# OBSERVAR que as variáveis de ambiente consideram que o Makefile está no diretótio "cthread"
# 

CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src
SRC_EXP=./exemplos
TEST_DIR=./testes
TEST_BIN_DIR=$(TEST_DIR)/bin

all: cthread libcthread

cthread: scheduler 
	$(CC) -c $(SRC_DIR)/cthread.c -o $(BIN_DIR)/cthread.o -Wall

scheduler:
	$(CC) -c $(SRC_DIR)/scheduler.c -o $(BIN_DIR)/scheduler.o -Wall

libcthread:
	ar rcs $(LIB_DIR)/libcthread.a $(BIN_DIR)/cthread.o $(BIN_DIR)/scheduler.o $(BIN_DIR)/support.o

clean:
	find $(BIN_DIR)/*.o ! -name 'support.o' -type f -exec rm -f {} +
	rm -rf $(LIB_DIR)/*.a $(BIN_DIR)/*~ $(TEST_DIR)/*~ $(TEST_BIN_DIR)/*~ $(SRC_DIR)/*~ $(INC_DIR)/*~ *~

execute_tests:
	cd $(TEST_DIR) && make execute_tests

linker:
	$(CC) -c $(SRC_DIR)/cthread.c -o $(BIN_DIR)/cthread.o
	$(CC) -c $(SRC_DIR)/scheduler.c -o $(BIN_DIR)/scheduler.o
	$(CC) -o $(SRC_EXP)/teste_semapheia $(TEST_DIR)/test_semaphore.c $(BIN_DIR)/support.o $(BIN_DIR)/scheduler.o $(BIN_DIR)/cthread.o


