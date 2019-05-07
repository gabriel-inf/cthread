cmake_minimum_required(VERSION 3.14)
project(sisopT1 C)

set(CMAKE_C_STANDARD 90)

include_directories(exemplos)
include_directories(include)
include_directories(src)
include_directories(testes)

add_executable(sisopT1
        exemplos/exemplo.c
        exemplos/mandel.c
        exemplos/series.c
        exemplos/teste_scheduler.c
        exemplos/teste_vetor.c
        include/cdata.h
        include/cthread.h
        include/error.h
        include/scheduler.h
        include/support.h
        src/cthread.c
        src/scheduler.c
        testes/test_semaphore.c
        testes/test_yield.c
        testes/test_set_prio.c)
