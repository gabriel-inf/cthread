#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"
#include "../include/error.h"
#include "../include/scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ucontext.h>


void *function() {

	printf("thread sendo executada\n");
	return;

}


int test_ccreate() {

	int i = 2;
	assert (ccreate(function, (void *)&i, 0) > 0);
	return 0;

}



int main(int argc, char *argv[]) {

	test_ccreate();
	return 0;
}

