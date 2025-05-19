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
__attribute__((naked)) init_scheduler_task(uint32_t sched_top_of_stack);

init_task_stack(void);
enable_processor_faults(void);

#define MAX_STACKS 4
uint32_t psp_of_stacks[MAX_STACKS] = {T1_STACK_START, T2_STACK_START, T3_STACK_START, T4_STACK_START};
uint32_t task_handlers[MAX_STACKS];

int main (void){
    enable_processor_faults();
    init_scheduler_task(SCHEDULER_STACK_START);

    task_handlers[0] = (uint32_t)task1_handler;
    task_handlers[1] = (uint32_t)task2_handler;
    task_handlers[2] = (uint32_t)task3_handler;
    task_handlers[3] = (uint32_t)task4_handler;

    init_task_stack();

    // convert from MSP to PSP
    switch_msp_to_psp();

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

__attribute__((naked)) init_scheduler_task(uint32_t sched_top_of_stack){
    __asm volatile("MSR MSP, %0": : "r"(sched_top_of_stack):);
    __asm volatile("BX LR"); //copy value of LR into PC
}

#define DUMMY_XPSR 0x01000000
init_task_stack(void){
    uint32_t *pPSP;
    for(int i = 0; i < MAX_STACKS; i++){
        pPSP = (uint32_t*)psp_of_stacks[i];

        pPSP--; // XPSR reg
        *pPSP = DUMMY_XPSR;

        pPSP--; // PC reg
        *pPSP = task_handlers[i];

        pPSP--; // LR reg
        *pPSP = 0xFFFFFFFD;

        for(int j = 0; j < 13; j++){
            pPSP--;
            *pPSP = 0;
        }

        // after done, save sPSP
        psp_of_stacks[i] = (uint32_t)pPSP;
    }
}

enable_processor_faults(void){
    // 1 enable all config exception like usage fault, mem manage fault and bus fault
    uint32_t *pSHCSR = (uint32_t*)0xE000ED24;

    *pSHCSR |= (1 << 16); // mem manage fault
    *pSHCSR |= (1 << 17); // bus fault
    *pSHCSR |= (1 << 18); // usage fault
}

void HardFault_Handler(void){
    printf("Exception: HardFault\n");
    while(1);
}

void MemManageFault_Handler(void){
    printf("Exception: MemManageFault\n");
    while(1);
}

void BusFault_Handler(void){
    printf("Exception: Busfault\n");
    while(1);
}

void SysTick_Handler(){}