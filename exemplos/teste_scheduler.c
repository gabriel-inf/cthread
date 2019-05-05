#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/error.h"
#include "../include/scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ucontext.h>

csem_t *semaphore_test;
/*
void test_scheduler_block_thread() {

	csem_t *sem = malloc(sizeof(csem_t));
	PFILA2 fila = malloc(sizeof(FILA2));
	executing = malloc(sizeof(FILA2));

	TCB_t *thread = malloc(sizeof(TCB_t));
	thread->state = PROCST_EXEC;	

	assert(AppendFila2(executing, (void *)thread) == SUCCESS_CODE);
	
	sem->fila = fila;
	sem->count = 10;

	assert(thread->state == PROCST_EXEC);
	assert(FirstFila2(fila) != SUCCESS_CODE);
	assert(FirstFila2(sem->fila) != SUCCESS_CODE);
	assert(FirstFila2(executing) == SUCCESS_CODE);

	assert( scheduler_block_thread(thread, sem) == SUCCESS_CODE);
	assert(thread->state == PROCST_BLOQ);
	assert(FirstFila2(fila) == SUCCESS_CODE);
	assert(FirstFila2(sem->fila) == SUCCESS_CODE);
	assert(FirstFila2(executing) != SUCCESS_CODE);
	assert(executing != NULL);
	assert(fila != NULL);
	assert(sem->fila != NULL);


}*/
/*
void test_scheduler_get_first_ready_thread() {

	ready = malloc(sizeof(FILA2));	
	
	TCB_t *thread = malloc(sizeof(TCB_t));
	thread->tid = 2;
	
	TCB_t *next = malloc(sizeof(TCB_t));
	next->tid = 3;

	assert(AppendFila2(ready, (void *)thread) == SUCCESS_CODE);
	
	assert(scheduler_get_first_ready_thread(&next) == SUCCESS_CODE);

	assert(next->tid == 2);

}

void test_scheduler_insert_in_ready() {

}*/

void process_execution() {

	printf("execution started\n");
	printf("resultado block = %d\n", scheduler_block_thread(semaphore_test));
	printf("execution finishing\n");

}

void ready_process_execution() {

	printf("execution ready process started\n");
	
	assert(ready_medium != NULL);
	assert(FirstFila2(ready_medium) != SUCCESS_CODE);
	assert(executing != NULL);
	assert(semaphore_test->fila != NULL);
	
	assert(FirstFila2(semaphore_test->fila) == SUCCESS_CODE);
	
	TCB_t *waiting_thread = (TCB_t *) semaphore_test->fila->first->node;
	assert( waiting_thread->tid == 9);
	assert( waiting_thread->state == PROCST_BLOQ);
	
	assert( ((TCB_t *) executing->first->node)->tid == 10);
	assert( ((TCB_t *) executing->first->node)->state == PROCST_EXEC );
	
	printf("execution ready finishing\n");
	setcontext(&mcontext);

}


