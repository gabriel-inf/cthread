#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ucontext.h"

#include "../include/cthread.h"
#include "../include/support.h"
#include "../include/cdata.h"

FILA2 ready, executing, blocked;
ucontext_t *final_context = NULL;
TCB_t *main_tcb = NULL;


//
// Created by gabriel on 11/04/19.
//
int ccreate (void* (*start)(void*), void *arg, int prio) {

    // save the existing context (for the first run is the main)
    ucontext_t *previous_context = (ucontext_t*) malloc(sizeof(ucontext_t));


    // alloc tcb
    TCB_t *tcb = (TCB_t *) malloc(sizeof(TCB_t));


    //


}