/*
**                     Copyright 1995,1996 by KVASER AB
**         P.O Box 4076 S-51104 KINNAHULT, SWEDEN. Tel. +46 320 15287
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/
/*
** This module handles the PIC (interrupt contoller) in the PC.
*/
#include <stdlib.h>
#include <dos.h>
#include <assert.h>

#include "std.h"
#include "canlib.h"
#include "irq.h"

#define INTERRUPTS 16

PRIVATE InterruptRoutine InterruptRtn[INTERRUPTS];
PRIVATE void* args[INTERRUPTS];

PRIVATE unsigned int irq2vect[INTERRUPTS] = {
   IRQ0_VECTOR, IRQ1_VECTOR, IRQ2_VECTOR, IRQ3_VECTOR,
   IRQ4_VECTOR, IRQ5_VECTOR, IRQ6_VECTOR, IRQ7_VECTOR,
   IRQ8_VECTOR, IRQ9_VECTOR, IRQA_VECTOR, IRQB_VECTOR,
   IRQC_VECTOR, IRQD_VECTOR, IRQE_VECTOR, IRQF_VECTOR,
};

#ifdef _MSC_VER
void __cdecl _dos_setvect(unsigned, void (__cdecl __interrupt __far *)());
#endif


#define INTERRUPT_RTN_LOW(x) \
   PRIVATE void far interrupt int##x (void) \
   { \
      WRITE_PORT_UCHAR(INTEOIREG1, INTENDOFINT); \
		InterruptRtn[x](args[x]); \
   }

#define INTERRUPT_RTN_HIGH(x) \
	PRIVATE void far interrupt int##x (void) \
	{ \
		WRITE_PORT_UCHAR(INTEOIREG2, INTENDOFINT); \
		WRITE_PORT_UCHAR(INTEOIREG1, INTENDOFINT); \
		InterruptRtn[x](args[x]); \
	}
INTERRUPT_RTN_LOW(0)
INTERRUPT_RTN_LOW(1)
INTERRUPT_RTN_LOW(2)
INTERRUPT_RTN_LOW(3)
INTERRUPT_RTN_LOW(4)
INTERRUPT_RTN_LOW(5)
INTERRUPT_RTN_LOW(6)
INTERRUPT_RTN_LOW(7)
INTERRUPT_RTN_HIGH(8)
INTERRUPT_RTN_HIGH(9)
INTERRUPT_RTN_HIGH(10)
INTERRUPT_RTN_HIGH(11)
INTERRUPT_RTN_HIGH(12)
INTERRUPT_RTN_HIGH(13)
INTERRUPT_RTN_HIGH(14)
INTERRUPT_RTN_HIGH(15)

PRIVATE void (far interrupt * ourVect[INTERRUPTS])(void) = {
	int0, int1, int2, int3,
	int4, int5, int6, int7,
	int8, int9, int10,int11,
	int12,int13,int14,int15,
};
PRIVATE void (far interrupt *oldVect[INTERRUPTS])(void);

PRIVATE int irqUsers[INTERRUPTS];

PUBLIC void IRQInit(void)
{
	WORD i;
	for (i=0; i<INTERRUPTS; i++) {
		InterruptRtn[i] = NULL;
		irqUsers[i] = 0;
		oldVect[i] = NULL;
		args[i] = NULL;
	}
}

/*
** Force deactivation of all interrupts we have used.
*/
PUBLIC void IRQExit(void)
{
	WORD i;
	for (i=0; i<INTERRUPTS; i++) {
		if (irqUsers[i] != 0) {
			irqUsers[i] = 1;
			(void)deactivateIRQ(i);
		}
	}
}

/*
PUBLIC int irq2intvect(int irq)
{
   switch (irq) {
      case 2: return (AT_Or_Bigger()?IRQ9_VECTOR:IRQ2_VECTOR);
      case 3: return IRQ3_VECTOR;
      case 5: return IRQ5_VECTOR;
      default:
         ;
   }
   assert(FALSE);
   return 0;
}

PUBLIC int AT_Or_Bigger(void)
{
    return (READ_PORT_UCHAR(INTEOIREG2) == 0);
}
*/

/*
** Check whether a certain IRQ is available or not.
** Returns TRUE if it is available.
*/
PUBLIC int testIRQ(WORD irq)
{
	BYTE mask1, mask2;

	if (irq>=INTERRUPTS) {
		return (int)canERR_PARAM;
	}

	mask1 = READ_PORT_UCHAR(INTMASKREG1);
	mask2 = READ_PORT_UCHAR(INTMASKREG2);

	if (irq==2) irq = 9;

	if (irq < 8) {
		return (mask1 & (1<<irq));
	} else {
		return (mask2 & (1<<(irq-8)));
	}
}


/*
** Activate an interrupt line, i.e. install the vector and set the
** registers in the interrupt controller.
** A reference count is kept so that the activation will take place at most
** once for every irq.
*/
PUBLIC canStatus activateIRQ(WORD irq, InterruptRoutine intrtn, void* arg)
{
	BYTE newMask1;
	BYTE newMask2;

	if (irq>=INTERRUPTS) {
		return canERR_PARAM;
	}

	if (irq==2) irq=9;

	if (irqUsers[irq]++ == 0) {

		DISABLE_INTERRUPTS;

		// Install interrupt vector functions
		oldVect[irq] = _dos_getvect(irq2vect[irq]);
		_dos_setvect(irq2vect[irq], ourVect[irq]);


		// Setup 8259 interrupt controllers
		newMask1 = READ_PORT_UCHAR(INTMASKREG1);
		newMask2 = READ_PORT_UCHAR(INTMASKREG2);

		//if ((irq >= 8) && AT_Or_Bigger()) {
		if (irq >= 8) {
			newMask2 &= ~(1U<<(irq-8));
			WRITE_PORT_UCHAR (INTMASKREG2, newMask2);
            // Enable IRQ2, just to be sure
			newMask1 &= ~0x04;
			WRITE_PORT_UCHAR (INTMASKREG1, newMask1);
		} else {
			newMask1 &= ~(1U<<irq);
			WRITE_PORT_UCHAR (INTMASKREG1, newMask1);
		}

		InterruptRtn[irq] = intrtn;
		args[irq] = arg;

		ENABLE_INTERRUPTS;
	}

	return canOK;
}


/*
** Deactivate the specified interrupt line.
** The reference count will be decreased and no action is taken unless
** it is zero.
*/
PUBLIC canStatus deactivateIRQ(WORD irq)
{
	BYTE newMask1;
	BYTE newMask2;

	if (irq==2) irq=9;

	//
	// Remove the interrupt vector and restore the old one if no CAN
	// circuit uses this interrupt anymore.
	//
	if (--irqUsers[irq] == 0) {
		DISABLE_INTERRUPTS;
		//
		// Install the old interrupt handler before restoring the
		// interrupt mask.
		//
		if (oldVect[irq] != NULL)
			_dos_setvect(irq2vect[irq], oldVect[irq]);

		// Restore 8259 interrupt mask.
		if (irq>=8){
			 newMask2  = READ_PORT_UCHAR(INTMASKREG2);
			 newMask2 |= (1<<irq);
			 WRITE_PORT_UCHAR (INTMASKREG2, newMask2);
		}

		// Note: we don't disable IRQ2, ever.
		if (irq < 8 && irq != 2) {
			 newMask1  = READ_PORT_UCHAR(INTMASKREG1);
			 newMask1 |= (1<<irq);
			 WRITE_PORT_UCHAR (INTMASKREG1, newMask1);
		}

		ENABLE_INTERRUPTS;
	}
	return canOK;
}


