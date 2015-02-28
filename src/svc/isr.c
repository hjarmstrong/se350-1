/**
 * isr.c -- ISR asembly and C functions to call iprocesses
 */

#include <LPC17xx.h>
#include "../proc/process.h"
#include "../proc/scheduler.h"
#include "../proc/sys_process.h"
#include "../uart.h"
#include "../uart_polling.h"
#include "../printf.h"
#include "../rtx.h"

__asm void UART0_IRQHandler(void) {
	PRESERVE8 ; We Need 8 bit alignment 
	
	IMPORT c_UART0_IRQ_Handler ; We go into a c function to modify the scheduler
	
	PUSH{r4-r11, lr} ; Save the context minus the hardware, The RTI SVC_0 ISR will restore this

	BL c_IRQ_Handler ; This c function sets up the scheduler and invokes the iprocess

    POP{r4-r11, lr} ; restore context

    BX lr ; process continues exicution where it left off
} 

__asm void TIMER0_IRQHandler(void) {
    
	PRESERVE8 ; We Need 8 bit alignment 
	
	IMPORT c_IRQHandler ; We go into a c function to modify the scheduler
	
	PUSH{r4-r11, lr} ; Save the context minus the hardware, The RTI SVC_0 ISR will restore this

	BL c_TIMER0_IRQ_Handler ; This c function sets up the scheduler and invokes the iprocess

    POP{r4-r11, lr} ; restore context

    BX lr ; process continues exicution where it left off
} 

void c_UART0_IRQ_Handler(void) { 
    PCB *p_pcb_old = gp_current_process;
    PCB *p_iproc = gp_pcbs[0];
	
    __disable_irq();
	k_enqueue_process(p_pcb_old->pid);
    gp_current_process = p_iproc;
    
    k_process_switch(p_pcb_old); // Switch to the iprocess

    // When the process is rescheduled it will branch back here to the ISR.
    return;
}

void c_TIMER0_IRQ_Handler(void) {
}
