#ifndef	__SCHEDULER_H__
#define	__SCHEDULER_H__

#include <ucontext.h>

/// Queues of TCB_t
PFILA2 executing, ready_low, ready_medium, ready_high, blocked;

/// Queue of JP_t, thread that called cjoin on another thread
PFILA2 joined;

/// Used to represent a thread that called cjoin and the tid it is waiting
typedef struct s_join_pair {
    TCB_t *blocked_thread;
    int	blocker_tid;
} JP_t;

int scheduler_init();

int scheduler_create_context(ucontext_t *context, ucontext_t *next);

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
 * Receives the thread to leave execution or NULL. In the first case, it swaps the thread with
 a new one to be executed. If there is no, it just set the context to another one.
 * Deals with the queues
 * @return: return if everything was okS
 */
int scheduler_schedule_next_thread(ucontext_t *context_to_leave);

int scheduler_kill_thread_from_exec();

/**
  * Deletes executing thread from executing queue. Updates state its state.
  * @return The context of the executing thread
  */
ucontext_t *scheduler_send_exec_to_ready();

/**
 * Moves the executing iterator to the first node and returns it already cast as TCB_t*
 * @return The executing thread, or NULL if there is any error accessing the node
 */
TCB_t *scheduler_get_executing_thread();

/// Returns the pair with the thread being blocked by tid, or NULL if none
JP_t *scheduler_get_pair_with_blocker(int tid);

int scheduler_thread_exists(int tid);

int scheduler_wait_thread(int tid);

void scheduler_show_state_queues();

#endif
