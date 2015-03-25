#include <LPC17xx.h>
#include "../rtx.h"
#include "uart.h"

#include "../core/mem.h"
#include "../core/process.h"
#include "../core/scheduler.h"
#include "../sys/uart.h"
#include "../sys/uart_polling.h"
#include "../sysproc/crt.h" // for crt_send_char
#include "../util/string.h"

static char *gp_input_buffer;
static char *gp_output_buffer;

U8 gp_buffer_index = 0;

U8 out_index;

static LPC_UART_TypeDef *pUart;
static volatile int gp_ready = 1;
static msgbuf *gp_echoBuffer = NULL;
msgbuf *gp_msgBuffer = NULL;

U32 uart_irq_init(U32 n_uart) {
    gp_input_buffer = request_memory_block();
    gp_output_buffer = request_memory_block();
    gp_echoBuffer = request_memory_block();

    if (n_uart == 0) {
        /*
        Steps 1 & 2: system control configuration.
        Under CMSIS, system_LPC17xx.c does these two steps
         
        -----------------------------------------------------
        Step 1: Power control configuration. 
                See table 46 pg63 in LPC17xx_UM
        -----------------------------------------------------
        Enable UART0 power, this is the default setting
        done in system_LPC17xx.c under CMSIS.
        Enclose the code for your refrence
        //LPC_SC->PCONP |= BIT(3);
    
        -----------------------------------------------------
        Step2: Select the clock source. 
               Default PCLK=CCLK/4 , where CCLK = 100MHZ.
               See tables 40 & 42 on pg56-57 in LPC17xx_UM.
        -----------------------------------------------------
        Check the PLL0 configuration to see how XTAL=12.0MHZ 
        gets to CCLK=100MHZin system_LPC17xx.c file.
        PCLK = CCLK/4, default setting after reset.
        Enclose the code for your reference
        //LPC_SC->PCLKSEL0 &= ~(BIT(7)|BIT(6));    
            
        -----------------------------------------------------
        Step 5: Pin Ctrl Block configuration for TXD and RXD
                See Table 79 on pg108 in LPC17xx_UM.
        -----------------------------------------------------
        Note this is done before Steps3-4 for coding purpose.
        */
        
        /* Pin P0.2 used as TXD0 (Com0) */
        LPC_PINCON->PINSEL0 |= (1 << 4);  
        
        /* Pin P0.3 used as RXD0 (Com0) */
        LPC_PINCON->PINSEL0 |= (1 << 6);  

        pUart = (LPC_UART_TypeDef *) LPC_UART0;     
        
    } else if (n_uart == 1) {
        
        /* see Table 79 on pg108 in LPC17xx_UM */ 
        /* Pin P2.0 used as TXD1 (Com1) */
        LPC_PINCON->PINSEL4 |= (2 << 0);

        /* Pin P2.1 used as RXD1 (Com1) */
        LPC_PINCON->PINSEL4 |= (2 << 2);          

        pUart = (LPC_UART_TypeDef *) LPC_UART1;
        
    } else {
        return 1; /* not supported yet */
    } 
    
    /*
    -----------------------------------------------------
    Step 3: Transmission Configuration.
            See section 14.4.12.1 pg313-315 in LPC17xx_UM 
            for baud rate calculation.
    -----------------------------------------------------
        */
    
    /* Step 3a: DLAB=1, 8N1 */
    pUart->LCR = UART_8N1; /* see uart.h file */ 

    /* Step 3b: 115200 baud rate @ 25.0 MHZ PCLK */
    pUart->DLM = 0; /* see table 274, pg302 in LPC17xx_UM */
    pUart->DLL = 9;    /* see table 273, pg302 in LPC17xx_UM */
    
    /* FR = 1.507 ~ 1/2, DivAddVal = 1, MulVal = 2
       FR = 1.507 = 25MHZ/(16*9*115200)
       see table 285 on pg312 in LPC_17xxUM
    */
    pUart->FDR = 0x21;       
    
 

    /*
    ----------------------------------------------------- 
    Step 4: FIFO setup.
           see table 278 on pg305 in LPC17xx_UM
    -----------------------------------------------------
        enable Rx and Tx FIFOs, clear Rx and Tx FIFOs
    Trigger level 0 (1 char per interrupt)
    */
    
    pUart->FCR = 0x07;

    /* Step 5 was done between step 2 and step 4 a few lines above */

    /*
    ----------------------------------------------------- 
    Step 6 Interrupt setting and enabling
    -----------------------------------------------------
    */
    /* Step 6a: 
       Enable interrupt bit(s) wihtin the specific peripheral register.
           Interrupt Sources Setting: RBR, THRE or RX Line Stats
       See Table 50 on pg73 in LPC17xx_UM for all possible UART0 interrupt sources
       See Table 275 on pg 302 in LPC17xx_UM for IER setting 
    */
    /* disable the Divisior Latch Access Bit DLAB=0 */
    pUart->LCR &= ~(BIT(7)); 
    
    //pUart->IER = IER_RBR | IER_THRE | IER_RLS; 
    pUart->IER = IER_RBR | IER_RLS;

    /* Step 6b: enable the UART interrupt from the system level */
    
    if ( n_uart == 0 ) {
        NVIC_EnableIRQ(UART0_IRQn); /* CMSIS function */
    } else if ( n_uart == 1 ) {
        NVIC_EnableIRQ(UART1_IRQn); /* CMSIS function */
    } else {
        return 1; /* not supported yet */
    }
    pUart->THR = '\0';
    return 0;
}

#ifdef _DEBUG_HOTKEYS
#define DEBUG_BUFFER_SIZE 40

static char display_buffer[DEBUG_BUFFER_SIZE];

