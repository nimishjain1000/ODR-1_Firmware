/***************************************************************************
* This code is used for ODR-1 control functions
*
* This work is licensed under the Creative Commons Attribution-ShareAlike
* 4.0 International License. To view a copy of this license, visit
* http://creativecommons.org/licenses/by-sa/4.0/ or send a letter to
* Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
****************************************************************************/

#include "p33EP512MU810.h"
#include "DSPIC33E_hardware.h"
#include "ODR1_Control_1.h"
#include "si5351a.h"
#include "TLV320AIC3204.h"
#include "UI_page0.h"
#include "LCD_driver.h"

/******************************************************************************
 * Function:       Init_Mixer_Board
 *
 * PreCondition:   None
 *
 * Input:          None
 *
 * Output:         None
 *
 * Side Effects:   None
 *
 * Overview:       Initialize the Oscillator and Mixer Board
 *****************************************************************************/
void Init_Mixer_Board()
{
    BPF_S0  = 0;    // Band Pass Filter Select 0
    BPF_S1  = 0;    // Band Pass Filter Select 1
    BPF_CS0 = 0;    // Band Pass Filter Chip Select 0
    BPF_CS1 = 0;    // Band Pass Filter Chip Select 1
    ATT_S0  = 0;    // RF Attenuator Select 0
    ATT_S1  = 0;    // RF Attenuator Select 1
    TX_RX   = 0;    // RX/TX Control line, 0=Receive
    AUX_OUT = 0;    // Rear Auxiliary Output
    PTT_OUT = 0;    // Rear PTT Output

    EN_12V  = 1;    // 12V Power Enable
    delayms(250);   // Delay for 250mS for supply rails

    // Default to 6M Bandpass filter
    BPF_S0  = 0;
    BPF_S1  = 1;
    BPF_CS0 = 1;
    BPF_CS1 = 0;

    // Default osillator frequency to 6M (2 * 10MHz)
    //si5351aSetFrequency(20000000); // set si5351 freq to 20MHz
    Init_si5351a() ;      // Initialize si5351 clock chip
}

/******************************************************************************
 * Function:       Set_bandpass_Filters
 *
 * PreCondition:   None
 *
 * Input:          None
 *
 * Output:         None
 *
 * Side Effects:   None
 *
 * Overview:       Select the bandpass filter based on frequency
 *****************************************************************************/
void Set_bandpass_Filters(int32 freq)
{
    if (freq < 2690001)                             // < 2.69MHz
    {
        BPF_S0  = 1;
        BPF_S1  = 1;
        BPF_CS0 = 0;
        BPF_CS1 = 1;
    }
    else if ((freq > 2690000)&&(freq < 4800001))   // 2.69-4.80MHz
    {
        BPF_S0  = 1;
        BPF_S1  = 1;
        BPF_CS0 = 1;
        BPF_CS1 = 0;
    }
    else if ((freq > 4800000)&&(freq < 7650001))   // 4.80-7.65MHz
    {
        BPF_S0  = 0;
        BPF_S1  = 1;
        BPF_CS0 = 0;
        BPF_CS1 = 1;
    }
    else if ((freq > 7650000)&&(freq < 11000001))  // 7.65-11.0MHz
    {
        BPF_S0  = 0;
        BPF_S1  = 1;
        BPF_CS0 = 1;
        BPF_CS1 = 0;
    }
    else if ((freq > 11000000)&&(freq < 16100001)) // 11.0-16.1MHz
    {
        BPF_S0  = 1;
        BPF_S1  = 0;
        BPF_CS0 = 0;
        BPF_CS1 = 1;
    }
    else if ((freq > 16100000)&&(freq < 22800001)) // 16.1-22.8MHz
    {
        BPF_S0  = 1;
        BPF_S1  = 0;
        BPF_CS0 = 1;
        BPF_CS1 = 0;
    }
    else if ((freq > 22800000)&&(freq < 31600001)) // 22.8-31.6MHz
    {
        BPF_S0  = 0;
        BPF_S1  = 0;
        BPF_CS0 = 0;
        BPF_CS1 = 1;
    }
    else                                           // > 31.6Mhz
    {
        BPF_S0  = 0;
        BPF_S1  = 0;
        BPF_CS0 = 1;
        BPF_CS1 = 0;
    }
}

