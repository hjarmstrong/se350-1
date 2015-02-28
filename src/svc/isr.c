/**
 * isr.c -- ISR asm functions to call iprocesses
 */

__asm void UART0_IRQHandler(void) {
	PRESERVE8 ; We Need 8 bit alignment 
	
	IMPORT c_UART0_IRQ_Handler ; We go into a c function to modify the scheduler
	
	PUSH{r4-r11, lr} ; Save the context minus the hardware, The RTI SVC_0 ISR will restore this

	BL c_UART0_IRQ_Handler ; This c function sets up the scheduler and invokes the iprocess

    POP{r4-r11, lr} ; restore context

    BX lr ; process continues exicution where it left off
} 

__asm void TIMER0_IRQHandler(void) {
    
	PRESERVE8 ; We Need 8 bit alignment 
	
	IMPORT c_TIMER0_IRQ_Handler ; We go into a c function to modify the scheduler
	
	PUSH{r4-r11, lr} ; Save the context minus the hardware, The RTI SVC_0 ISR will restore this

	BL c_TIMER0_IRQ_Handler ; This c function sets up the scheduler and invokes the iprocess

    POP{r4-r11, lr} ; restore context

    BX lr ; process continues exicution where it left off
} 


