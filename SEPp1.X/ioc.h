/* 
 * File:   ioc.h
 * Authors: pedro, mariana, antonia
 *
 * Created on October 23, 2023
 */

#ifndef IOC_H
#define IOC_H

#include <stdint.h>
#include <p24FJ256GA702.h>
#include <libpic30.h>

void initializeInterruption();
void timerConfig();
void __attribute__((__interrupt__, auto_psv)) _IOCInterrupt();
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt();

#endif // IOC_H