/******************************************************************************
 * Function:       Set_RFGain
 *
 * PreCondition:   None
 *
 * Input:          int16 with value of 0 to 3
 *
 * Output:         None
 *
 * Side Effects:   None
 *
 * Overview:       Set RF gain on mixer board
 *****************************************************************************/
void Set_RFGain(int16 gain)
{
    switch(gain) // Action based on gain selected
    {
        case 0:  // Select -24dB RF Gain
            ATT_S0  = 0;    // RF Attenuator Select 0
            ATT_S1  = 1;    // RF Attenuator Select 1
        break;
        case 1:  // Select -6dB RF Gain
            ATT_S0  = 1;    // RF Attenuator Select 0
            ATT_S1  = 0;    // RF Attenuator Select 1
        break;
        case 2:  // Select 0dB RF Gain
            ATT_S0  = 0;    // RF Attenuator Select 0
            ATT_S1  = 0;    // RF Attenuator Select 1
        break;
        case 3:  // Select +15dB RF Gain
            ATT_S0  = 1;    // RF Attenuator Select 0
            ATT_S1  = 1;    // RF Attenuator Select 1
        break;
        default: // Default 0dB RF Gain
            ATT_S0  = 0;    // RF Attenuator Select 0
            ATT_S1  = 0;    // RF Attenuator Select 1
    }
}

/******************************************************************************
 * Function:       Radio_Receive
 *
 * PreCondition:   None
 *
 * Input:          int16 with value of 0 to 3
 *
 * Output:         None
 *
 * Side Effects:   None
 *
 * Overview:       Set radio to receive
 *****************************************************************************/
void Radio_Receive()
{
    Codec_HP_Gain(-7);    // Mute headphones
    TX_RX   = 0;    // RX/TX Control line, 0=Receive
    PTT_OUT = 0;    // Rear PTT Output

    switch(rxtx_mode) // Action based on rxtx mode radio is in
    {
        case 0:  // Mode 0 CW
            Codec_Config_PT1_RX();      // Configure CODEC for receive
        break;
        case 1:  // Mode 1 USB
            Codec_Config_PT1_RX();      // Configure CODEC for receive
        break;
        case 2:  // Mode 2 LSB
            Codec_Config_PT1_RX();      // Configure CODEC for receive
        break;
        case 3:  // Mode 3 AM
            Codec_Config_PT1_RX();      // Configure CODEC for receive
        break;
        case 4:  // Mode 4 Analog Pass Through
            Codec_Config_PT1_RX();      // Configure CODEC for receive
        break;
        case 5:  // Mode 5 ADC-DAC Pass Through
            Codec_Config_PT2_RX();      // Configure CODEC for receive
        break;
        case 6:  // Mode 6 ADC-DSP-DAC Pass Through
            Codec_Config_PT3_RX();      // Configure CODEC for receive
        break;
    }
    Codec_HP_Gain(af_gain);    // Restore headphones gain 
}

/******************************************************************************
 * Function:       Radio_Transmit
 *
 * PreCondition:   None
 *
 * Input:          int16 with value of 0 to 3
 *
 * Output:         None
 *
 * Side Effects:   None
 *
 * Overview:       Set radio to transmit
 *****************************************************************************/
void Radio_Transmit()
{
    Codec_HP_Gain(-7);    // Mute headphones

    switch(rxtx_mode) // Action based on rxtx mode radio is in
    {
        case 0:  // Mode 0 CW
            Codec_Config_PT1_TX();      // Configure CODEC for transmit
        break;
        case 1:  // Mode 1 USB
            Codec_Config_TX_Mic();      // Configure CODEC for transmit
        break;
        case 2:  // Mode 2 LSB
            Codec_Config_TX_Mic();      // Configure CODEC for transmit
        break;
        case 3:  // Mode 3 AM
            Codec_Config_TX_Mic();      // Configure CODEC for transmit
        break;
        case 4:  // Mode 4 Analog Pass Through
            Codec_Config_PT1_TX();      // Configure CODEC for transmit
        break;
        case 5:  // Mode 5 ADC-DAC Pass Through
            Codec_Config_PT1_TX();      // Configure CODEC for transmit
        break;
        case 6:  // Mode 6 ADC-DSP-DAC Pass Through
            Codec_Config_PT1_TX();      // Configure CODEC for transmit
        break;
    }
    Codec_HP_Gain(af_gain);    // Restore headphones gain
    TX_RX   = 1;    // RX/TX Control line, 0=Receive
    PTT_OUT = 1;    // Rear PTT Output
}