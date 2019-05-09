#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/error.h"
#include "../include/scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

// Other important constants
#define MAIN_TID 0

int did_init = 0;

// There is no need for it to be public, so I wont them add to the header
int scheduler_free_waiting_thread(int tid);
int scheduler_has_thread_ready();

int scheduler_initialize_queues() {
    if (DEBUG) printf("Start: %s\n", __FUNCTION__);

	ready_low = malloc(sizeof(FILA2));
	ready_medium = malloc(sizeof(FILA2));
	ready_high = malloc(sizeof(FILA2));
	executing = malloc(sizeof(FILA2));
	blocked = malloc(sizeof(FILA2));
	joined = malloc(sizeof(FILA2));
	
	if (ready_low == NULL) return MALLOC_ERROR;
	if (ready_medium == NULL) return MALLOC_ERROR;
	if (ready_high == NULL) return MALLOC_ERROR;
	if (executing == NULL) return MALLOC_ERROR;
    if (blocked == NULL) return MALLOC_ERROR;
    if (joined == NULL) return MALLOC_ERROR;

	if (CreateFila2(ready_low) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
	if (CreateFila2(ready_medium) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
	if (CreateFila2(ready_high) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
	if (CreateFila2(executing) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
    if (CreateFila2(blocked) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
    if (CreateFila2(joined) != SUCCESS_CODE) return LINE_OPERATION_ERROR;

    if (DEBUG) printf("End: %s\n", __FUNCTION__);

	return SUCCESS_CODE;
}

/**
 *  Initializes the main thread, should be called at the start of every main lib function
 *  returns 0 if succeeded and -1 if failed
 */
int scheduler_initialize_main() {
    if (DEBUG) printf("Start: %s\n", __FUNCTION__);

    TCB_t *main_thread = malloc(sizeof(TCB_t));
    main_thread->tid = MAIN_TID;
    main_thread->prio = LOW_PRIO;
    if (scheduler_create_context(&(main_thread->context), NULL) != SUCCESS_CODE) return FAILED;

    if (AppendFila2(executing, (void *) main_thread) != SUCCESS_CODE) return ERROR_CREATING_MAIN;

    if (DEBUG) printf("End: %s\n", __FUNCTION__);

    return SUCCESS_CODE;
}

int scheduler_init() {
    if (DEBUG) printf("Start: %s\n", __FUNCTION__);

    if (!did_init) {
        int queue_result = scheduler_initialize_queues();
        if (DEBUG) printf("Queue result: %d\n", queue_result);
        if (queue_result != SUCCESS_CODE) return queue_result;

        int main_result = scheduler_initialize_main();
        if (DEBUG) printf("Main result: %d\n", main_result);
        if (main_result != SUCCESS_CODE) return main_result;

        did_init = 1;
    }

    if (DEBUG) printf("End: %s\n", __FUNCTION__);

    return SUCCESS_CODE;
}

/**
 * Create a context
 */
int scheduler_create_context(ucontext_t *context, ucontext_t *next) {
    if (DEBUG) printf("Start: %s\n", __FUNCTION__);

	//Laura not sure se isso tbm nao deveria estar no fim da funcao... mas esta funcionando..
    if (getcontext(context) == FAILED) return FAILED;

    if (next == NULL) {
        context->uc_link = 0;
    } else {
        context->uc_link = next;
    }

    char* stack = (char*) malloc(SIGSTKSZ);
    if (stack == NULL) return MALLOC_ERROR;

    context->uc_stack.ss_sp = stack;
    context->uc_stack.ss_size = SIGSTKSZ;

    if (DEBUG) printf("End: %s\n", __FUNCTION__);

    return SUCCESS_CODE;
}

int scheduler_add_to_blocked_queue(TCB_t *thread) {
    if (blocked == NULL) return  NULL_POINTER;
    if (AppendFila2(blocked, thread) != SUCCESS_CODE) return LINE_OPERATION_ERROR;

    thread->state = PROCST_BLOQ;

    return SUCCESS_CODE;
}

int scheduler_remove_from_blocked_queue(TCB_t *thread) {

    if (blocked == NULL) return  NULL_POINTER;
    if (FirstFila2(blocked) != SUCCESS_CODE) return EMPTY_LINE;

    do {
        TCB_t *iteration_thread = (TCB_t *) GetAtIteratorFila2(blocked);

        if (iteration_thread->tid == thread->tid) {
            if (DeleteAtIteratorFila2(blocked) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
            thread->state = PROCST_APTO;
            return SUCCESS_CODE;
        }

    } while (NextFila2(blocked) == SUCCESS_CODE);

    return INVALID_BLOCKED_THREAD;
}

int scheduler_block_thread(csem_t *sem) {

	if (DEBUG) printf("Start: %s\n", __FUNCTION__);

	if (sem == NULL) return NULL_POINTER;
	if (sem->fila == NULL) return NULL_POINTER;

	// Without this, it would cause a deadlock
	if (scheduler_has_thread_ready() != SUCCESS_CODE) return NOTHING_TO_SCHEDULE;
	
	TCB_t *executing_thread = scheduler_get_executing_thread();
	if (executing_thread == NULL) return NULL_POINTER;

	int add_result = scheduler_add_to_blocked_queue(executing_thread);
	if (add_result != SUCCESS_CODE) return add_result;
	
	if (DeleteAtIteratorFila2(executing) != SUCCESS_CODE) return LINE_OPERATION_ERROR;		
	if (AppendFila2(sem->fila, (void *)executing_thread)) return LINE_OPERATION_ERROR;

    return scheduler_schedule_next_thread(&(executing_thread->context));
}

int scheduler_free_thread(csem_t *sem) {

	if (sem == NULL) return NULL_POINTER;
	if (sem->fila == NULL) return NULL_POINTER;
	
	if (FirstFila2(sem->fila) != SUCCESS_CODE) return EMPTY_LINE;
	TCB_t *thread_to_wake = sem->fila->first->node;

	int remove_result = scheduler_remove_from_blocked_queue(thread_to_wake);
	if (remove_result != SUCCESS_CODE) return remove_result;
	
	if (DeleteAtIteratorFila2(sem->fila) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
	return scheduler_insert_in_ready(thread_to_wake);
}

/// Similar to scheduler_get_first_ready_thread, but only tests if there is any thread available
int scheduler_has_thread_ready() {
    if ((ready_high != NULL) && (FirstFila2(ready_high) == SUCCESS_CODE) && (GetAtIteratorFila2(ready_high) != NULL)) {
        return SUCCESS_CODE;

    } else if ((ready_medium != NULL) && (FirstFila2(ready_medium) == SUCCESS_CODE) && (GetAtIteratorFila2(ready_medium) != NULL)) {
        return SUCCESS_CODE;

    } else if ((ready_low != NULL) && (FirstFila2(ready_low) == SUCCESS_CODE) && (GetAtIteratorFila2(ready_low) != NULL)) {
        return SUCCESS_CODE;

    } else {
        return NOTHING_TO_SCHEDULE;
    }
}

/**
 * If there is a thread to be executed, it will set the context and deal with the queues
 * @return: return SUCCESS_CODE when it gets a new thread to be executed otherwise an error
 * next will have the thread to be executed 
 */

int scheduler_get_first_ready_thread( TCB_t** next ) {
	
	if (*next == NULL) return NULL_POINTER;
	
	if ((ready_high != NULL) && (FirstFila2(ready_high) == SUCCESS_CODE)) {
	
		*next = (TCB_t *) GetAtIteratorFila2(ready_high);
		return SUCCESS_CODE;
	
	} else if ((ready_medium != NULL) && (FirstFila2(ready_medium) == SUCCESS_CODE)) {
	
		*next = (TCB_t *) GetAtIteratorFila2(ready_medium);
		return SUCCESS_CODE;
	
	} else if ((ready_low != NULL) && (FirstFila2(ready_low) == SUCCESS_CODE)) {
	
		*next = (TCB_t *) GetAtIteratorFila2(ready_low);
		return SUCCESS_CODE;
	
	} else {
		
		return NOTHING_TO_SCHEDULE;
	}
}

int scheduler_schedule_next_thread(ucontext_t *context_to_leave) {

	if (DEBUG) printf("Start: %s\n", __FUNCTION__);

    if (executing == NULL) return EMPTY_LINE;
    
   	TCB_t *next = malloc(sizeof(TCB_t));
   	if (next == NULL) return MALLOC_ERROR;
    
    int next_result = scheduler_get_first_ready_thread(&next);
    if (next_result != SUCCESS_CODE) return next_result;
	
	int prio = next->prio;
	
	switch (prio) {
        case HIGH_PRIO:
        	
        	if (FirstFila2(ready_high) != SUCCESS_CODE) return EMPTY_LINE;
        	if (DeleteAtIteratorFila2(ready_high) != SUCCESS_CODE) return LINE_OPERATION_ERROR;	
        	break;
        	
        case MEDIUM_PRIO:
        	
        	if (FirstFila2(ready_medium) != SUCCESS_CODE) return EMPTY_LINE;
        	if (DeleteAtIteratorFila2(ready_medium) != SUCCESS_CODE) return LINE_OPERATION_ERROR;	
        	break;
        	
        case LOW_PRIO:
        
            if (FirstFila2(ready_low) != SUCCESS_CODE) return EMPTY_LINE;
        	if (DeleteAtIteratorFila2(ready_low) != SUCCESS_CODE) return LINE_OPERATION_ERROR;	
            break;
            
        default:
            return ERROR_PRIO_NOT_DEFINED;
    }
    
    next->state = PROCST_EXEC;

    if (AppendFila2(executing, (void *) next) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
    if (&(next->context) == NULL) return FAILED;

    if (next->context.uc_stack.ss_sp == NULL) return NULL_POINTER;
	
	if (DEBUG) {
        scheduler_show_state_queues();
		printf("\n - The thread that will be executed: %d\n", next->tid);
	}

    if (context_to_leave == NULL) {
        return setcontext(&(next->context));

    } else {

        return swapcontext(context_to_leave, &(next->context));
    }
}

int scheduler_kill_thread_from_exec() {

	TCB_t *executing_thread = scheduler_get_executing_thread();
	if (executing_thread == NULL) return NULL_POINTER;
	
	executing_thread->state = PROCST_TERMINO;

	if (DeleteAtIteratorFila2(executing) != SUCCESS_CODE) return LINE_OPERATION_ERROR;

	int free_result = scheduler_free_waiting_thread(executing_thread->tid);
	if (free_result != SUCCESS_CODE) return free_result;

	free(executing_thread);

	return SUCCESS_CODE;
}

ucontext_t *scheduler_send_exec_to_ready() {
	
    TCB_t *executing_thread = scheduler_get_executing_thread();
    if (executing_thread == NULL) return NULL;

	executing_thread->state = PROCST_APTO;
	
	if (DeleteAtIteratorFila2(executing) != SUCCESS_CODE) return NULL;

	int insert_ready_result = scheduler_insert_in_ready(executing_thread);
	if (insert_ready_result != SUCCESS_CODE) return NULL;

	return &(executing_thread->context);
}


int scheduler_insert_in_ready(TCB_t *thread) {

	int prio = thread->prio;

	switch (prio) {
        case HIGH_PRIO:
        	return AppendFila2(ready_high, (void *) thread);

        case MEDIUM_PRIO:
        	return AppendFila2(ready_medium, (void *) thread);

        case LOW_PRIO:
            return AppendFila2(ready_low, (void *) thread);

        default:
            return ERROR_PRIO_NOT_DEFINED;
    }
}

TCB_t *scheduler_get_executing_thread() {
    if (executing == NULL) return NULL;
    if (FirstFila2(executing) != SUCCESS_CODE) return NULL;

    return (TCB_t *) GetAtIteratorFila2(executing);
}

// Start cjoin functions

JP_t *scheduler_get_pair_with_blocker(int tid) {
    // If the queue is empty, there is no one waiting
    if (FirstFila2(joined) != SUCCESS_CODE) return NULL;

    do {
        JP_t *join_pair = (JP_t *) GetAtIteratorFila2(joined);
        if (join_pair->blocker_tid == tid) return join_pair;

    } while (NextFila2(joined) == SUCCESS_CODE);

    return NULL;
}

int scheduler_queue_has_thread(PFILA2 queue, int tid) {
    if (FirstFila2(queue) != SUCCESS_CODE) return INVALID_THREAD;

    do {
        TCB_t *thread = (TCB_t *) GetAtIteratorFila2(queue);
        if (thread->tid == tid) return SUCCESS_CODE;

    } while (NextFila2(queue) == SUCCESS_CODE);

    return INVALID_THREAD;
}

int scheduler_thread_exists(int tid) {
    if (scheduler_queue_has_thread(ready_high, tid) == SUCCESS_CODE ||
        scheduler_queue_has_thread(ready_medium, tid) == SUCCESS_CODE ||
        scheduler_queue_has_thread(ready_low, tid) == SUCCESS_CODE ||
        scheduler_queue_has_thread(blocked, tid) == SUCCESS_CODE) {
        return SUCCESS_CODE;

    } else {
        return INVALID_THREAD;
    }
}

int scheduler_wait_thread(int tid) {
    // Without this, it would cause a deadlock
    if (scheduler_has_thread_ready() != SUCCESS_CODE) return NOTHING_TO_SCHEDULE;

    TCB_t *executing_thread = scheduler_get_executing_thread();
    if (executing_thread == NULL) return NULL_POINTER;

    // Actually should not reach here since the executing thread is not tested on scheduler_thread_exists, but is better to be safe than sorry
    if (executing_thread->tid == tid) return INVALID_THREAD;

    JP_t *new_pair = malloc(sizeof(JP_t));
    if (new_pair == NULL) return MALLOC_ERROR;

    new_pair->blocker_tid = tid;
    new_pair->blocked_thread = executing_thread;

    if (FirstFila2(executing) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
    if (DeleteAtIteratorFila2(executing) != SUCCESS_CODE) return LINE_OPERATION_ERROR;
    if (joined == NULL) return NULL_POINTER;
    if (AppendFila2(joined, new_pair)) return LINE_OPERATION_ERROR;

    int add_result = scheduler_add_to_blocked_queue(executing_thread);
    if (add_result != SUCCESS_CODE) return add_result;

    return scheduler_schedule_next_thread(&(executing_thread->context));
}

int scheduler_free_waiting_thread(int tid) {
    JP_t *waiting_pair = scheduler_get_pair_with_blocker(tid);
    if (waiting_pair == NULL) return SUCCESS_CODE;

    if (DeleteAtIteratorFila2(joined) != SUCCESS_CODE) return LINE_OPERATION_ERROR;

    TCB_t *waiting_thread = waiting_pair->blocked_thread;

    free(waiting_pair);

    int remove_result = scheduler_remove_from_blocked_queue(waiting_thread);
    if (remove_result != SUCCESS_CODE) return remove_result;

    return scheduler_insert_in_ready(waiting_thread);
}

// End cjoin functions

void scheduler_show_state_queues() {

	printf("\n\n---------------------READY QUEUES:-------------------\n");

	if (FirstFila2(ready_low) != SUCCESS_CODE) printf("- READY_LOW IS EMPTY!\n");
	else {
		printf("Ready low:\n");
		do {
			TCB_t *thread = (TCB_t *) GetAtIteratorFila2(ready_low);
			printf(" -> Thread id: %d, prio: %d\n", thread->tid, thread->prio);
		} while(NextFila2(ready_low) == SUCCESS_CODE);
	}

	if (FirstFila2(ready_medium) != SUCCESS_CODE) printf("\n- READY_MEDIUM IS EMPTY!\n");
	else {
		printf("Ready medium:\n");
		do {
			TCB_t *thread = (TCB_t *) GetAtIteratorFila2(ready_medium);
			printf(" -> Thread id: %d, prio: %d\n", thread->tid, thread->prio);
		} while(NextFila2(ready_medium) == SUCCESS_CODE);
	}

	if (FirstFila2(ready_high) != SUCCESS_CODE) printf("\n- READY_HIGH IS EMPTY!\n");
	else {
		printf("Ready high:\n");
		do {
			TCB_t *thread = (TCB_t *) GetAtIteratorFila2(ready_high);
			printf("	-> Thread id: %d, prio: %d\n", thread->tid, thread->prio);
		} while(NextFila2(ready_high) == SUCCESS_CODE);
	}

    printf("\n\n---------------------BLOCKED:-------------------\n");

    if (FirstFila2(blocked) != SUCCESS_CODE) printf("- Blocked is empty\n");
    else {
        printf("Blocked:\n");
        do {
            TCB_t *thread = (TCB_t *) GetAtIteratorFila2(blocked);
            printf(" -> Thread id: %d, prio: %d\n", thread->tid, thread->prio);
        } while(NextFila2(blocked) == SUCCESS_CODE);
    }

    printf("\n\n---------------------JOINED:-------------------\n");

    if (FirstFila2(joined) != SUCCESS_CODE) printf("- Joined is empty\n");
    else {
        printf("Joined:\n");
        do {
            JP_t *pair = (JP_t *) GetAtIteratorFila2(joined);
            printf(" -> Thread id: %d, prio: %d, is waiting for thread id: %d\n", pair->blocked_thread->tid, pair->blocked_thread->prio, pair->blocker_tid);
        } while(NextFila2(joined) == SUCCESS_CODE);
    }

	printf("\n\n---------------------EXEC:-------------------\n");

	if (FirstFila2(executing) != SUCCESS_CODE) printf("- executing is empty\n");
	else {
		printf("executing:\n");
		do {
			TCB_t *thread = (TCB_t *) GetAtIteratorFila2(executing);
			printf(" -> Thread id: %d, prio: %d\n", thread->tid, thread->prio);
		} while(NextFila2(ready_high) == SUCCESS_CODE);
	}
}
