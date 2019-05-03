
PFILA2 executing, ready;


//
// Created by gabriel on 11/04/19.
//
int ccreate (void* (*start)(void*), void *arg, int prio) {

    // save the existing context (for the first run is the main)
    //

}


int csem_init(csem_t *sem, int count) {

    PFILA2 pfila = malloc(sizeof(FILA2));

    sem->count = count;
    sem->fila = pfila;

    if (CreateFila2(pfila) == SUCCESS_CODE) {
        return SUCCESS_CODE;
    } else {
        return ERROR_CODE;
    }
}

int cwait(csem_t *sem) {

    sem->count --;
    if (sem->count < 0) {

        if (executing == NULL) return ERROR_CODE;
        if (sem->fila == NULL) return ERROR_CODE;

        if ( FirstFila2(executing) != SUCCESS_CODE ) return ERROR_CODE;
        TCB_t *executing_process = (TCB_t *)executing->first->node;
        executing_process->state = PROCST_BLOQ;

        if (DeleteAtIteratorFila2(executing) != SUCCESS_CODE) return ERROR_CODE;
        if (AppendFila2(sem->fila, &executing_process) != SUCCESS_CODE) return ERROR_CODE;

    }
    return SUCCESS_CODE;
}

int csignal(csem_t *sem) {

    sem->count ++;
    if (sem->count <= 0) {

        if (sem->fila == NULL) return ERROR_CODE;
        if (ready == NULL) return ERROR_CODE;

        TCB_t *process_to_wake = (TCB_t *) sem->fila->first->node;
        process_to_wake->state = PROCST_APTO;
        if (FirstFila2(sem->fila) != SUCCESS_CODE) return ERROR_CODE;
        if (DeleteAtIteratorFila2(sem->fila) != SUCCESS_CODE) return ERROR_CODE;
        if (AppendFila2(ready, &process_to_wake) != SUCCESS_CODE) return ERROR_CODE;

    }

    return SUCCESS_CODE;

}
