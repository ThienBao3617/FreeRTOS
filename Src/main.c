#include <stdio.h>
#include <stdint.h>

// stack memory
#define SIZE_TASK_STACK (5*1024)
#define SIZE_SCHEDULER_STACK (5*1024)

#define SRAM_START 0x20000000U
#define SIZE_SRAM (128*1024)
#define SRAM_END (SRAM_START + SIZE_SRAM)

#define T1_STACK_START SRAM_END
#define T2_STACK_START (SRAM_END - (1 * SIZE_TASK_STACK))
#define T3_STACK_START (SRAM_END - (2 * SIZE_TASK_STACK))
#define T4_STACK_START (SRAM_END - (3 * SIZE_TASK_STACK))
#define SCHEDULER_STACK_START (SRAM_END - (4 * SIZE_TASK_STACK))

//task handle function
void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);

int main (void){
    //loop forever
    for(;;);
}

void task1_handler(void){
    while(1){
        printf("Task 1\n");
    }
}

void task2_handler(void){
    while(1){
        printf("Task 2\n");
    }
}

void task3_handler(void){
    while(1){
        printf("Task 3\n");
    }
}

void task4_handler(void){
    while(1){
        printf("Task 4\n");
    }
}