static void cls() {
    strncpy(display_buffer, "\r\n", DEBUG_BUFFER_SIZE);
    uart1_put_string(display_buffer);
}

// Assumes pid is less than three digits
static void copy_integer(int *idx, int pid) {
	  if (pid >= 10) {
    		display_buffer[(*idx)++] = (pid / 10) + '0';
		}
    display_buffer[(*idx)++] = (pid % 10) + '0';
}

static void debug_hotkeys(char key) {
	  int i, j, idx;

		if (!strncmp(&key, "!", 1)) {
				strncpy(display_buffer, "Ready Queues", 12);
				uart1_put_string(display_buffer);
				cls();
				for (i = 0; i < PNULL; ++i) {
						strncpy(display_buffer, "> Queue X:", 10);
						display_buffer[8] = i + '0';

						idx = strlen(display_buffer);
						for (j = 0; j < NUM_PROCS; ++j) {
								if (g_queues[i][j] != NULL) {
										display_buffer[idx++] = ' ';
										copy_integer(&idx, g_queues[i][j]->pid);
								}
						}
						uart1_put_string(display_buffer);
						cls();
				}
		} else if (!strncmp(&key, "@", 1)) {
				strncpy(display_buffer, "Blocked on Receive Queue", 25);
				uart1_put_string(display_buffer);
				cls();

				strncpy(display_buffer, "> Queue:", 8);

				idx = strlen(display_buffer);
				for (i = 0; i < NUM_PROCS; ++i) {
						if (g_queues[PRIORITY_BLOCKED_ON_RECEIVE][i] != NULL) {
								display_buffer[idx++] = ' ';
								copy_integer(&idx, g_queues[PRIORITY_BLOCKED_ON_RECEIVE][i]->pid);
						}
				}
				uart1_put_string(display_buffer);
				cls();
		} else if (!strncmp(&key, "#", 1)) {
				strncpy(display_buffer, "Blocked on Memory Queue", 24);
				uart1_put_string(display_buffer);
				cls();

				strncpy(display_buffer, "> Queue:", 8);

				idx = strlen(display_buffer);
				for (i = 0; i < NUM_PROCS; ++i) {
						if (g_queues[PRIORITY_BLOCKED_ON_MEMORY][i] != NULL) {
								display_buffer[idx++] = ' ';
								copy_integer(&idx, g_queues[PRIORITY_BLOCKED_ON_MEMORY][i]->pid);
						}
				}
				uart1_put_string(display_buffer);
				cls();
        } else if (!strncmp(&key, "$", 1)) {
                print_memory();
		}
}
#endif // _DEBUG_HOTKEYS

void c_UART0_IRQ_Handler(void) { 
    uint8_t IIR_IntId;        // Interrupt ID from IIR 
    char input_character, output_character;    
    LPC_UART_TypeDef *pUart = (LPC_UART_TypeDef *)LPC_UART0;

    /* Reading IIR automatically acknowledges the interrupt */
    IIR_IntId = (pUart->IIR) >> 1 ; // skip pending bit in IIR 

    if (IIR_IntId & IIR_RDA) { 
        
        /* read UART. Read RBR will clear the interrupt */
         input_character = pUart->RBR;
			   
			   #ifdef _DEBUG_HOTKEYS
		     debug_hotkeys(input_character);
			   #endif // _DEBUG_HOTKEYS

         // Echo -- caller keeps pointer
         gp_echoBuffer->mtype = CALLER_MANAGED_PRINT;
         gp_echoBuffer->mtext[0] = input_character;
         if (input_character == '\r') {
            gp_echoBuffer->mtext[1] = '\n';
            gp_echoBuffer->mtext[2] = '\0';
         } else {
            gp_echoBuffer->mtext[1] = '\0';
         }
         k_send_message(PID_CRT, gp_echoBuffer);
         
         // Invoke KCD
         if (input_character == '\r' || input_character == '\n' || get_input_buffer_size() == gp_buffer_index) {
             while (!gp_msgBuffer || gp_msgBuffer == (void *) 0xFFFFFFFF) { // msg block is pending.
                 k_release_processor();
             }
             // message envelope should contain the input buffer
             gp_msgBuffer->mtype = DEFAULT;
             strncpy(gp_msgBuffer->mtext, gp_input_buffer, gp_buffer_index);
             gp_msgBuffer->mtext[gp_buffer_index] = 0;
             k_send_message(PID_KCD, gp_msgBuffer);
             gp_buffer_index = 0;
         } else {
             gp_input_buffer[gp_buffer_index] = input_character;
             gp_buffer_index++;
         }
         
    } else if (IIR_IntId & IIR_THRE) {
        /* THRE Interrupt, transmit holding register becomes empty */
    
        if (gp_output_buffer[out_index] != '\0' ) { 
            output_character = gp_output_buffer[out_index];
            pUart->THR = output_character;
            out_index++;
        } else {
            // We hit the end of the string to be writen, reset and stop interupts
            pUart->IER ^= IER_THRE; // toggle the IER_THRE bit 
            pUart->THR = '\0';
            out_index = 0;        
      	    gp_ready = 1;
        }
        return; // Writing a character does not preemt
    }    
    
    k_release_processor(); // The interupts preemt the interupted process.

    // When the process is rescheduled it will branch back here to the ISR.
    return;
}

int get_input_buffer_size(void) {
    return BLOCK_SIZE - sizeof(msgbuf);
}
int get_output_buffer_size(void) {
    return BLOCK_SIZE - sizeof(msgbuf);
}
void crt_write_output_buffer(const char* c) {
    strncpy(gp_output_buffer, c, get_output_buffer_size());
    pUart->IER = IER_THRE | IER_RLS | IER_RBR;
    gp_ready = 0;
    while(!gp_ready) {
        release_processor();
    }
}
