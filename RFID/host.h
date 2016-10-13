//***************************************************************
//------------<02.Dec.2010 by Peter Reiser>----------------------
//***************************************************************

#ifndef _HOST_H_
#define _HOST_H_

//================================================================

//#include <MSP430x23x0.h> 			// prozessor spezific header
//#include <stdio.h>					// standard input/output header
#include "mcu.h"
#include "trf797x.h"
#include "types.h"
#include "uart.h"

void HostCommands();

extern u08_t HostRequestCommand(u08_t *pbuf, u08_t length, u08_t broken_bits, u08_t no_crc);
//===============================================================

#endif
