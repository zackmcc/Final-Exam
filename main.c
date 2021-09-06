/*
 * Name: Zackary McClamma
 * Course: ECE 532
 * Assignment: Final Exam
 * Date: 11 DEC 2019
 * File: main.c
 *
 * */
#include <stdio.h>
#include <system.h>
#include "final.h"
#include "includes.h"

OS_EVENT *sem;


/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task1_stk[TASK_STACKSIZE];
OS_STK    task2_stk[TASK_STACKSIZE];
OS_STK	  task3_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define TASK1_PRIORITY      1
#define TASK2_PRIORITY      3
#define TASK3_PRIORITY		2

/* Toggles the Green LED every 500ms */
void task1(void* pdata)
{
	while(1){
	  INT8U err;
	  //OSMutexPend(tex,0,&err);
	  unsigned int* gled = GREEN_LED_BASE;

		  if(*gled == 0x01){
			  led_off(GREEN_LED_BASE);
		  }
		  else{
		  	led_on(GREEN_LED_BASE);
		  }
	  //OSMutexPost(tex);
	  OSTimeDlyHMSM(0, 0, 0, 500);
	}
}

/*Increment 7-Segment display*/
void task2(void* pdata)
{
	int* display = DISPLAY_BASE;
	*display = display_values[0];
	INT16U timeout = 0;
	INT8U err;
	while(1)
	{
		OSSemPend(sem, timeout, err);
		if (edge_capture !=0)
		{
			increment_display();
			edge_capture = 0;
		}
	}
}

/*Display Menu and facilitate user selections*/
void task3(void* pdata)
{
	  INT8U err;
	  i2c_init();
	  while (1)
	  {
		int start, stop, opt, addr, data, addrSet, dataSet = 0;
		unsigned int dma_length = 1024*1024;
		unsigned char read;
		//display menu
		printf("Options:\r\n");
		printf("1) Write EEPROM\r\n");
		printf("2) Read EEPROM\r\n");
		printf("3) Turn on Red LED\r\n");
		printf("4) Turn off Red LED\r\n");
		printf("5) DMA Test\r\n");
		printf("6) SDRAM Test\r\n");
		scanf("%d", &opt);
		fflush(stdin);
		printf("You chose option %d\r\n",opt);
		switch(opt){
		//Write EEPROM
		case 1:
			addrSet = 0;
			dataSet = 0;
			while(addrSet == 0)
			{
				printf("Enter Address: ");
				scanf("%d", &addr);
				fflush(stdin);
				printf(" %d\r\n", addr);

				if((addr < 0) || (addr > 127))
				{
					printf("Invalid input try again\r\n");
				}
				else
				{
					addrSet = 1;
				}
			}
			while(dataSet == 0)
			{
				printf("Enter Data: ");
				scanf("%d", &data);
				fflush(stdin);
				printf(" %d\r\n", data);

				if (data > 255)
				{
					printf("Invalid input try again\r\n");
				}
				else
				{
					dataSet = 1;
				}
			}
			eep_write(addr, data);
			OSTimeDlyHMSM(0, 0, 1, 0);
			break;
		//Read EEPROM
		case 2:
			addrSet = 0;
			while(addrSet == 0)
			{
				printf("Enter Address: ");
				scanf("%d", &addr);
				fflush(stdin);
				printf("\r\n");
				if((addr < 0) || (addr > 127))
				{
					printf("Invalid input try again\r\n");
				}
				else
				{
					addrSet = 1;
				}
			}
			read = eep_read(addr);
			printf("Value at address %d is %d\r\n", addr, (int)read);
			OSTimeDlyHMSM(0, 0, 1, 0);
			break;
		//Turn Red LED on
		case 3:
			led_on(RED_LED_BASE);
			OSTimeDlyHMSM(0, 0, 1, 0);
			break;
		//Turn Red LED off
		case 4:
			led_off(RED_LED_BASE);
			OSTimeDlyHMSM(0, 0, 1, 0);
			break;
		//DMA Test
		case 5:


			printf("Initializing 1MB block to 0x000A0000.\r\n");
			dma_init_block();

			printf("Copying 1MB block to 0x10100000...");
			start = OSTimeGet();
			dma_start(SDRAM_BASE, 0x10100000, dma_length);
			stop = OSTimeGet();
			printf("Process took %f seconds\r\n", ((float)stop-(float)start)/CLOCKS_PER_SEC);
			dma_check(0x10100000);

			printf("Copying 1MB block to 0x10200000...");
			start = OSTimeGet();
			dma_start(SDRAM_BASE, 0x10200000, dma_length);
			//dma_wait();
			stop = OSTimeGet();
			printf("Process took %f seconds\r\n", ((float)stop-(float)start)/100.0);
			dma_check(0x10200000);
			break;
		//SDRAM Test
		case 6:
			sdram_test_ones();
			sdram_test_zeros();
			sdram_test_increment();
			break;
		default:
			printf("Invalid option please try again.\r\n");
			break;

		}
	   // OSTimeDlyHMSM(0, 0, 1, 0);
		//OSMutexPost(tex);
		//OSTimeDlyHMSM(0, 0, 0, 500);
	  }
}
/* The main function creates two task and starts multi-tasking */
int main(void)
{

  INT16U initSem = 1;
  sem = OSSemCreate(initSem);
  int tst;
  init_button();
  tst = alt_ic_irq_enable(0, 5);

  //Check if Interrupt was enabled successfully
  if (tst<0) printf("\nFailed to enable interrupt, enable returned %d\r\n", tst);
  else printf("\n Interrupt Enabled.\r\n");
  //Initialize display to 0
  int* display = DISPLAY_BASE;
   *display = display_values[0];

  OSTaskCreateExt(task1,
                  NULL,
                  (void *)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);
              
               
  OSTaskCreateExt(task2,
                  NULL,
                  (void *)&task2_stk[TASK_STACKSIZE-1],
                  TASK2_PRIORITY,
                  TASK2_PRIORITY,
                  task2_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);


  OSTaskCreateExt(task3,
                  NULL,
                  (void *)&task3_stk[TASK_STACKSIZE-1],
                  TASK3_PRIORITY,
                  TASK3_PRIORITY,
                  task3_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);

  OSStart();
  return 0;
}

