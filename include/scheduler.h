#ifndef	__SCHEDULER_H__
#define	__SCHEDULER_H__

// LEGAL COLOCAR AS FILAS TODAS AQUI, CERTỌ (INTERROGACAO) (NAO ACHO A INTERROGACAO NO LINUX)

PFILA2 executing, ready;
ucontext_t mcontext;

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


#endif