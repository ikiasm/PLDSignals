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
#define pin3TRIS    TRISBbits.TRISB13
#define pin3        LATBbits.LATB13


static int tickRpm = 0;
static int tickActual = 0;
static int tickFase = 0;
static int dutyPosAux = 0;
static int contaDientesCig = 0;
static int periodoRpm = 0;

static int analogValue = 0;
static int rpm = 100;
static int delay = 0;
static bool sincro = false;

static bool sincroFase = false;
static bool sincroRpm = false;
static int ticksVuelta = 0;
static int avanceFase = 0;
static int avanceRpm = 0;
static int ticksPorGrados = 0;
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
            pin3TRIS = 0;
            
            DRV_ADC_Open();
            DRV_ADC_Start();
            analogInit();
            DRV_TMR1_Start();
            
            
            if (appInitialized)
            {
            
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
//            ticksVuelta = ticksPorVuelta(rpm);
//            avanceFase = ticksVuelta - avanceEnTicks(55 , rpm);
//            avanceRpm = ticksVuelta - avanceEnTicks(65 , rpm);
//            if(tickActual >= avanceRpm)
//            {
//                sincroFase = true;
//            }
            crankshaftSignal(rpm);
            camshaftSignal(rpm);
//            crankshaftSignal(rpm);
//            camshaftSignal(rpm);
//            poner capacitor al potenciometro de rpm
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

void crankshaftSignal(int rpm)
{
    //para 1200 rpm el perido es de 50ms
    //cantidad de ticks para 1200 rpm = 1000
    //aprox 28 ticks por pulso
    int rpmAux  = rpm;
    static int dutyPos =  0;
    static int dutyNeg = 0;
    static int stage = 0;
    static int contaDientes = 0;
    static int vuelta = 1;
    
    ticksPorGrados = ((60000000) / ((rpmAux) * (10) * (360)));  
    periodoRpm = ((60000000) / (rpmAux * 36 * 10));   //periodo en ticks
    
    switch(stage)
    {
        case 0:
            tickRpm = 0;
            tickActual = 0;
            contaDientes = 0;
            contaDientesCig = 0;
            dutyPos =  ((periodoRpm * 20) / 100);
            dutyNeg = (periodoRpm - dutyPos);
            stage++;
            break;
        case 1:
            if((tickActual >= (ticksPorGrados * 45)) && contaDientes == 5)
                {
                    if((vuelta%2) == 0)
                    {
                        sincroFase = true;
                        pin3 = 1;
                    }
                    
                }
            if(tickRpm >= dutyNeg)
            {
                rpmSignal = 1;
                tickRpm = 0;
                stage++;
                if(contaDientes ==37)
                {
                    dutyPos = ((periodoRpm * 20) / 100);
                    dutyPosAux = dutyPos; //Ancho de pulso igual CMP = CKP
                    dutyNeg = (periodoRpm - dutyPos);
                    contaDientes = 0;
                    contaDientesCig = 0;
                    tickActual = 0;
                }
            }
            break;
        case 2:
            if(tickRpm >= dutyPos)
            {
                rpmSignal = 0;
                tickRpm = 0;
                contaDientes++;
                stage = 1;
                if((tickActual >= (ticksPorGrados * 45)) && (contaDientes == 8))
                {   //contaDientes == 8 unicamente para darle duracion al pin3
                    pin3 = 0;
                    vuelta++;
                }
                if(contaDientes == 36) 
                {
                    dutyPos = (dutyNeg / 3);
                    dutyNeg = (dutyNeg / 3);
                }
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
    static int periodoFase = 0;//(periodoRpm * 6);
    static int dutyPos =  0;
    static int dutyNeg = 0;
    periodoFase = (periodoRpm * 6);//((60000000) / (rpmAux * 12 * 10));   //periodo en ticks
    switch(stage)
    {
        case 0:
            tickFase = 0;
            contaDientes = 0;
            dutyPos = dutyPosAux;//dutyPos =  ((periodo * 20) / 100);
            dutyNeg = (periodoFase - dutyPos);
            faseSignal = 0;
            if(sincroFase)
            {
                faseSignal = 1;
                sincroFase = false;
                stage = 2;
            }
            break;
        case 1:
            if(tickFase >= dutyNeg)
            {
                faseSignal = 1;
                tickFase = 0;
                stage++;
                if(contaDientes == 12)
                {   //dutyNeg funciona unicamente cuando no hay sincronismo
                    //dutyNeg = (((periodoFase - dutyPos) * 2) / (3));
                    sincroFase = false;
                }
                if(contaDientes >=13)
                {
                    dutyPos = dutyPosAux;//dutyPos = ((periodo * 20) / 100);
                    dutyNeg = (periodoFase - dutyPos);
                    contaDientes = 0;
                    stage = 0;
                }
            }
            break;
        case 2:
            if(tickFase >= dutyPos)
            {
                faseSignal = 0;
                tickFase = 0;
                contaDientes++;
                stage = 1;
                if(contaDientes == 12) 
                {
                    dutyPos = dutyPosAux;
                    dutyNeg = (dutyNeg / 3);
                }
                if(contaDientes == 13)
                {
                    dutyPos = dutyPosAux;
                    dutyNeg = (periodoFase - dutyPos);
                    contaDientes = 0;
                    stage = 0;
                }
            }
            break;
        default:
            break;
    }
    
}

int avanceEnTicks(int grados, int rpm)
{
    int rev;
    int gradosAux;
    
    rev = 60000000 / rpm;
    gradosAux = ((rev) / (360 * 10));
    gradosAux = gradosAux * grados;
    return gradosAux;
}

int ticksPorVuelta(int rpm)
{
    int rpmAux = rpm;
    int tickAuxiliar;
    tickAuxiliar = 60000000 / (rpmAux * 10);
    return tickAuxiliar;
}
void tick10us()
{
    tickActual++;
    tickRpm++;
    tickFase++;
    
}

void analogicValue()
{
    analogValue = ADC1BUF0;
    //maximo valor de rpm = 4196
    //minimo valor de rpm = 100
    rpm = ((4 * analogValue) + 100);
}

void analogInit()
{
    AD1CON1bits.SSRC = 0x02;    //Hago coincidir el Timer 3 con el fin del sample de la se�al y empieza la conversion
    AD1CSSLbits.CSSL = 0x00;    //Todos los ANX scan estan disable
    AD1CON3bits.ADCS = 0x00;    //ADC conversion clock -> TAD = 2 * TPB
    AD1CON3bits.ADRC = 0x0;     //Uso el clock de perifericos
    AD1CON3bits.SAMC = 0x00;    //Auto-Sample time bits (lo dejo en cero aunque diga no esta permitido, asi estaba en el datasheet)
    AD1CON2bits.SMPI = 0x01;    //Interrupts at the completion of conversion for each 2nd sample/convert sequence
    
    AD1CON1bits.ADON = 1;       //ADC ON
    AD1CON1bits.ASAM = 1;       //Sampling begins immediately after last conversion completes; SAMP bit is automatically set
}

void delayDump()
{
    delay++;
}
/*******************************************************************************
 End of File
 */