void led_on(int* base){
//	INT8U err;
	//OSMutexPend(tex,0,&err);
	*base = 0x01;
	//OSMutexPost(tex);
	return;
}

void led_off(int* base){
//	INT8U err;
	//OSMutexPend(tex,0,&err);
	*base = 0x00;
	return;
}

void i2c_init(void){
	i2c_reg *reg = I2C_0_BASE;

	reg->i2c_scl_low = 1000;
	reg->i2c_scl_high = 1000;
	reg->i2c_sda_hold = 500;
	reg->i2c_tfr_cmd_fifo_lvl = 8;
	reg->i2c_rx_data_fifo_lvl = 8;
}

void eep_write(unsigned short addr, unsigned char data){
	i2c_reg* reg = I2C_0_BASE;
//	INT8U err;
	unsigned char addr_low = addr >> 8;
	unsigned char addr_high = addr & 0x00FF;
	//OSMutexPend(tex,0,&err);
	reg->i2c_ctrl = 0x1;
	reg->i2c_tfr_cmd = 0x2A0;
	reg->i2c_tfr_cmd = addr_high;
	reg->i2c_tfr_cmd = addr_low;
	reg->i2c_tfr_cmd = data | 0x100;

	while(reg->i2c_status != 0);
	reg->i2c_ctrl = 0x0;
	//OSMutexPost(tex);
	OSTimeDlyHMSM(0, 0, 0, 5);
}

unsigned char eep_read(unsigned short addr){
	i2c_reg* reg = I2C_0_BASE;
	unsigned char addr_low = addr >> 8;
	unsigned char addr_high = addr & 0x00FF;
	unsigned char data;
	unsigned char tempData = 0;
	//OSMutexPend(tex,0,&err);

	reg->i2c_ctrl = 0x1;
	reg->i2c_tfr_cmd = 0x2A0;
	reg->i2c_tfr_cmd = addr_high;
	reg->i2c_tfr_cmd = addr_low;
	reg->i2c_tfr_cmd = 0x2A1;
	reg->i2c_tfr_cmd = tempData | 0x100;


	while(reg->i2c_status != 0);
	data = reg->i2c_rxdata;
	reg->i2c_ctrl = 0x0;
	//OSMutexPost(tex);
	return data;
}

int sdram_test_ones(void){
	unsigned int idx;
	unsigned int *sdram=SDRAM_BASE;
	unsigned int start, stop;

	printf("\r\nWriting ones to SDRAM...");
	start = OSTimeGet();
	for(idx = 0;idx<SDRAM_SIZE_WORDS;idx++)
	{
		sdram[idx] = 0xFFFFFFFF;
	}
	stop=OSTimeGet();
	printf("done.\r\n");
	printf("Process took %u seconds\r\n", (stop-start)/100);

	printf("Verifying ones...");
	start=OSTimeGet();
	for(idx=0;idx<SDRAM_SIZE_WORDS;idx++)
	{
		if(sdram[idx] != 0xFFFFFFFF)
		{
			stop=OSTimeGet();
			printf("MEMORY TEST FAILED!\r\n");
			printf("Process took %u seconds \r\n",(stop-start)/100);
			return 0;
		}
	}
	stop=OSTimeGet();
	printf("done.\r\n");
	printf("Process took %u seconds\r\n", (stop-start)/100);
	printf("Memory Test PASSED\r\n");
	return 0;
}

