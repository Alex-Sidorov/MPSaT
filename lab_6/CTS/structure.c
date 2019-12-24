/* --COPYRIGHT--,BSD
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//******************************************************************************
// structure.c
//
// 5 elements configured as a 100 point slider.
//
//
//******************************************************************************

#include "structure.h"

//// Actual demo structures
//const struct Element element0 = {   //CB0, P6.0
//              .inputBits = CBIMSEL_0,
//              .maxResponse = 250,
//              .threshold = 125
//};
//
//const struct Element element1 = {   //CB1, P6.1
//              .inputBits = CBIMSEL_1,
//              .maxResponse = 390,
//              .threshold = 105
//};
//const struct Element element2 = {   //CB2, P6.2
//              .inputBits = CBIMSEL_2,
//              .maxResponse = 340,
//              .threshold = 170
//};
//const struct Element element3 = {   //CB3, P6.3
//              .inputBits = CBIMSEL_3,
//              .maxResponse = 500,
//              .threshold = 230
//};
//const struct Element element4 = {   //CB4, P6.4
//              .inputBits = CBIMSEL_4,
//              .maxResponse = 400,
//              .threshold = 200
//};
//
////*** CAP TOUCH HANDLER *******************************************************/
//// This defines the grouping of sensors, the method to measure change in
//// capacitance, and the function of the group
//
//const struct Sensor slider =
//               {
//                  .halDefinition = fRO_COMPB_TA1_SW,
//                  .numElements = 5,
//                  .baseOffset = 0,
//                  .cbpdBits = 0x001F, //BIT0+BIT1+BIT2+...BITE+BITF
//				  .points = 100,
//				  .sensorThreshold = 75,
//                  // Pointer to elements
//                  .arrayPtr[0] = &element0,  // point to first element
//                  .arrayPtr[1] = &element1,
//                  .arrayPtr[2] = &element2,
//                  .arrayPtr[3] = &element3,
//                  .arrayPtr[4] = &element4,
//
//                  .cboutTAxDirRegister = (unsigned char *)&P1DIR,  // PxDIR
//                  .cboutTAxSelRegister = (unsigned char *)&P1SEL,  // PxSEL
//                  .cboutTAxBits = BIT6, // P1.6
//                  // Timer Information
//                  //.measGateSource= GATE_WDT_ACLK,     //  0->SMCLK, 1-> ACLK
//                  .accumulationCycles = 50 //
//               };


// P1.0 input, P1.1 reference
const struct Element element0 = {
              .inputPxoutRegister = (uint8_t *)&P6OUT,
              .inputPxinRegister = (uint8_t *)&P6IN,
              .inputPxdirRegister = (uint8_t *)&P6DIR,
              .referencePxoutRegister= (uint8_t *)&P6OUT,
              .referencePxdirRegister = (uint8_t *)&P6DIR,
              .referenceBits = BIT1,
              .inputBits =  BIT0,
              .maxResponse = 250,
              .threshold = 80
};
// P1.1 input, P1.0 reference
const struct Element element1 = {
          .inputPxoutRegister = (uint8_t *)&P6OUT,
              .inputPxinRegister = (uint8_t *)&P6IN,
              .inputPxdirRegister = (uint8_t *)&P6DIR,
              .referencePxoutRegister= (uint8_t *)&P6OUT,
              .referencePxdirRegister = (uint8_t *)&P6DIR,
              .referenceBits = BIT2,
              .inputBits =  BIT1,
              .maxResponse = 250,
              .threshold = 80
};
// P1.2 input, P1.3 reference
const struct Element element2 = {
              .inputPxoutRegister = (uint8_t *)&P6OUT,
              .inputPxinRegister = (uint8_t *)&P6IN,
              .inputPxdirRegister = (uint8_t *)&P6DIR,
              .referencePxoutRegister= (uint8_t *)&P6OUT,
              .referencePxdirRegister = (uint8_t *)&P6DIR,
              .referenceBits = BIT3,
              .inputBits =  BIT2,
              .maxResponse = 250,
              .threshold = 80
};
// P1.3 input, P1.2 reference
const struct Element element3 = {
              .inputPxoutRegister = (uint8_t *)&P6OUT,
              .inputPxinRegister = (uint8_t *)&P6IN,
              .inputPxdirRegister = (uint8_t *)&P6DIR,
              .referencePxoutRegister= (uint8_t *)&P6OUT,
              .referencePxdirRegister = (uint8_t *)&P6DIR,
              .referenceBits = BIT4,
              .inputBits =  BIT3,
              .maxResponse = 250,
              .threshold = 80
};

const struct Element element4 = {
              .inputPxoutRegister = (uint8_t *)&P6OUT,
              .inputPxinRegister = (uint8_t *)&P6IN,
              .inputPxdirRegister = (uint8_t *)&P6DIR,
              .referencePxoutRegister= (uint8_t *)&P6OUT,
              .referencePxdirRegister = (uint8_t *)&P6DIR,
              .referenceBits = BIT5,
              .inputBits =  BIT4,
              .maxResponse = 250,
              .threshold = 80
};


//*** Sensor *******************************************************/
// This defines the grouping of sensors, the method to measure change in
// capacitance, and the function of the group

const struct Sensor slider =
{
    .halDefinition = RC_PAIR_TA0,
    .numElements = 1,
    //.numElements = 5,
    .baseOffset = 0,
    .points = 80,
    .sensorThreshold = 75,
    // Pointer to elements
    .arrayPtr[0] = &element0,  // point to first element
    /*.arrayPtr[1] = &element1,
    .arrayPtr[2] = &element2,
    .arrayPtr[3] = &element3,
    .arrayPtr[4] = &element4,*/
    // Timer Information
    //.measGateSource= TIMER_TxCLK,   // TxCLK
    .measGateSource= TIMER_ACLK,    // ACLK
    //.measGateSource= TIMER_SMCLK,     // SMCLK
    //.sourceScale = TIMER_SOURCE_DIV_0,
    .sourceScale = TIMER_SOURCE_DIV_1,
    //.sourceScale = TIMER_SOURCE_DIV_2,
    //.sourceScale = TIMER_SOURCE_DIV_3,
    .accumulationCycles = 10
};
