/*
**                     Copyright 1995,1996 by KVASER AB
**         P.O Box 4076 S-51104 KINNAHULT, SWEDEN. Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
#ifndef _IRQ_H_
#define _IRQ_H_

#define INTMASKREG1     0x21
#define INTMASKREG2     0xA1

#define INTEOIREG1      0x20
#define INTEOIREG2      0xA0
#define INTENDOFINT     0x20

#define IRQ0_VECTOR     0x08
#define IRQ1_VECTOR     0x09
#define IRQ2_VECTOR     0x0A
#define IRQ3_VECTOR     0x0B
#define IRQ4_VECTOR     0x0C
#define IRQ5_VECTOR     0x0D
#define IRQ6_VECTOR     0x0E
#define IRQ7_VECTOR     0x0F
#define IRQ8_VECTOR     0x70
#define IRQ9_VECTOR     0x71
#define IRQA_VECTOR     0x72
#define IRQB_VECTOR     0x73
#define IRQC_VECTOR     0x74
#define IRQD_VECTOR     0x75
#define IRQE_VECTOR     0x76
#define IRQF_VECTOR     0x77



typedef void (*InterruptRoutine)(void*);

PUBLIC int irq2intvect(int irq);
PUBLIC int AT_Or_Bigger(void);
PUBLIC canStatus deactivateIRQ(WORD irq);
PUBLIC canStatus activateIRQ(WORD irq, InterruptRoutine intrtn, void* arg);
PUBLIC int testIRQ(WORD irq);
PUBLIC void IRQExit(void);
PUBLIC void IRQInit(void);


#endif
