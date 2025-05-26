#include <stdio.h>
#include <stdint.h>
#include "led.h"

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
#define IDLE_STACK_START ((SRAM_END) - (4 * SIZE_TASK_STACK))
#define SCHEDULER_STACK_START (SRAM_END - (5 * SIZE_TASK_STACK))

#define TICK_HZ 1000U
#define HSI_CLOCK 16000000U
#define SYS_TIMER_CLOCK HSI_CLOCK

// task handle function
void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);
void idle_task(void);
void init_systick_timer(uint32_t tick_hz);
__attribute__((naked)) init_scheduler_task(uint32_t sched_top_of_stack);

void init_tasks_stack(void);
void enable_processor_faults(void);
__attribute__((naked)) void switch_msp_to_psp(void);
void update_next_task(void);
void save_psp_value(uint32_t current_psp_value);
uint32_t get_psp_value(void);

void task_delay(uint32_t tick_count);

#define INTERRUPT_DISABLE() do{__asm volatile ("MOV R0,#0x1"); asm volatile("MSR PRIMASK,R0");} while(0)
#define INTERRUPT_ENABLE() do{__asm volatile ("MOV R0,#0x0"); asm volatile("MSR PRIMASK,R0");} while(0)

// #define MAX_STACKS 4
#define MAX_TASKS  5
uint32_t g_tick_count = 0;
// uint32_t psp_of_stacks[MAX_STACKS] = {T1_STACK_START, T2_STACK_START, T3_STACK_START, T4_STACK_START};
// uint32_t task_handlers[MAX_STACKS];

uint8_t current_task = 1; // task 1 is running

typedef struct{
    uint32_t psp_value;
    uint32_t block_count;
    uint8_t current_state;
    void(*task_handler)(void);
}TCB_t;

TCB_t user_tasks[MAX_TASKS];

int main (void){
    enable_processor_faults();

    init_scheduler_task(SCHEDULER_STACK_START);

    // task_handlers[0] = (uint32_t)task1_handler;
    // task_handlers[1] = (uint32_t)task2_handler;
    // task_handlers[2] = (uint32_t)task3_handler;
    // task_handlers[3] = (uint32_t)task4_handler;

    led_init_all();

    init_tasks_stack();

    init_systick_timer(TICK_HZ);

    // convert from MSP to PSP
    switch_msp_to_psp();

    task1_handler();

    // loop forever
    for(;;);
}

void idle_task(void){
    while(1); 
}

void task1_handler(void){
    while(1){
        printf("Task 1\n");
        led_on(12);
        task_delay(1000);
        led_off(12);
        task_delay(1000);
    }
}

void task2_handler(void){
    while(1){
        printf("Task 2\n");
        led_on(13);
        task_delay(500);
        led_off(13);
        task_delay(500);
    }
}

void task3_handler(void){
    while(1){
        printf("Task 3\n");
        led_on(14);
        task_delay(250);
        led_off(14);
        task_delay(250);
    }
}

void task4_handler(void){
    while(1){
        printf("Task 4\n");
        led_on(15);
        task_delay(125);
        led_off(15);
        task_delay(125);
    }
}

void init_systick_timer(uint32_t tick_hz){
    uint32_t *pSRVR = (uint32_t *) 0xE000E014;
    uint32_t *pSCSR = (uint32_t *) 0xE000E010;

    // reload value
    uint32_t count_value = (SYS_TIMER_CLOCK / TICK_HZ) - 1;

    // delete SVR 24 bits
    *pSCSR &= ~(0x00FFFFFF);

    // write value into SVR
    *pSCSR |= count_value;

    // enable systick exception request
    *pSRVR |= (1 << 1);

    // clocksource 
    *pSRVR |= (1 << 2);

    // enable counter
    *pSRVR |= (1 << 0);
}

__attribute__((naked)) init_scheduler_task(uint32_t sched_top_of_stack){
    __asm volatile("MSR MSP, %0": : "r"(sched_top_of_stack):);
    __asm volatile("BX LR"); // copy value of LR into PC
}

