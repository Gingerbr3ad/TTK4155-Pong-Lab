
#include "sam.h"
#include "drivers/can_driver.h"
#include <stdio.h>

void can_printmsg(CanMsg m){
    printf("CanMsg(id:%d, length:%d, data:{", m.id, m.length);
    if(m.length){
        printf("%d", m.byte[0]);
    }
    for(uint8_t i = 1; i < m.length; i++){
        printf(", %d", m.byte[i]);
    }
    printf("})\n");
}


#define txMailbox 0
#define rxMailbox 1


void can_init(CanInit init, uint8_t rxInterrupt) {
    // Enable clocks before accessing CAN0 and its GPIO configuration.
    PMC->PMC_PCER0 = 1u << ID_PIOA;
    PMC->PMC_PCER1 = 1u << (ID_CAN0 - 32);

    // Start disabled in normal mode (not autobaud/listen-only).
    CAN0->CAN_MR = 0;
    CAN0->CAN_IDR = 0xFFFFFFFFu;
    NVIC_DisableIRQ(CAN0_IRQn);
    NVIC_ClearPendingIRQ(CAN0_IRQn);
    (void)CAN0->CAN_SR;

    // PA0/PA1 connect to the shield's transceiver, not directly to CANH/L.
    PIOA->PIO_IDR = PIO_PA1A_CANRX0 | PIO_PA0A_CANTX0;
    PIOA->PIO_ABSR &= ~(PIO_PA1A_CANRX0 | PIO_PA0A_CANTX0);
    PIOA->PIO_PDR = PIO_PA1A_CANRX0 | PIO_PA0A_CANTX0;
    PIOA->PIO_PUER = PIO_PA1A_CANRX0 | PIO_PA0A_CANTX0;

    // CAN0 uses MCK (84 MHz). Write timing while the controller is disabled.
    CAN0->CAN_BR = init.reg;

    for (uint8_t i = 0; i < 8; i++) {
        CAN0->CAN_MB[i].CAN_MMR = CAN_MMR_MOT_MB_DISABLED;
    }

    // Transmit standard 11-bit frames.
    CAN0->CAN_MB[txMailbox].CAN_MID = 0;
    CAN0->CAN_MB[txMailbox].CAN_MMR = CAN_MMR_MOT_MB_TX;

    // Receive any standard ID; the application recognizes the test frame.
    CAN0->CAN_MB[rxMailbox].CAN_MAM = CAN_MAM_MIDE;
    CAN0->CAN_MB[rxMailbox].CAN_MID = 0; // MIDE=0: standard frame
    CAN0->CAN_MB[rxMailbox].CAN_MMR = CAN_MMR_MOT_MB_RX;
    CAN0->CAN_MB[rxMailbox].CAN_MCR = CAN_MCR_MTCR;

    if (rxInterrupt) {
        // Requires a CAN0_Handler. The exercise 6 test uses polling instead.
        CAN0->CAN_IER = 1u << rxMailbox;
        NVIC_EnableIRQ(CAN0_IRQn);
    }

    // Synchronization with an idle bus happens automatically after enabling.
    CAN0->CAN_MR = CAN_MR_CANEN;
}

void can_tx(CanMsg m) {
    if (m.id > 0x7FF || m.length > 8) {
        return;
    }
    while (!(CAN0->CAN_MB[txMailbox].CAN_MSR & CAN_MSR_MRDY)) {}

    // MIDE=0 selects the same standard 11-bit format as Node 1.
    CAN0->CAN_MB[txMailbox].CAN_MID = CAN_MID_MIDvA(m.id);
    CAN0->CAN_MB[txMailbox].CAN_MDL = m.dword[0];
    CAN0->CAN_MB[txMailbox].CAN_MDH = m.dword[1];
    CAN0->CAN_MB[txMailbox].CAN_MCR = CAN_MCR_MDLC(m.length) | CAN_MCR_MTCR;
}

uint8_t can_rx(CanMsg *m) {
    if (!m) {
        return 0;
    }
    uint32_t status = CAN0->CAN_MB[rxMailbox].CAN_MSR;
    if (!(status & CAN_MSR_MRDY)) {
        return 0;
    }

    uint32_t id = CAN0->CAN_MB[rxMailbox].CAN_MID;
    uint8_t length = (uint8_t)((status & CAN_MSR_MDLC_Msk) >> CAN_MSR_MDLC_Pos);
    // This API represents standard data frames only, not extended or RTR frames.
    if ((id & CAN_MID_MIDE) || (status & CAN_MSR_MRTR) || length > 8) {
        CAN0->CAN_MB[rxMailbox].CAN_MCR = CAN_MCR_MTCR;
        return 0;
    }

    // Preserve all 11 bits (0x123 must not be truncated to 0x23).
    m->id = (uint16_t)((id & CAN_MID_MIDvA_Msk) >> CAN_MID_MIDvA_Pos);
    m->length = length;
    m->dword[0] = CAN0->CAN_MB[rxMailbox].CAN_MDL;
    m->dword[1] = CAN0->CAN_MB[rxMailbox].CAN_MDH;

    // Release the mailbox only after copying all data.
    CAN0->CAN_MB[rxMailbox].CAN_MCR = CAN_MCR_MTCR;
    return 1;
}
