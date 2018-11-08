/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It 
    implements the logic of the application's state machine and it may call 
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "peripheral/ports/plib_ports.h"
#include "peripheral/adc/plib_adc.h"
#include "system/system.h"
#include "system/devcon/sys_devcon.h"
#include "framework/driver/adc/drv_adc_static.h"
#include "system/ports/sys_ports.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
#define rpmTRIS     TRISBbits.TRISB15
#define faseTRIS    TRISBbits.TRISB14
#define rpmSignal   LATBbits.LATB15
#define faseSignal  LATBbits.LATB14


static int tickRpm = 0;
static int tickActual = 0;
static int tickFase = 0;
static int contaDientesCig = 0;
// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.
    
    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    
    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;
            
            rpmTRIS = 0;
            faseTRIS = 0;
            DRV_ADC_Open();
            
            if (appInitialized)
            {
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            crankshaftSignal(100);
            camshaftSignal(100);
            analog_read();
            break;
        }

        /* TODO: implement your application state machine.*/
        

        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

void analog_read()
{
    static int estate=0;
    int result;
    switch(estate)
    {

        case 0: //DRV_ADC_Start();    //start sample
                        
            //        if(PLIB_ADC_ConversionHasCompleted(DRV_ADC_ID_1)==1)  //convertion completed
                        PLIB_ADC_SampleAutoStartEnable(ADC_ID_1);
           //                 result=PLIB_ADC_ResultGetByIndex(ADC_ID_1, 0);
                            
                            result=DRV_ADC_SamplesRead(0);   //buffer 0???
                            result=result;
                        
                estate++;
                break;
        case 1: 
                estate=0;
                break;
    }

}

void crankshaftSignal(int rpm)
{
    //para 1200 rpm el perido es de 50ms
    //cantidad de ticks para 1200 rpm = 1000
    //aprox 28 ticks por pulso
    int rpmAux = rpm;
    int ticksPorGrados;
    int grados;
    ticksPorGrados = 60000000 / (rpmAux * 10 * 360);
    //grados = ticks1Rpm / 360;
    static int periodo = 0;
    periodo = ((60000000) / (rpmAux * 36 * 10));   //periodo en ticks
    static int dutyPos =  0;
    static int dutyNeg = 0;
    static int stage = 0;
    static int contaDientes = 0;
    switch(stage)
    {
        case 0:
            tickRpm = 0;
            tickActual = 0;
            contaDientes = 0;
            contaDientesCig = 0;
            dutyPos =  ((periodo * 20) / 100);
            dutyNeg = (periodo - dutyPos);
            stage++;
            break;
        case 1:
            if(tickRpm >= dutyNeg)
            {
                rpmSignal = 1;
                tickRpm = 0;
                if(contaDientes >=37)
                {
                    dutyPos = ((periodo * 20) / 100);
                    dutyNeg = (periodo - dutyPos);
                    contaDientes = 0;
                    contaDientesCig = 0;
                    grados = tickActual / ticksPorGrados;
                    tickActual = 0;
                }
                stage++;
            }
            break;
        case 2:
            if(tickRpm >= dutyPos)
            {
                rpmSignal = 0;
                tickRpm = 0;
                contaDientes++;
                contaDientesCig++;
                if(contaDientes == 36) 
                {
                    dutyPos = (dutyNeg / 3);
                    dutyNeg = (dutyNeg / 3);
                }
                stage = 1;
            }
            break;
        default:
            break;
    }
}

void camshaftSignal(int rpm)
{
    int rpmAux = (rpm / 2);
    static int stage = 0;
    static int contaDientes = 0;
    static int periodo = 0;
    static int dutyPos =  0;
    static int dutyNeg = 0;
    periodo = ((60000000) / (rpmAux * 12 * 10));   //periodo en ticks
    switch(stage)
    {
        case 0:
            tickFase = 0;
            contaDientes = 0;
            dutyPos =  ((periodo * 20) / 100);
            dutyNeg = (periodo - dutyPos);
            //if(contaDientesCig >= 1)
            //{
               stage++; 
            //}
            
            break;
        case 1:
            if(tickFase >= dutyNeg)
            {
                faseSignal = 1;
                tickFase = 0;
                if(contaDientes >=13)
                {
                    dutyPos = ((periodo * 20) / 100);
                    dutyNeg = (periodo - dutyPos);
                    contaDientes = 0;
                }
                stage++;
            }
            break;
        case 2:
            if(tickFase >= dutyPos)
            {
                faseSignal = 0;
                tickFase = 0;
                contaDientes++;
                if(contaDientes == 12) 
                {
                    dutyPos = (dutyNeg / 3);
                    dutyNeg = (dutyNeg / 3);
                }
                stage = 1;
                //stage = 1;
            }
            break;
        default:
            break;
    }
    
}

void tick10us()
{
    tickActual++;
    tickRpm++;
    tickFase++;
    
}

/*******************************************************************************
 End of File
 */
