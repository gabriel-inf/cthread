#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ucontext.h"

#include "../include/cthread.h"
#include "../include/support.h"
#include "../include/cdata.h"

// 128 MB
#define STACK_SIZE 131072


// TRÊS NÍVEIS DE PRIORIDADE: 0 (ALTA), 1 (MÉDIA) e 2 (BAIXA)
FILA2 ready, executing, blocked;
ucontext_t *final_context = NULL;
TCB_t *main_tcb = NULL;
int id_count = 0;



/**
 * ccrete is responsible for creating a new threading and adding it to the ready queue
 * @param start: pointer for the function that will be executed
 * @param arg: arguments for the function
 * @param prio: thread priority
 * @return returns a code of SUCCESS or failure
 */
int ccreate (void* (*start)(void*), void *arg, int prio) {

    // save the existing context (for the first run is the main)
    ucontext_t *current_context = (ucontext_t*) malloc(sizeof(ucontext_t));
    if(getcontext(current_context) == -1) {
        return FAILED;
    }

    // this struct will be responsible for maintaining the thread state
    TCB_t *tcb = (TCB_t *) malloc(sizeof(TCB_t));

    current_context->uc_stack.ss_sp = (char*) malloc(STACK_SIZE * sizeof(char));
    current_context->uc_stack.ss_size = STACK_SIZE;
    current_context->uc_link = NULL;

    makecontext(current_context, (void (*) (void)) CB_end_thread, 0);

    tcb->tid = id_count++;
    tcb->state = PROCST_APTO;

    switch (prio){
        case 0:
        case 1:
        case 2:
            tcb->prio = prio;
            AppendFila2(ready, tcb);
            break;
        default:
            return ERROR_PRIO_NOT_DEFINED;
    }



}