#define DUMMY_XPSR 0x01000000
#define TASK_RUNNING_STATE 0x00
#define TASK_BLOCKED_STATE 0xFF
void init_tasks_stack(void){
    user_tasks[0].current_state = TASK_RUNNING_STATE;
    user_tasks[1].current_state = TASK_RUNNING_STATE;
    user_tasks[2].current_state = TASK_RUNNING_STATE;
    user_tasks[3].current_state = TASK_RUNNING_STATE;
    user_tasks[4].current_state = TASK_RUNNING_STATE;

    user_tasks[0].psp_value = IDLE_STACK_START;
    user_tasks[1].psp_value = T1_STACK_START;
    user_tasks[2].psp_value = T2_STACK_START;
    user_tasks[3].psp_value = T3_STACK_START;
    user_tasks[4].psp_value = T4_STACK_START;

    user_tasks[0].task_handler = idle_task;
    user_tasks[1].task_handler = task1_handler;
    user_tasks[2].task_handler = task2_handler;
    user_tasks[3].task_handler = task3_handler;
    user_tasks[4].task_handler = task4_handler;

    uint32_t *pPSP;
    for(int i = 0; i < MAX_TASKS; i++){
        pPSP = (uint32_t*)user_tasks[i].psp_value;

        pPSP--; // XPSR reg
        *pPSP = DUMMY_XPSR;

        pPSP--; // PC reg
        *pPSP = (uint32_t*)user_tasks[i].task_handler;
        pPSP--; // LR reg
        *pPSP = 0xFFFFFFFD;

        for(int j = 0; j < 13; j++){
            pPSP--;
            *pPSP = 0;
        }

        // after done, save sPSP
        user_tasks[i].psp_value = (uint32_t)pPSP;
    }
}

void enable_processor_faults(void){
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

uint32_t get_psp_value(void){
    return user_tasks[current_task].psp_value;
}
 
void save_psp_value(uint32_t current_psp_value){
    user_tasks[current_task].psp_value = current_psp_value;
}

void update_next_task(void){
    int state = TASK_BLOCKED_STATE;

    for(int i = 0; i < MAX_TASKS; i++){
        current_task++;
        current_task %= MAX_TASKS;
        state = user_tasks[current_task].current_state;
        if((state == TASK_RUNNING_STATE) && (current_task != 0)) break;
    }

    if(state != TASK_RUNNING_STATE) current_task = 0;
    // current_task++;
    // current_task = current_task % MAX_STACKS;
}

__attribute__((naked)) void switch_msp_to_psp(void){
    //1. initialize the PSP with TASK1 stack start address
    // get value of the PSP at current task
    __asm volatile("PUSH {PR}"); // save LR connect again to main()
    __asm volatile("BL get_psp_value"); // return value save in R0
    __asm volatile("MSR PSP, R0"); // initialize PSP
    __asm volatile("POP {LR}"); // value return in LR

    //2. change MSP to PSP using CONTROL register
    __asm volatile("MOV R0, #0x02");
    __asm volatile("MSR CONTROL, R0");
    __asm volatile("BX LR");
}

void schedule(){
    uint32_t *pICSR = (uint32_t*) 0xE000ED04;
    // pend the pendsv exception
    *pICSR |= (1 << 28);
}

void task_delay(uint32_t tick_count){
    // disable interrupt
    INTERRUPT_DISABLE();

    if(current_task){
        user_tasks[current_task].block_count = g_tick_count + tick_count;
        user_tasks[current_task].current_state = TASK_BLOCKED_STATE;
        schedule();
    }

    // enable interrupt
    INTERRUPT_ENABLE();
}

__attribute__((naked)) void PendSV_Handler(void){
     // save status for task current
    //1. get value PSP of running current task
    __asm volatile("MSR R0, PSP");
    //2. use PSP value that store SF2 (R4 - R11)
    __asm volatile("STMDB R0!, {R4-R11}");
    __asm volatile("PUSH {LR}");
    //3. save current value of PSP
    __asm volatile("BL save_psp_value");

    // context access of the next task
    //1. decide which task will run
    __asm volatile("BL update_next_task");
    //2. get PSP value
    __asm volatile("BL get_psp_value");
    //3. use PSP value to access SF2 (R4 - R11)
    __asm volatile("LDMIA R0!, {R4-R11}");
    //4. update PSP and exit
    __asm volatile("MSR PSP, R0"); 
    __asm volatile("POP {LR}");
    __asm volatile("BX LR");
}

void update_global_tick_count(void){
    g_tick_count++;
}

void unblock_tasks(void){
    for(int i = 1; i < MAX_TASKS; i++){
        if(user_tasks[i].current_state != TASK_RUNNING_STATE){
            if(user_tasks[i].block_count == g_tick_count){
                user_tasks[i].current_state = TASK_RUNNING_STATE;
            }
        }
    }
}

void SysTick_Handler(){
    uint32_t *pICSR = (uint32_t*) 0xE000ED04;

    update_global_tick_count();

    unblock_tasks();

    // pend the pendsv exception
    *pICSR |= (1 << 28);
}