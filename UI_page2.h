/*******************************************************************************
* This work is licensed under the Creative Commons Attribution-ShareAlike
* 4.0 International License. To view a copy of this license, visit
* http://creativecommons.org/licenses/by-sa/4.0/ or send a letter to
* Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
*******************************************************************************/

#ifndef __UI_PAGE2_H
#define __UI_PAGE2_H

#define	int8 signed char                // AKA int8_t
#define	uint8 unsigned char             // AKA uint8_t
#define	int16 signed int                // AKA int16_t
#define	uint16 unsigned int             // AKA uint16_t
#define	int32 signed long int           // AKA int32_t
#define	uint32 unsigned long int        // AKA uint32_t
#define	int64 signed long long int      // AKA int64_t
#define	uint64 unsigned long long int   // AKA uint64_t


//Defines to make code easier to read	



// Functions
void Page2_pointer1_update();
void Page2_pointer2_update();
void Refresh_page2();        // Refresh page 2
void Refresh_Parameter_Name();
void Save_Page2_Item();
void Refresh_Parameter_Value();
void Update_Parameter_Value();
void Display_10_Numbers(int32 num);
#endif