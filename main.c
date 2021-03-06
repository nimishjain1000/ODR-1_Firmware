/******************************************************************************
* This is the main for the ODR-1 Firmware Defined Radio
*
* This work is licensed under the Creative Commons Attribution-ShareAlike
* 4.0 International License. To view a copy of this license, visit
* http://creativecommons.org/licenses/by-sa/4.0/ or send a letter to
* Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
******************************************************************************/

#include "p33EP512MU810.h"
#include "DSPIC33E_hardware.h"
#include "main.h"
#include "devconfig.h"
#include "LCD_driver.h"
#include "LCD_fonts.h"
#include "UI_main.h"
#include "UI_page0.h"
#include "UI_page1.h"
#include "UI_page2.h"
#include "ODR1_Control_1.h"
#include "TLV320AIC3204.h"
#include "DSP.h"
#include "ODR1_Meters.h"

void Service_Interface(void);
// void Init_Mixer_Board();

uint8 timer1_flag = 0;     // Timer 1 flag used to say when to read encoders

int main(void)
{

    // Initialize processor and its peripherals
    Init_P33EP512MU810_osc();
    Init_P33EP512MU810_pins();
    Init_QEI_1();     // Initialize quadrature encoder interface #1
    Init_QEI_2();     // Initialize quadrature encoder interface #2
    Init_SPI1();      // Initialize SPI 1
    Init_SPI2();      // Initialize SPI 2
    Init_SPI3();      // Initialize SPI 3
    Init_SPI4();      // Initialize SPI 4
    Init_PWM();       // Initialize PWM Module
    Init_RTCC();      // Initialize the real time clock
    Init_I2C2();      // Initialize I2C2
    Init_REFCLK();    // Initialize Reference Clock output used as codec MCLK
    Init_ADC1();      // Initialize A-D Converter #1

    // Initialize other board hardware
    Lcd_Init();		     // tft initialization
    Init_Mixer_Board();      // Initialize the Mixer board

    AUX_OUT = 0;             // Rear Auxiliary Output
    PTT_OUT = 0;             // Rear PTT Output

    // Display and fill main screen
    Color_pallet_update();  // Set up the color pallet
    Refresh_page0();        // Refresh main screen
    
    Init_Audio_Codec();      // Initialize the audio Codec
    Init_DCI();

    //Configure Timer1 for interrupt operation
    Init_Timer1();

    while(1)  // main, loop forever
    {   
        Service_Interface();
        if (timer1_flag == 1)    // Its time to update the encoders
        {
            Encoder1_Update();   // Get the latest encoder status
            Encoder2_Update();   // Get the latest encoder status
            timer1_flag = 0;     // Reset the flag

        }

    }

}
/******************************************************************************/
/******************************************************************************
 * Function:       void __attribute__((interrupt,no_auto_psv)) _T1Interrupt( void )
 *
 * PreCondition:   None
 *
 * Input:          None
 *
 * Output:         None
 *
 * Side Effects:   None
 *
 * Overview:       ISR ROUTINE FOR THE TIMER1 INTERRUPT
 *****************************************************************************/
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;
    T1CONbits.TON = 0;

    PERIODIC = ~PERIODIC;
    //Encoder1_Update(); // Get the latest encoder status
    //Encoder2_Update(); // Get the latest encoder status
    timer1_flag = 1;     // Set the encoder flag
	
    TMR1 = 0;
    T1CONbits.TON = 1;

    /* reset Timer 1 interrupt flag */
}

/******************************************************************************
 * Function:       void Service_Interface(void)
 *
 * PreCondition:   None
 *
 * Input:          None
 *
 * Output:         None
 *
 * Side Effects:   None
 *
 * Overview:       Main UI thread to scan encoders, button presses, etc
 *****************************************************************************/
void Service_Interface(void)
{
    
    if (current_page == 0)
    {
        Page0_pointer1_update(); // Update upper main screen data
        Page0_pointer2_update(); // Update lower main screen data
        Scan_Key_Inputs();       // Scan the keyer input lines and react

    }
    else if (current_page == 1)
    {
        Page1_pointer1_update(); // Update upper main screen data
    }
    else if (current_page == 2)
    {
        Page2_pointer1_update(); // Update screen data
        if (!(page_pointer1 & 0x8000)) // Check for item focus
        {
            Page2_pointer2_update(); // Focus on item data
        }
    }

}