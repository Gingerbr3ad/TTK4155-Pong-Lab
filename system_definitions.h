/* 
This file is for defninitg variables used by the various AVR Libc macros.
Without it calues like F_CPU would need to be defined in each file using it separately,
this way we just need to include this file. 
!!!MAKE SURE THAT THIS IS THE 1ST FILE INCLUDED!!!
*/


#define F_CPU 1000000UL // I think this is correct? Dont reallt understand this well
#define BAUD 9600
// #define UBRR 31 // Calculated with an online baud rate caluculater for 9600 baudrate with 4.915 mHz oscilator REPLACED WITH THE SETBAUD MACRO