void test_scheduler_block_thread() {

	semaphore_test = malloc(sizeof(csem_t));
	PFILA2 fila = malloc(sizeof(FILA2));
	executing = malloc(sizeof(FILA2));
	ready_medium = malloc(sizeof(FILA2));

	semaphore_test->fila = fila;
	
	TCB_t *exec_thread = malloc(sizeof(TCB_t));
	exec_thread->tid = 9;
	exec_thread->state = PROCST_EXEC;
	
	TCB_t *ready_thread = malloc(sizeof(TCB_t));
	ready_thread->tid = 10;
	ready_thread->state = PROCST_APTO;
	ready_thread->prio = MEDIUM_PRIO;
	
	ucontext_t exec_context, ready_context;
	
	assert (getcontext(&exec_context) != FAILED);
	char *pilha = (char *)malloc(SIGSTKSZ);
	assert (pilha != NULL);
	
	exec_context.uc_stack.ss_sp = pilha;
	exec_context.uc_stack.ss_size = SIGSTKSZ;
	makecontext(&exec_context, process_execution, 0);
	
	assert (getcontext(&ready_context) != FAILED);
	char *pilha2 = (char *)malloc(SIGSTKSZ);
	assert (pilha2 != NULL);
	
	ready_context.uc_stack.ss_sp = pilha2;
	ready_context.uc_stack.ss_size = SIGSTKSZ;
	makecontext(&ready_context, ready_process_execution, 0);
	
	exec_thread->context = exec_context;
	ready_thread->context = ready_context;
	
	assert(AppendFila2(executing, (void *) exec_thread) == SUCCESS_CODE);
	assert(AppendFila2(ready_medium, (void *) ready_thread) == SUCCESS_CODE);
	
	swapcontext(&mcontext, &exec_context);
	
}
/*
void test_scheduler_free_thread() {

	csem_t *sem = malloc(sizeof(csem_t));
	PFILA2 fila = malloc(sizeof(FILA2));
	ready = malloc(sizeof(FILA2));
	
	TCB_t *thread = malloc(sizeof(TCB_t));
	thread->state = PROCST_BLOQ;	
	
	sem->fila = fila;
	
	assert(AppendFila2(sem->fila, (void *)thread) == SUCCESS_CODE);
	
	assert(FirstFila2(sem->fila) == SUCCESS_CODE);
	assert(FirstFila2(ready) != SUCCESS_CODE);
	
	assert(scheduler_free_thread(sem) == SUCCESS_CODE);
	
	assert(FirstFila2(sem->fila) != SUCCESS_CODE);
	assert(FirstFila2(ready) == SUCCESS_CODE);
	assert(thread->state == PROCST_APTO);

}*/

void test_scheduler_get_first_ready_thread1() {

	TCB_t *next = NULL;
	
	int result = scheduler_get_first_ready_thread(&next);
	
	assert(result == -3);

}

void test_scheduler_get_first_ready_thread2() {

	ready_high = malloc(sizeof(FILA2));
	ready_medium = malloc(sizeof(FILA2));
	ready_low = malloc(sizeof(FILA2));
	
	assert(ready_high != NULL);
	assert(ready_medium != NULL);
	assert(ready_low != NULL);
	
	TCB_t *next = malloc(sizeof(TCB_t));

	assert(scheduler_get_first_ready_thread(&next) == NOTHING_TO_SCHEDULE);

}

void test_scheduler_get_first_ready_thread3() {

	ready_high = malloc(sizeof(FILA2));
	ready_medium = malloc(sizeof(FILA2));
	ready_low = malloc(sizeof(FILA2));
	
	TCB_t *next = malloc(sizeof(TCB_t));
	
	TCB_t *thread_high = malloc(sizeof(TCB_t));
	thread_high->tid = 10;
	thread_high->prio = 0;
	
	TCB_t *thread_med = malloc(sizeof(TCB_t));
	thread_med->tid = 9;
	thread_med->prio = 1;
	
	assert( (AppendFila2(ready_high, (void *)thread_high) == SUCCESS_CODE ));
	assert( (AppendFila2(ready_medium, (void *)thread_med) == SUCCESS_CODE ));

	assert(scheduler_get_first_ready_thread(&next) == SUCCESS_CODE);
	
	assert(next->tid == 10);

}

void test_scheduler_get_first_ready_thread4() {

	ready_high = NULL;
	ready_medium = NULL;
	ready_low = malloc(sizeof(FILA2));
	
	TCB_t *next = malloc(sizeof(TCB_t));
	
	TCB_t *thread_low = malloc(sizeof(TCB_t));
	thread_low->tid = 90;
	thread_low->prio = 1;
	
	assert( (AppendFila2(ready_low, (void *)thread_low) == SUCCESS_CODE ));

	assert(scheduler_get_first_ready_thread(&next) == SUCCESS_CODE);
	
	assert(next->tid == 90);

}

int main(int argc, char *argv[]) {

	test_scheduler_block_thread();
	return 0;
}

