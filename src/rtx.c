/** 
 * @brief:  rtx.c kernel API implementations, this is only a skeleton.
 * @author: Yiqing Huang
 * @date:   2014/01/12
 */

#include "uart_polling.h"
#include "printf.h"
/****************************************************************************
 ************************READ BEFORE YOU PROCEED FURTHER*********************
 ****************************************************************************
  To better strcture your code, you may want to split these functions
  into different files. For example, memory related kernel APIs in one file
  and process related API in another file.
*/

/* 
  This symbol is defined in the scatter file, 
  refer to RVCT Linker User Guide
*/  
extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;  

int k_release_processor(void)
{
  uart0_put_string("k_release_processor: entering\n\r");
	return 0;
}

