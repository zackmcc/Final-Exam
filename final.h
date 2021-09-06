/*
 * Name: Zackary McClamma
 * Course: ECE 532
 * Assignment: Final Exam
 * Date: 11 DEC 2019
 * File: hw5.h
 *
 * */


#ifndef FINAL_H_
#define FINAL_H_

#include <sys/alt_irq.h>
#include <sys/alt_timestamp.h>
#include "altera_avalon_pio_regs.h"
#include <time.h>
#include "alt_types.h"
#include "sys/alt_irq.h"

#define I2C_BASE 0x80000
#define UART_BASE 0x60000
#define RED_LED_BASE 0x90000
#define GREEN_LED_BASE 0xA0000
#define SDRAM_BASE 0x10000000
#define SDRAM_SIZE_WORDS 32000000
#define DISPLAY_BASE 0x000D0000
#define BUTTON_BASE 0x000E0000
#define DMA_BASE 0x000c0000


typedef struct str_timer_regs{
	unsigned int stats;
	unsigned int control;
	unsigned int periodl;
	unsigned int periodh;
	unsigned int snapl;
	unsigned int snaph;
}timer_regs;

typedef volatile struct{
	unsigned int uart_rxdata;
	unsigned int uart_txdata;
	unsigned int uart_status;
	unsigned int uart_control;
	unsigned int uart_divisor;
	unsigned int uart_eop;
}uart_reg;

typedef volatile struct{
	unsigned int i2c_tfr_cmd;
	unsigned int i2c_rxdata;
	unsigned int i2c_ctrl;
	unsigned int i2c_iser;
	unsigned int i2c_isr;
	unsigned int i2c_status;
	unsigned int i2c_tfr_cmd_fifo_lvl;
	unsigned int i2c_rx_data_fifo_lvl;
	unsigned int i2c_scl_low;
	unsigned int i2c_scl_high;
	unsigned int i2c_sda_hold;
}i2c_reg;

typedef volatile struct{
	unsigned int data;
	unsigned int dir;
	unsigned int intmask;
	unsigned int edge;
	unsigned int outset;
	unsigned int outclear;
}gpio_regs;

typedef volatile struct{
	unsigned int dma_status;
	unsigned int dma_read_addr;
	unsigned int dma_write_addr;
	unsigned int dma_length;
	unsigned int dma_res1;
	unsigned int dma_res2;
	unsigned int dma_control;
	unsigned int dma_res3;
}dma_reg;

int display_values[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x00, 0x98};
volatile int edge_capture = 0;
volatile int counter = 0;


void i2c_init(void);
void eep_write(unsigned short addr, unsigned char data);
unsigned char eep_read(unsigned short addr);
void printMenu(void);

void led_on(int* base);
void led_off(int* base);

void timer_isr(void);
int sdram_test_ones(void);
int sdram_test_zeros(void);
int sdram_test_increment(void);

void dma_start(unsigned int raddr, unsigned int waddr, unsigned int len);
void dma_wait();
void dma_check(unsigned int start_addr);
void dma_init_block();

void increment_display();
void init_button();
void button_isr(void* context, alt_u32 id);
#endif /* FINAL_H_ */