int sdram_test_zeros(void){
	unsigned int idx;
	unsigned int *sdram=SDRAM_BASE;
	unsigned int start, stop;

	printf("\r\nWriting zeros to SDRAM...");
	start = OSTimeGet();
	for(idx = 0;idx<SDRAM_SIZE_WORDS;idx++)
	{
		sdram[idx] = 0x00000000;
	}
	stop=OSTimeGet();
	printf("done.\r\n");
	printf("Process took %u seconds\r\n", (stop-start)/100);

	printf("Verifying zeros...");
	start=OSTimeGet();
	for(idx=0;idx<SDRAM_SIZE_WORDS;idx++)
	{
		if(sdram[idx] != 0x00000000)
		{
			stop=OSTimeGet();
			printf("MEMORY TEST FAILED!\r\n");
			printf("Process took %u seconds \r\n",(stop-start)/100);
			return 0;
		}
	}
	stop=OSTimeGet();
	printf("done.\r\n");
	printf("Process took %u seconds\r\n", (stop-start)/100);
	printf("Memory Test PASSED\r\n");
	return 0;
}

int sdram_test_increment(void)
{
	unsigned int idx;
	unsigned int *sdram=SDRAM_BASE;
	unsigned int start, stop;

	printf("\r\nWriting incrementing values to SDRAM...");
	start = OSTimeGet();
	for(idx = 0;idx<SDRAM_SIZE_WORDS;idx++)
	{
		sdram[idx] = idx;
	}
	stop=OSTimeGet();
	printf("done.\r\n");
	printf("Process took %u seconds\r\n", (stop-start)/100);

	printf("Verifying incremented values...");
	start=OSTimeGet();
	for(idx=0;idx<SDRAM_SIZE_WORDS;idx++)
	{
		if(sdram[idx] != idx)
		{
			stop=OSTimeGet();
			printf("MEMORY TEST FAILED!\r\n");
			printf("Process took %u seconds \r\n",(stop-start)/100);
			return 0;
		}
	}
	stop=OSTimeGet();
	printf("done.\r\n");
	printf("Process took %u seconds\r\n", (stop-start)/100);
	printf("Memory Test PASSED\r\n");
	return 0;
}

void button_isr(void* context, alt_u32 id){
	//Set context to edge_capture_ptr value (global value registered with ISR)
	volatile int* edge_capture_ptr = (volatile int*) context;
	//Read edge capture register of Button
	*edge_capture_ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_BASE);
	OSSemPost(sem);
	//Write 1 to edge capture register
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_BASE, 0x1);

	return;
}

void init_button()
{
	//Type cast edge_capture variable so it can be passed to register function
	void* edge_capture_ptr = (void*) &edge_capture;

	//Write 1 to IRQ Mask
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(BUTTON_BASE, 0x1);
	//Write 0 to edge capture of Button
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(BUTTON_BASE, 0x0);

	//Register Interrupt
	alt_ic_isr_register(0,
						5,
						(void*)button_isr,
						edge_capture_ptr,
						0x0);
}

void increment_display()
{
	int* display = DISPLAY_BASE;
	counter++;
	if (counter > 9)
	{
		counter = 0;
	}
	*display = display_values[counter];
}

void dma_start(unsigned int raddr, unsigned int waddr, unsigned int len)
{
	dma_reg *dmaPtr = DMA_BASE;
	dmaPtr->dma_read_addr = raddr;
	dmaPtr->dma_write_addr = waddr;
	dmaPtr->dma_length = 2^20;
	dma_wait();

}

void dma_wait(void)
{
	dma_reg *dmaPtr = DMA_BASE;
	dmaPtr->dma_control = 0x31;
	while((dmaPtr->dma_status >> 0) & 1);
	dmaPtr->dma_status &= 0x0;
	//dmaPtr->dma_control = 0x00000000;

}

void dma_check(unsigned int start_addr)
{
	unsigned int idx = 0;
	unsigned int *sdram=SDRAM_BASE;
	unsigned int stop, start;
	printf("Verifying DMA Copy...");
	start = OSTimeGet();
		for(idx=0;idx<((1024*1024)/4);idx++)
			{
				if(sdram[start_addr + idx] != 0x000A0000)
				{
					stop = OSTimeGet();
					printf("Process took %g seconds\r\n", ((float)stop-(float)start)/100.0);
					printf("DMA Copy FAILED!\r\n");
					return;
				}
			}
		stop = OSTimeGet();
		printf("done.\r\n");
		printf("Process took %g seconds\r\n", ((float)stop-(float)start)/100.0);
		printf("DMA Copy Successful.\r\n");
}

void dma_init_block()
{
	unsigned int idx;
	unsigned int *sdram=SDRAM_BASE;
	for(idx = 0;idx<((1024*1024)/4);idx++)
	{
		sdram[idx] = 0x000A0000;
	}
}


