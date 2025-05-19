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

#define TICK_HZ 1000U
#define HSI_CLOCK 16000000U
#define SYS_TIMER_CLOCK HSI_CLOCK

//task handle function
void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);
void init_systick_timer(uint32_t tick_hz);

int main (void){
    init_systick_timer(TICK_HZ);
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

void init_systick_timer(uint32_t tick_hz){
    uint32_t *pSRVR = (uint32_t *) 0xE000E014;
    uint32_t *pSCSR = (uint32_t *) 0xE000E010;

    //reload value
    uint32_t count_value = (SYS_TIMER_CLOCK / TICK_HZ) - 1;

    //delete SVR 24 bits
    *pSCSR &= ~(0x00FFFFFF);

    //write value into SVR
    *pSCSR |= count_value;

    //enable systick exception request
    *pSRVR |= (1 << 1);

    //clocksource 
    *pSRVR |= (1 << 2);

    //enable counter
    *pSRVR |= (1 << 0);
}

void SysTick_Handler(){}