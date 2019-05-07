#ifndef	__SCHEDULER_H__
#define	__SCHEDULER_H__

PFILA2 executing, ready_low, ready_medium, ready_high;

/*--------------------------------------------------------------------------------
Função:	Recebe um semaforo e insere a thread que executa no momento na fila do semaforo.
Ret:
	==0, se conseguiu
	!=0, caso contrário
--------------------------------------------------------------------------------*/

int scheduler_block_thread(csem_t *sem);

/*--------------------------------------------------------------------------------
Função:	Recebe um semaforo e libera a primeira das threads que espera em sua fila.
Ret:
	==0, se conseguiu
	!=0, caso contrário
--------------------------------------------------------------------------------*/

int scheduler_free_thread(csem_t *sem);

/*--------------------------------------------------------------------------------
Função:	Recebe uma thread e insere-a no estado apto conforme sua prioridade.
Ret:
	==0, se conseguiu
	!=0, caso contrário
--------------------------------------------------------------------------------*/

int scheduler_insert_in_ready(TCB_t *thread);

/**
 * If there is a thread to be executed, it will set the context and deal with the queues
 * @return: return if everything was ok
 */
int scheduler_schedule_next_thread();

int scheduler_kill_thread_from_exec();

/**
 * Moves the executing iterator to the first node and returns it already cast as TCB_t*
 * @return The executing thread, or NULL if there is any error accessing the node
 */
TCB_t* scheduler_get_executing_thread();

void scheduler_show_state_queues();

#endif
