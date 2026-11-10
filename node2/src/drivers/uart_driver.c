#include <stdio.h>
#include <string.h>

#include "sam.h"

#include "drivers/uart_driver.h"

volatile uint8_t uart_recieved_flag = 0; // Index for the head of the buffer
static char uart_buffer[BufferSize];


typedef struct RingBuf RingBuf;
struct RingBuf {
    uint8_t buffer[1024];
    int capacity;
    
    int insertIdx;
    int removeIdx;
    int length;
};
RingBuf ringBuf = {0};


int push(RingBuf* rb, uint8_t val){
    if(rb->length >= (sizeof(rb->buffer)/sizeof(rb->buffer[0]))){
        return 0;
    }
    rb->buffer[rb->insertIdx] = val;
    rb->insertIdx = (rb->insertIdx + 1) % (sizeof(rb->buffer)/sizeof(rb->buffer[0]));
    rb->length++;
    return 1;
}

int pop(RingBuf* rb, uint8_t* val){    
    if(!rb->length){
        return 0;
    }
    *val = rb->buffer[rb->removeIdx];
    rb->removeIdx = (rb->removeIdx + 1) % (sizeof(rb->buffer)/sizeof(rb->buffer[0]));
    rb->length--;
    return 1;
}

void uart_init(){
    PMC->PMC_PCER0 |= (1 << ID_UART);
    
    // Set UART pins (A8, A9) to use alternate function (this disables regular IO)
    PIOA->PIO_PDR   |=   PIO_PA8 | PIO_PA9;
    // Set alternate function A (see tables 9-2, 34-2)
    PIOA->PIO_ABSR  &= ~(PIO_PA8 | PIO_PA9);
    
    // Configure UART settings
    UART->UART_CR   |= UART_CR_TXEN | UART_CR_RXEN;
    UART->UART_MR   |= UART_MR_PAR_NO;
    UART->UART_BRGR = SystemCoreClock / 16 / BAUD;
    
    
    // Configure interrupts on receive ready and errors
    UART->UART_IDR = 0xFFFFFFFF;
    UART->UART_IER = UART_IER_RXRDY | UART_IER_OVRE | UART_IER_FRAME | UART_IER_PARE;

    // Enable UART interrupt in the Nested Vectored Interrupt Controller (NVIC)
    NVIC_EnableIRQ((IRQn_Type) ID_UART);
    
}    

void uart_tx(uint8_t val){
    while(!(UART->UART_SR & UART_SR_TXEMPTY)){}
    UART->UART_THR = val;
}

uint8_t uart_rx(uint8_t* val){
    return pop(&ringBuf, val);
}    

// Changed to null terminate
int uart_flush(char* buf, int len) {
    int r = 0;
    uint8_t received;

    for (; r < len - 1; r++) {
        int ret = uart_rx(&received);

        if (!ret) {
            break;
        }
        else if (received == '\0') {
            break;
        }

        buf[r] = (char)received;
    }

    buf[r] = '\0';
    return r;
}


void UART_Handler(){
    uint32_t status = UART->UART_SR;
    
    // Errors: Reset UART
    if(status & (UART_SR_OVRE | UART_SR_FRAME | UART_SR_PARE)){
        UART->UART_CR = UART_CR_RXEN | UART_CR_TXEN | UART_CR_RSTSTA;
    }
    
    // Receive ready: push to ring buffer
    if (status & UART_SR_RXRDY) {
        uint8_t received = UART->UART_RHR & 0xFF;

        if (received == '\n') {
            push(&ringBuf, '\0');
            uart_recieved_flag = 1;
        } else {
            push(&ringBuf, received);
        }
    } 
}


uart_command_t handle_uart_interrupt() {
    uart_command_t command = UART_COMMAND_NONE;

    __disable_irq();
    uart_flush(uart_buffer, sizeof(uart_buffer)/sizeof(uart_buffer[0]));
    uart_recieved_flag = 0;
    __enable_irq();

    printf("##################### UART RECIEVED ####################\n");
    if (!strcmp(uart_buffer, "C_TEST")) {
        command =  UART_COMMAND_TEST;
    } else if(!strcmp(uart_buffer, "C_CAN_TEST")) {
        command = UART_COMMAND_CAN_TEST;
    } else if(!strcmp(uart_buffer, "C_SHOOT")) {
        command = UART_COMMAND_SHOOT;
    } else {
        printf("I've recieved this from UART (Command not recognized): %s \n", uart_buffer);
        command = UART_COMMAND_NONE;
    }

    return command;
}



// See https://interrupt.memfault.com/blog/boostrapping-libc-with-newlib

extern int _end;
#include <sys/stat.h>

void *_sbrk(int incr){
    static unsigned char *heap = NULL;
    unsigned char *prev_heap;

    if(heap == NULL){
        heap = (unsigned char*)&_end;
    }
    prev_heap = heap;

    heap += incr;

    return prev_heap;
}

int _close(int file){
    return -1;
}

int _fstat(int file, struct stat* st){
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file){
    return 1;
}

int _lseek(int file, int ptr, int dir){
    return 0;
}

__attribute__((naked)) void _exit(int status){
    //__asm("BKPT #0");
}

void _kill(int pid, int sig){
    return;
}

int _getpid(void){
    return -1;
}

int _write(int file, char* ptr, int len){
    if(file > 1){
        return -1;    
    }

    for(int idx = 0; idx < len; idx++){
        uart_tx((uint8_t)ptr[idx]);
    }
    return len;
}

int _read(int file, char* ptr, int len){
    if(file > 1){
        return -1;
    }
    
    int nread = 0;
    for(int idx = 0; idx < len; idx++){
        int b = uart_rx((uint8_t*)&ptr[idx]);
        nread += b;     
        if(!b){
            return nread;
        }        
    }
    return nread;
}


   