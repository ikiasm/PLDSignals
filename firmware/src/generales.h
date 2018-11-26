/* ************************************************************************** */
/** Descriptive File Name
  @Company
    Company Name
  @File Name
    filename.h
  @Summary
    Brief description of the file.
  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef GRALES_H
#define   GRALES_H

#ifdef   __cplusplus
extern "C" {
#endif

/*--------------------------DEFINES---------------------------*/
    
//SALIDAS
#define HIGH                 1
#define LOW                  0
#define HIGH_INV             0    
#define LOW_INV              1
#define INPUT                1
#define OUTPUT               0
//#define TRUE                 1
//#define FALSE                0
    
/*---------------------VARIABLES GLOBALES-----------------------*/


/*-------------------PROTOTIPO DE FUNCIONES---------------------*/
    
    
/*! Entero de 8 bits con signo */
typedef signed char Int8;
/*! Entero de 8 bits sin signo */
typedef unsigned char UInt8;
/*! Entero de 16 bits con signo */
typedef signed short Int16;
/*! Entero de 16 bits sin signo */
typedef unsigned short UInt16;
/*! Entero de 32 bits con signo */
typedef signed int Int32;
/*! Entero de 32 bits sin signo */
typedef unsigned int UInt32;
/*! Entero de 64 bits con signo */
typedef signed long int Int64;
/*! Entero de 64 bits sin signo */
typedef unsigned long int UInt64;
/*! Flotante */
typedef float Float;
/*! Flotante con mayor presicion */
typedef double Double;
/*! Boleano
   Solo puede tomar los valores TRUE o FALSE
 */
typedef Int8 Bool;
/*! Caracter */
typedef unsigned char Char;
/*! Para manejo de numeros reales */
typedef double Real;



#ifdef   __cplusplus
}
#endif

#endif   /* GRALES_H */