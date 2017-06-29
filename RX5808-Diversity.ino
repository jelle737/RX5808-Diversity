/*
 * SPI driver based on fs_skyrf_58g-main.c Written by Simon Chambers
 * TVOUT by Myles Metzel
 * Scanner by Johan Hermen
 * Inital 2 Button version by Peter (pete1990)
 * Refactored and GUI reworked by Marko Hoepken
 * Universal version my Marko Hoepken
 * Diversity Receiver Mode and GUI improvements by Shea Ivey
 * OLED Version by Shea Ivey
 * Seperating display concerns by Shea Ivey

The MIT License (MIT)

Copyright (c) 2015 Marko Hoepken

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <avr/pgmspace.h>
#include <EEPROM.h>

#include "settings.h"

// uncomment depending on the display you are using.
// this is an issue with the arduino preprocessor
#ifdef TVOUT_SCREENS
//    #include <TVout.h>
//    #include <fontALL.h>
#endif
#ifdef OLED_128x64_ADAFRUIT_SCREENS

	#ifdef SH1106
		#include <Adafruit_SH1106.h>
	#else
		#include <Adafruit_SSD1306.h>
	#endif
    #include <Adafruit_GFX.h>
    #include <Wire.h>
    #include <SPI.h>
#endif
#ifdef OLED_128x64_U8G_SCREENS
//    #include <U8glib.h>
#endif

#include "screens.h"
screens drawScreen;

// Channels to sent to the SPI registers
const uint16_t channelTable[] PROGMEM = {
  // Channel 1 - 8
  0x2A05,    0x299B,    0x2991,    0x2987,    0x291D,    0x2913,    0x2909,    0x289F,    // Band A
  0x2903,    0x290C,    0x2916,    0x291F,    0x2989,    0x2992,    0x299C,    0x2A05,    // Band B
  0x2895,    0x288B,    0x2881,    0x2817,    0x2A0F,    0x2A19,    0x2A83,    0x2A8D,    // Band E
  0x2906,    0x2910,    0x291A,    0x2984,    0x298E,    0x2998,    0x2A02,    0x2A0C,    // Band F / Airwave
  0x281D,    0x2890,    0x2902,    0x2915,    0x2987,    0x299A,    0x2A0C,    0x2A1F,     // Band C / Immersion Raceband
  0x2609, 0x261C, 0x268E, 0x2701, 0x2713, 0x2786, 0x2798, 0x280B //Low band
};//           2890                  2915                  299A                  2A1F
//  0x281D,    0x288F,    0x2902,    0x2914,    0x2987,    0x2999,    0x2A0C,    0x2A1E     // Band C / Immersion Raceband
/*
 * low band
 * 0x2609, 0x261C, 0x268E, 0x2701, 0x2713, 0x2786, 0x2798, 0x280B 
 * 5362, 5399, 5436, 5473, 5510, 5547, 5584, 5621
 * 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8
 * 40, 41, 42, 43, 44, 45, 46, 47, 19, 18, 32, 17, 33, 16, 7,  34, 8,  24, 6,  9,  25, 5,  35, 10, 26, 4,  11, 27, 3, 36, 12, 28, 2,  13, 29, 37, 1,  14, 30, 0, 15, 31, 38, 20, 21, 39, 22, 23

 */


// Channels with their Mhz Values


const uint16_t channelFreqTable[] PROGMEM = {
  // Channel 1 - 8
  5865, 5845, 5825, 5805, 5785, 5765, 5745, 5725, // Band A
  5733, 5752, 5771, 5790, 5809, 5828, 5847, 5866, // Band B
  5705, 5685, 5665, 5645, 5885, 5905, 5925, 5945, // Band E
  5740, 5760, 5780, 5800, 5820, 5840, 5860, 5880, // Band F / Airwave
  5658, 5695, 5732, 5769, 5806, 5843, 5880, 5917,  // Band C / Immersion Raceband
  5362, 5399, 5436, 5473, 5510, 5547, 5584, 5621  // Band L
};

// calculate the frequency to bit bang payload
/*uint16_t freq_to_reg(uint16_t f) {
    uint16_t tf, N, A;
    tf = (f - 479) / 2;
    N = tf / 32;
    A = tf % 32;
    return (N<<7) + A;
}*/

// do coding as simple hex value to save memory.
/*const uint8_t channelNames[] PROGMEM = {
  0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, // Band A
  0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, // Band B
  0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, // Band E
  0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, // Band F / Airwave
  0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8,  // Band C / Immersion Raceband
  0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8   // Band L
};*/

// All Channels of the above List ordered by Mhz
const uint8_t channelList[] PROGMEM = {
//  19, 32, 18, 17, 33, 16, 7, 34, 8, 24, 6, 9, 25, 5, 35, 10, 26, 4, 11, 27, 3, 36, 12, 28, 2, 13, 29, 37, 1, 14, 30, 0, 15, 31, 38, 20, 21, 39, 22, 23
  40, 41, 42, 43, 44, 45, 46, 47, 19, 32, 18, 17, 33, 16, 7, 34, 8, 24, 6, 9, 25, 5, 35, 10, 26, 4, 11, 27, 3, 36, 12, 28, 2, 13, 29, 37, 1, 14, 30, 0, 15, 31, 38, 20, 21, 39, 22, 23
};

bool channelFavs[48];
uint8_t numberOfFavs = 0;

char channel = 0;
uint8_t channelIndex = 0;
//uint8_t rssi = 0;
//uint8_t rssi_scaled = 0;
uint8_t active_receiver = useReceiverA;
#ifdef USE_DIVERSITY
    uint8_t diversity_mode = useReceiverAuto;
    char diversity_check_count = 0; // used to decide when to change antennas.
#endif
uint8_t rssi_seek_threshold = RSSI_SEEK_TRESHOLD;
//uint8_t hight = 0;
uint8_t state = START_STATE;
uint8_t state_last_used=START_STATE;
//uint8_t writePos = 0;
//uint8_t switch_count = 0;
uint8_t man_channel = 0;
uint8_t last_channel_index = 0;
uint8_t force_seek=0;
uint8_t seek_direction=1;
unsigned long time_of_tune = 0;        // will store last time when tuner was changed
unsigned long time_screen_saver = 0;
unsigned long time_next_payload = 0;
uint8_t last_active_channel=0;
uint8_t seek_found=0;
uint8_t last_dip_channel=255;
uint8_t last_dip_band=255;
uint8_t scan_start=0;
//uint8_t first_tune=1;
uint8_t in_menu_time_out=0;
uint16_t rssi_best=0; // used for band scaner
uint16_t rssi_min_a=RSSI_MIN_VAL;
uint16_t rssi_max_a=RSSI_MAX_VAL;
uint16_t rssi_setup_min_a=RSSI_MIN_VAL;
uint16_t rssi_setup_max_a=RSSI_MAX_VAL;
#ifdef USE_DIVERSITY
    uint16_t rssi_min_b=RSSI_MIN_VAL;
    uint16_t rssi_max_b=RSSI_MAX_VAL;
    uint16_t rssi_setup_min_b=RSSI_MIN_VAL;
    uint16_t rssi_setup_max_b=RSSI_MAX_VAL;
#endif
uint8_t rssi_setup_run=0;
//jelle add for les calls to readRSSI
int rssi=0;
int rssiA=0;
int rssiB=0;

bool settings_beeps = true;
bool settings_orderby_channel = true;

// SETUP ----------------------------------------------------------------------------
void setup()
{
//    Serial.begin(9600);
//    Serial.println("Diversity");
    // IO INIT
    // initialize digital pin 13 LED as an output.
    pinMode(led, OUTPUT); // status pin for TV mode errors
    digitalWrite(led, HIGH);
    // buzzer
    pinMode(buzzer, OUTPUT); // Feedback buzzer (active buzzer, not passive piezo)
    digitalWrite(buzzer, HIGH);
    // minimum control pins
    pinMode(buttonUp, INPUT);
    digitalWrite(buttonUp, INPUT_PULLUP);
    pinMode(buttonMode, INPUT);
    digitalWrite(buttonMode, INPUT_PULLUP);
    // optional control
    pinMode(buttonDown, INPUT);
    digitalWrite(buttonDown, INPUT_PULLUP);
    pinMode(buttonSave, INPUT);
    digitalWrite(buttonSave, INPUT_PULLUP);
    //Receiver Setup
    pinMode(receiverA_led,OUTPUT);
//#ifdef USE_DIVERSITY
    pinMode(receiverB_led,OUTPUT);
//#endif
    setReceiver(useReceiverA);
    // SPI pins for RX control
    pinMode (slaveSelectPin, OUTPUT);
    pinMode (spiDataPin, OUTPUT);
    pinMode (spiClockPin, OUTPUT);

    // use values only of EEprom is not 255 = unsaved
    uint8_t eeprom_check = EEPROM.read(EEPROM_ADR_STATE);
    if(eeprom_check == 255) // unused
    {
        // save 8 bit
        EEPROM.write(EEPROM_ADR_STATE,START_STATE);
        EEPROM.write(EEPROM_ADR_TUNE,CHANNEL_MIN_INDEX);
        EEPROM.write(EEPROM_ADR_BEEP,settings_beeps);
        EEPROM.write(EEPROM_ADR_ORDERBY,settings_orderby_channel);
        // save 16 bit
        EEPROM.write(EEPROM_ADR_RSSI_MIN_A_L,lowByte(RSSI_MIN_VAL));
        EEPROM.write(EEPROM_ADR_RSSI_MIN_A_H,highByte(RSSI_MIN_VAL));
        // save 16 bit
        EEPROM.write(EEPROM_ADR_RSSI_MAX_A_L,lowByte(RSSI_MAX_VAL));
        EEPROM.write(EEPROM_ADR_RSSI_MAX_A_H,highByte(RSSI_MAX_VAL));

        for(int i=0; i<6;i++){
            EEPROM.write(EEPROM_ADR_FAVS+i,B00000000);
        }

        EEPROM.write(EEPROM_ADR_DIVERSITY,diversity_mode);
        // save 16 bit
        EEPROM.write(EEPROM_ADR_RSSI_MIN_B_L,lowByte(RSSI_MIN_VAL));
        EEPROM.write(EEPROM_ADR_RSSI_MIN_B_H,highByte(RSSI_MIN_VAL));
        // save 16 bit
        EEPROM.write(EEPROM_ADR_RSSI_MAX_B_L,lowByte(RSSI_MAX_VAL));
        EEPROM.write(EEPROM_ADR_RSSI_MAX_B_H,highByte(RSSI_MAX_VAL));
    }

    // read last setting from eeprom
    state=EEPROM.read(EEPROM_ADR_STATE);
    channelIndex=EEPROM.read(EEPROM_ADR_TUNE);
    // set the channel as soon as we can
    // faster boot up times :)
    setChannelModule(channelIndex);
    channel = channel_from_index(channelIndex);
    last_channel_index=channelIndex;

    settings_beeps=EEPROM.read(EEPROM_ADR_BEEP);
    settings_orderby_channel=EEPROM.read(EEPROM_ADR_ORDERBY);

    for(int j=0; j<6; j++){
        uint8_t FavsY = EEPROM.read(EEPROM_ADR_FAVS+j);
        for(int i=0; i<8; i++){
            channelFavs[j*8+i] = ((FavsY & B10000000) == B10000000);
            if((FavsY & B10000000) == B10000000){
                numberOfFavs++;
            }
            FavsY = FavsY << 1;
        }
    }

    rssi_min_a=((EEPROM.read(EEPROM_ADR_RSSI_MIN_A_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MIN_A_L)));
    rssi_max_a=((EEPROM.read(EEPROM_ADR_RSSI_MAX_A_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MAX_A_L)));
//#ifdef USE_DIVERSITY
    diversity_mode = EEPROM.read(EEPROM_ADR_DIVERSITY);
    rssi_min_b=((EEPROM.read(EEPROM_ADR_RSSI_MIN_B_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MIN_B_L)));
    rssi_max_b=((EEPROM.read(EEPROM_ADR_RSSI_MAX_B_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MAX_B_L)));
//#endif
    in_menu_time_out=0;

    // Init Display
    if (drawScreen.begin() > 0) {
        // on Error flicker LED
        while (true) { // stay in ERROR for ever
            digitalWrite(led, !digitalRead(led));
            delay(100);
        }
    }

    digitalWrite(led, LOW);

}

// LOOP ----------------------------------------------------------------------------
void loop(){
    /******JELLE OK*****/
    /*   MAIN MENU    */
    /*******************/
    uint8_t in_menu;

  //  if(state = STATE_MAIN_MENU){

        
  //  }



    if (digitalRead(buttonMode) == LOW){ // key pressed ? Jelle: (LOW=PRESSED)
        time_screen_saver=0; // Jelle: =?
        //Jelle: 2 beeps is gemakkelijk, weet je dat je de knop hebt ingedrukt
        beep(50); // beep & debounce
        delay(KEY_DEBOUNCE/2); // debounce
        beep(50); // beep & debounce
        delay(KEY_DEBOUNCE/2); // debounce
        uint8_t press_time=0; //10presses = save
        // on entry wait for release
        while(digitalRead(buttonMode) == LOW && press_time < 30){
            delay(10);
            press_time++;
        }

       // Show Mode Screen
        if(state==STATE_SEEK_FOUND)
        {
            state=STATE_SEEK;
        }

        char menu_id=state_last_used;
        in_menu_time_out=50; // 20x 100ms = 5 seconds
        do{
            if(press_time == 30) // if menu held for 1 second invoke quick save.
            {
                // user held the mode button and wants to quick save.
                delay(100);
                in_menu_time_out=0; // EXIT
                state = STATE_QUICK_MENU;
                break;
            }
            drawScreen.modeMenu(menu_id);
            delay(KEY_DEBOUNCE);
            while(--in_menu_time_out && (digitalRead(buttonMode) == HIGH) && (digitalRead(buttonUp) == HIGH) && (digitalRead(buttonDown) == HIGH)){
                delay(100); // timeout delay // wait for next key press or time out                
            }
            if(in_menu_time_out==0 || digitalRead(buttonMode) == LOW){
                if(in_menu_time_out==0){
                    state = state_last_used;
                    beep(50);
                    delay(KEY_DEBOUNCE/2);
                }else{ 
                    /*switch(menu_id){ //Should be state=menu_id in final version
                        case 0:
                            state=STATE_MANUAL;
                            break;
                        case 1:
                            state=STATE_FAVORITES;
                            break;
                        case 2:
                            state=STATE_SEEK;
                            break;
                        case 3:
                            state=STATE_SCAN;
                            break;
                        case 4:
                            state=STATE_ANTENNA;
                            break;
                        case 5:
                            state=STATE_DIVERSITY;
                            break;
                        case 6:
                            state=STATE_SETUP_MENU;
                            break;
                        case 7:
                            state=STATE_SETUP_MENU;
                            break;
                    }*/
                    state=menu_id;
                    beep(50);
                    delay(KEY_DEBOUNCE/2);
                    in_menu_time_out=0;
                }
            }else{
                if(digitalRead(buttonUp) == LOW) {
                	menu_id = (menu_id+5-1)%5;
                }else if(digitalRead(buttonDown) == LOW) {
                	menu_id = (menu_id+1)%5;
                }
                in_menu_time_out=50;
			}
            beep(50); // beep & debounce
        }while(in_menu_time_out);
        delay(KEY_DEBOUNCE/2);
    }
    
    /*******************JELLE OK*************/
    /*   Draw screen if mode has changed or timed out   */
    /***************************************/
    if(in_menu_time_out==0){
//        beep(300); //temp to see if it works
        switch(state){
            case STATE_FAVORITES:
                if(numberOfFavs==0){
                    state=STATE_MANUAL;
                }
                if(channelFavs[channelIndex]==false){
                    for(int i=0; i<48; i++){
                        channelIndex=(channelIndex+1)%48;
                        if(channelFavs[channelIndex]==true){
                            break;
                        }
                    }
                }    
            case STATE_MANUAL:
            case STATE_ANTENNA:
                time_screen_saver=millis();
                drawScreen.seekMode(state);
                if(state_last_used == STATE_SCAN) {
                    channelIndex=EEPROM.read(EEPROM_ADR_TUNE);
                    channel = channel_from_index(channelIndex);
                }
                state_last_used=state;
                break;  
            case STATE_SEEK:
                time_screen_saver=0; 
                rssi_seek_threshold = RSSI_SEEK_TRESHOLD;
                rssi_best=0;
                force_seek=1;
                drawScreen.seekMode(state);
                state_last_used=state;
                break;
            case STATE_SCAN:
                channel=CHANNEL_MIN;
                channelIndex = pgm_read_byte_near(channelList + channel);
                rssi_best=0;
                scan_start=1;
                drawScreen.bandScanMode(state);
                state_last_used=state;
                break;
        }
        in_menu_time_out=-1;
    }
    /*************************JELLE OK**********************/
    /*   Processing depending of state   **   SCREEN SAVER   */
    /*********************************************************/
    if(state == STATE_SCREEN_SAVER) {
        drawScreen.screenSaver(diversity_mode, channelIndex, pgm_read_word_near(channelFreqTable + channelIndex));
        do{
            readRSSI();
            drawScreen.updateScreenSaver(active_receiver, rssiA, rssiB);
        }
        while((digitalRead(buttonMode) == HIGH) && (digitalRead(buttonUp) == HIGH) && (digitalRead(buttonDown) == HIGH)); // wait for next button press
        in_menu_time_out=0;
        state=state_last_used;
        time_screen_saver=0;
        return;
    }



    /****************************************************/
    /*   Personal addapted screen for antenna compare   */
    /****************************************************/
    if(state == STATE_ANTENNA_SAVER) {
        drawScreen.screenAntenna(diversity_mode, channelIndex, pgm_read_word_near(channelFreqTable + channelIndex));
        do{
            readRSSI();
            drawScreen.updateScreenAntenna(active_receiver, rssiA, rssiB);
            drawScreen.updateScreenSaver(active_receiver, rssiA, rssiB);
        }
        while((digitalRead(buttonMode) == HIGH) && (digitalRead(buttonUp) == HIGH) && (digitalRead(buttonDown) == HIGH)); // wait for next button press
        in_menu_time_out=0;
        state=state_last_used;
        time_screen_saver=0;
        return;
    }

    /***************JELLE OK*********/
    /*   Diversity selection menu   */
    /********************************/
    if(state == STATE_DIVERSITY) {
        // simple menu
        char diversity_mode_previous=diversity_mode;
		    in_menu_time_out=50;
        do{
            drawScreen.diversity(diversity_mode);
            readRSSI();
            drawScreen.updateDiversity(active_receiver, rssiA, rssiB);
            delay(KEY_DEBOUNCE);
            while(--in_menu_time_out && (digitalRead(buttonMode) == HIGH) && (digitalRead(buttonUp) == HIGH) && (digitalRead(buttonDown) == HIGH)){
                readRSSI();
                drawScreen.updateDiversity(active_receiver, rssiA, rssiB);
            }
            if(in_menu_time_out==0 || digitalRead(buttonMode) == LOW){
                if(in_menu_time_out==0){
                    diversity_mode = diversity_mode_previous;
                }
                EEPROM.write(EEPROM_ADR_DIVERSITY,diversity_mode);
                beep(50);
                delay(KEY_DEBOUNCE/2);
                in_menu_time_out=0;
            }else{
                if(digitalRead(buttonUp) == LOW) {
                	diversity_mode = (diversity_mode+2)%3;
                }else if(digitalRead(buttonDown) == LOW) {
                        diversity_mode = (diversity_mode+1)%3;
                }
            in_menu_time_out=50;
            }
            beep(50); // beep & debounce
        }while(in_menu_time_out);
        delay(KEY_DEBOUNCE/2);
        state=state_last_used;
    }

    /*****************************************/
    /*   Processing MANUAL MODE / SEEK MODE / ANTENNA MODE / FAVORITES MODE */
    /*****************************************/
    if(state == STATE_MANUAL || state == STATE_SEEK || state==STATE_ANTENNA || state==STATE_FAVORITES)
    {
        // read rssi
        wait_rssi_ready();
        readRSSI();
//        rssi_best = (rssi > rssi_best) ? rssi : rssi_best;
//channel -> order by Ghz freq
//channelIndex -> Order by name F1 channel
        drawScreen.updateSeekMode(state, channelIndex, channel, rssi, pgm_read_word_near(channelFreqTable + channelIndex), rssi_seek_threshold, seek_found, channelFavs);
    //    channel=channel_from_index(channelIndex); // get 0...48 index depending of current channel
        if(state == STATE_MANUAL || state == STATE_ANTENNA) // MANUAL MODE
        {
            // handling of keys
            if( digitalRead(buttonUp) == LOW){        // channel UP
                if(settings_orderby_channel){ //order by channel
                    uint8_t press_time=0; //3presses = save
                    // on entry wait for release
                    while(digitalRead(buttonUp) == LOW && press_time < 30){
                        delay(10);
                        press_time++;
                    }
                    if(press_time==30){
                        channelIndex=(channelIndex/8*8+8)%48;
                        channel = channel_from_index(channelIndex);
                    }else{
                        channelIndex=(channelIndex+1)%48;
                        channel = channel_from_index(channelIndex);
                    }

                }else{ //order by frequency
                    channel=(channel+1)%48;
                    channelIndex = pgm_read_byte_near(channelList + channel);
                }
            beep(50); // beep & debounce
            delay(KEY_DEBOUNCE); // debounce
            time_screen_saver=millis();
            }else if( digitalRead(buttonDown) == LOW){ // channel DOWN
                if(settings_orderby_channel){ //order by channel
                    uint8_t press_time=0; //3presses = save
                    // on entry wait for release
                    while(digitalRead(buttonDown) == LOW && press_time < 30){
                        delay(10);
                        press_time++;
                    }
                    if(press_time==30){
                        channelIndex=(channelIndex/8*8+47)%48;
                        channel = channel_from_index(channelIndex);
                    }else{
                        channelIndex=(channelIndex+48-1)%48;
                        channel = channel_from_index(channelIndex);
                    }
                }else{ //order by frequency
                    channel=(channel+48-1)%48;
                    channelIndex = pgm_read_byte_near(channelList + channel);
                }
            beep(50); // beep & debounce
            delay(KEY_DEBOUNCE); // debounce
            time_screen_saver=millis();
            }
        }

        if(state == STATE_FAVORITES){
            if( digitalRead(buttonUp) == LOW){        // channel UP
                for(int i=0; i<48; i++){
                    channelIndex=(channelIndex+1)%48;
                    if(channelFavs[channelIndex]==true){
                        break;
                    }
                }
                channel = channel_from_index(channelIndex);
            beep(50); // beep & debounce
            delay(KEY_DEBOUNCE); // debounce
            time_screen_saver=millis();
            }else if( digitalRead(buttonDown) == LOW){ // channel DOWN
                for(int i=0; i<48; i++){
                    channelIndex=(channelIndex+47)%48;
                    if(channelFavs[channelIndex]==true){
                        break;
                    }
                }
                channel = channel_from_index(channelIndex);
            beep(50); // beep & debounce
            delay(KEY_DEBOUNCE); // debounce
            time_screen_saver=millis();
            }
        }

        // handling for seek mode after screen and RSSI has been fully processed
        if(state == STATE_SEEK) //
        { // SEEK MODE

            // recalculate rssi_seek_threshold
            ((int)((float)rssi_best * (float)(RSSI_SEEK_TRESHOLD/100.0)) > rssi_seek_threshold) ? (rssi_seek_threshold = (int)((float)rssi_best * (float)(RSSI_SEEK_TRESHOLD/100.0))) : false;

            if(!seek_found){ // search if not found
                if ((!force_seek) && (rssi > rssi_seek_threshold)){ // check for found channel
                    seek_found=1;
                    time_screen_saver=millis();
                    // beep twice as notice of lock
                    beep(50);
                    delay(KEY_DEBOUNCE/2);
                    beep(50);
                }else{ // seeking itself
                    force_seek=0;
                    // next channel
                    channel+=seek_direction;
                    if (channel > CHANNEL_MAX){
                        // calculate next pass new seek threshold
                        rssi_seek_threshold = (int)((float)rssi_best * (float)(RSSI_SEEK_TRESHOLD/100.0));
                        channel=CHANNEL_MIN;
                        rssi_best = 0;
                    }else if(channel < CHANNEL_MIN){
                        // calculate next pass new seek threshold
                        rssi_seek_threshold = (int)((float)rssi_best * (float)(RSSI_SEEK_TRESHOLD/100.0));
                        channel=CHANNEL_MAX;
                        rssi_best = 0;
                    }
                    rssi_seek_threshold = rssi_seek_threshold < 5 ? 5 : rssi_seek_threshold; // make sure we are not stopping on everyting
                    channelIndex = pgm_read_byte_near(channelList + channel);
                }
            }else{ // seek was successful
            }
            if (digitalRead(buttonUp) == LOW || digitalRead(buttonDown) == LOW){ // restart seek if key pressed
                if(digitalRead(buttonUp) == LOW) {
                    seek_direction = 1;
                }else{
                    seek_direction = -1;
                }
                beep(50); // beep & debounce
                delay(KEY_DEBOUNCE); // debounce
                force_seek=1;
                seek_found=0;
                time_screen_saver=0;
            }
        }
        // change to screensaver after lock and 5 seconds has passed.
        if(time_screen_saver != 0 && ((time_screen_saver+5000 < millis() && rssi > 50) || (time_screen_saver + (SCREENSAVER_TIMEOUT*1000) < millis() ))){
            state = (state == STATE_ANTENNA)? STATE_ANTENNA_SAVER:STATE_SCREEN_SAVER;
        }
    }
    /****************************/
    /*   Processing SCAN MODE   */
    /****************************/
    else if (state == STATE_SCAN || state == STATE_RSSI_SETUP)
    {
        // force tune on new scan start to get right RSSI value
        if(scan_start)
        {
            scan_start=0;
            setChannelModule(channelIndex);
            last_channel_index=channelIndex;
        }

        // print bar for spectrum
        wait_rssi_ready();
        // value must be ready
        readRSSI();

        if(state == STATE_SCAN)
        {
            if (rssi > RSSI_SEEK_TRESHOLD)
            {
                if(rssi_best < rssi) {
                    rssi_best = rssi;
                }
            }
        }

        uint8_t bestChannelName = channelIndex;
        uint16_t bestChannelFrequency = pgm_read_word_near(channelFreqTable + channelIndex);

        drawScreen.updateBandScanMode((state == STATE_RSSI_SETUP), channel, rssi, bestChannelName, bestChannelFrequency, rssi_setup_min_a, rssi_setup_max_a);

        // next channel
        if (channel < CHANNEL_MAX)
        {
            channel++;
        }
        else
        {
            channel=CHANNEL_MIN;
            if(state == STATE_RSSI_SETUP)
            {
                if(!rssi_setup_run--)
                {
                    // setup done
                    rssi_min_a=rssi_setup_min_a;
                    rssi_max_a=rssi_setup_max_a;
                    if(rssi_max_a < 125) { // user probably did not turn on the VTX during calibration
                        rssi_max_a = RSSI_MAX_VAL;
                    }
                    rssi_min_b=rssi_setup_min_b;
                    rssi_max_b=rssi_setup_max_b;
                    if(rssi_max_b < 125) { // user probably did not turn on the VTX during calibration
                        rssi_max_b = RSSI_MAX_VAL;
                    }

                    state=STATE_RSSI_MENU;
                    beep(1000);
                }
            }
        }
        // new scan possible by press scan
        if (digitalRead(buttonUp) == LOW) // force new full new scan
        {
            beep(50); // beep & debounce
            delay(KEY_DEBOUNCE); // debounce
            channel=CHANNEL_MIN;
            scan_start=1;
            rssi_best=0;
        }
        // update index after channel change
        channelIndex = pgm_read_byte_near(channelList + channel);
    }

    /******************JELLE OK**/
    /*   Processing SETUP MENU  */
    /****************************/
    if(state == STATE_SETUP_MENU){
        // simple menu
        char menu_id=0;
        in_menu_time_out=50;
        int editing = -1;
        do{
            //drawScreen.mainMenu(menu_id, s0, s0size, 5);
            drawScreen.setupMenu(menu_id, settings_beeps, settings_orderby_channel);

             
            //drawScreen.setupMenu(menu_id, settings_beeps, settings_orderby_channel, editing);
            delay(KEY_DEBOUNCE);
            while(--in_menu_time_out && (digitalRead(buttonMode) == HIGH) && (digitalRead(buttonUp) == HIGH) && (digitalRead(buttonDown) == HIGH)){
                delay(100); // timeout delay // wait for next key press or time out                
            }
            if(in_menu_time_out==0 || digitalRead(buttonMode) == LOW){
                if(in_menu_time_out==0){
                    state = state_last_used;
                    beep(50);
                    delay(KEY_DEBOUNCE/2);
                }else{ 
                    switch(menu_id){
                        case 0: // Channel Order Channel/Frequency
                            settings_orderby_channel = !settings_orderby_channel;
                            in_menu_time_out=50;
                            EEPROM.write(EEPROM_ADR_ORDERBY,settings_orderby_channel);
                            break;
                        case 1:// Beeps enable/disable
                            settings_beeps = !settings_beeps;
                            EEPROM.write(EEPROM_ADR_BEEP,settings_beeps);
                            in_menu_time_out=50;
                            break;
                        case 2:// settings RSSI
                            in_menu_time_out = 0;
                            state=STATE_RSSI_MENU;
                            beep(50);
				            delay(KEY_DEBOUNCE/2);
                            break;
                        case 3://  Calibrate RSSI
                            in_menu_time_out = 0;
                            state=STATE_RSSI_SETUP;
                            //literal paste
                            
                            rssi_min_a=50;
                            rssi_max_a=300; // set to max range
                            rssi_setup_min_a=RSSI_MAX_VAL;
                            rssi_setup_max_a=RSSI_MIN_VAL;
                            rssi_min_b=50;
                            rssi_max_b=300; // set to max range
                            rssi_setup_min_b=RSSI_MAX_VAL;
                            rssi_setup_max_b=RSSI_MIN_VAL;
                            rssi_setup_run=RSSI_SETUP_RUN;
                            channel=CHANNEL_MIN;
                            channelIndex = pgm_read_byte_near(channelList + channel);
                            rssi_best=0;
                            scan_start=1;
                            drawScreen.bandScanMode(state);
                            beep(50);
                            delay(KEY_DEBOUNCE/2);
                            break;
                        case 4:
                            in_menu_time_out=0;
                            state = state_last_used;
                            break;
                    }
                }
            }else{
                if(digitalRead(buttonUp) == LOW) {
                	   menu_id = (menu_id+4)%5;
               }else if(digitalRead(buttonDown) == LOW) {
                        menu_id = (menu_id+1)%5;
                }
			    in_menu_time_out=50;
            }
            beep(50); // beep & debounce
        }while(in_menu_time_out);
        delay(KEY_DEBOUNCE/2);
    }

    /******************JELLE OK**/
    /*   Processing QUICK  */
    /****************************/
    if(state == STATE_QUICK_MENU){
        // simple menu
        char menu_id=0;
        in_menu_time_out=50;
//        int editing = -1;
        do{
            drawScreen.quickMenu(menu_id);
            delay(KEY_DEBOUNCE);
            while(--in_menu_time_out && (digitalRead(buttonMode) == HIGH) && (digitalRead(buttonUp) == HIGH) && (digitalRead(buttonDown) == HIGH)){
                delay(100); // timeout delay // wait for next key press or time out                
            }
            if(in_menu_time_out==0 || digitalRead(buttonMode) == LOW){
                if(in_menu_time_out==0){
                    state = state_last_used;
                    beep(50);
                    delay(KEY_DEBOUNCE/2);
                }else{
                    uint8_t FavsZ = B00000000;
                    switch(menu_id){
                        case 0: // Fastbood here
                            EEPROM.write(EEPROM_ADR_TUNE,channelIndex);
                            EEPROM.write(EEPROM_ADR_STATE,state_last_used);
                            state=state_last_used;
                            in_menu_time_out=0;
                            break;
                        case 1:// add to favorite
                            channelFavs[channelIndex]=true;
                            numberOfFavs++;
                        case 2://  remove from favorite
                            if(menu_id==2){
                                channelFavs[channelIndex]=false;
                                numberOfFavs--;
                                if(EEPROM.read(EEPROM_ADR_TUNE)==channelIndex&&EEPROM.read(EEPROM_ADR_STATE)==STATE_FAVORITES){
                                    EEPROM.write(EEPROM_ADR_STATE,STATE_MANUAL);
                                }
                            }
                            for(int i=channelIndex/8*8; i<channelIndex/8*8+8;i++){
                                FavsZ = FavsZ<<1;
                                FavsZ += channelFavs[i];
                            }
                            EEPROM.write(EEPROM_ADR_FAVS+channelIndex/8,FavsZ);
                            state=state_last_used;
                            in_menu_time_out=0;
                            break;
                        case 3:// diversity menu
                            state=STATE_DIVERSITY;
                            in_menu_time_out=0;
                            break;
                        case 4://settings menu
                            state=STATE_SETUP_MENU;
                            in_menu_time_out=0;
                            break;
                    }
                    beep(50);
                    delay(KEY_DEBOUNCE/2);
                }
            }else{
                if(digitalRead(buttonUp) == LOW) {
                	   menu_id = (menu_id+4)%5;
                }else if(digitalRead(buttonDown) == LOW) {
                        menu_id = (menu_id+1)%5;
                }
			    in_menu_time_out=50;
            }
            beep(50); // beep & debounce
        }while(in_menu_time_out);
        delay(KEY_DEBOUNCE/2);
    }




    /****************JELLE OK****/
    /*   Processing SETUP RSSI  */
    /****************************/
    if(state == STATE_RSSI_MENU){
        char menu_id=0;
        in_menu_time_out=50;
        bool editing = false;
        do{
            drawScreen.rssiMenu(menu_id, rssi_min_a, rssi_max_a, rssi_min_b, rssi_max_b, editing);

            
            
            //drawScreen.mainMenu(menu_id, o0, o0size, 5);
            //bool tempolar = false;
            //drawScreen.overlayRSSI(menu_id, o0size,5, rssi_min_a, rssi_max_a, rssi_min_b, rssi_max_b, tempolar);
            delay(KEY_DEBOUNCE);
            while(--in_menu_time_out && (digitalRead(buttonMode) == HIGH) && (digitalRead(buttonUp) == HIGH) && (digitalRead(buttonDown) == HIGH)){
                delay(100); // timeout delay
            }
            if(in_menu_time_out==0 || digitalRead(buttonMode) == LOW){
                if(in_menu_time_out==0){
                    state = state_last_used;
                    beep(50);
                    delay(KEY_DEBOUNCE/2);
                }else{
                    if(editing){
                        editing=false;
                    }else if(menu_id<4){
                        editing=true;
                    }else if(menu_id==4){
                        in_menu_time_out=0;
                        rssi_min_a=((EEPROM.read(EEPROM_ADR_RSSI_MIN_A_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MIN_A_L)));
                        rssi_max_a=((EEPROM.read(EEPROM_ADR_RSSI_MAX_A_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MAX_A_L)));
                        rssi_min_b=((EEPROM.read(EEPROM_ADR_RSSI_MIN_B_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MIN_B_L)));
                        rssi_max_b=((EEPROM.read(EEPROM_ADR_RSSI_MAX_B_H)<<8) | (EEPROM.read(EEPROM_ADR_RSSI_MAX_B_L)));

                        state=STATE_SETUP_MENU;
                        beep(50);
                        delay(KEY_DEBOUNCE/2);
                    }else{
                        in_menu_time_out=0;
                        // save 16 bit
                        EEPROM.write(EEPROM_ADR_RSSI_MIN_A_L,(rssi_min_a & 0xff));
                        EEPROM.write(EEPROM_ADR_RSSI_MIN_A_H,(rssi_min_a >> 8));
                        // save 16 bit
                        EEPROM.write(EEPROM_ADR_RSSI_MAX_A_L,(rssi_max_a & 0xff));
                        EEPROM.write(EEPROM_ADR_RSSI_MAX_A_H,(rssi_max_a >> 8));
                        // save 16 bit
                        EEPROM.write(EEPROM_ADR_RSSI_MIN_B_L,(rssi_min_b & 0xff));
                        EEPROM.write(EEPROM_ADR_RSSI_MIN_B_H,(rssi_min_b >> 8));
                        // save 16 bit
                        EEPROM.write(EEPROM_ADR_RSSI_MAX_B_L,(rssi_max_b & 0xff));
                        EEPROM.write(EEPROM_ADR_RSSI_MAX_B_H,(rssi_max_b >> 8));
                        state=STATE_SETUP_MENU;
                        beep(50);
                        delay(KEY_DEBOUNCE/2);
                    }
                }
            }else{
                if(digitalRead(buttonUp) == LOW) {
                    if(editing){
                        switch(menu_id){
                            case 0:
                                rssi_min_a++;
                                break;
                            case 1:
                                rssi_max_a++;
                                break;
                            case 2:
                                rssi_min_b++;
                                break;
                            case 3:
                                rssi_max_b++;
                                break;
                        }
                    }else{
                        menu_id = (menu_id+5)%6;
                    }
                }else if(digitalRead(buttonDown) == LOW) {
                    if(editing){
                        switch(menu_id){
                            case 0:
                                rssi_min_a--;
                                break;
                            case 1:
                                rssi_max_a--;
                                break;
                            case 2:
                                rssi_min_b--;
                                break;
                            case 3:
                                rssi_max_b--;
                                break;
                        }
                    }else{
                        menu_id = (menu_id+1)%6;
                    }
                }
				in_menu_time_out=50;
			}
            beep(50); // beep & debounce
        }while(in_menu_time_out);
        delay(KEY_DEBOUNCE/2);
    }









    /*****************************/
    /*   General house keeping   */
    /*****************************/
    if(last_channel_index != channelIndex)         // tune channel on demand
    {
        setChannelModule(channelIndex);
        last_channel_index=channelIndex;
        // keep time of tune to make sure that RSSI is stable when required
        time_of_tune=millis();
    }
}

/*******************/
/*   SUB ROUTINES  */
/*******************/

void beep(uint16_t time)
{
    digitalWrite(led, HIGH);
    if(settings_beeps){
        digitalWrite(buzzer, LOW); // activate beep
    }
    delay(time/2);
    digitalWrite(led, LOW);
    digitalWrite(buzzer, HIGH);
}

uint8_t channel_from_index(uint8_t channelIndex)
{
    uint8_t loop=0;
    uint8_t channel=0;
    for (loop=0;loop<=CHANNEL_MAX;loop++)
    {
        if(pgm_read_byte_near(channelList + loop) == channelIndex)
        {
            channel=loop;
            break;
        }
    }
    return (channel);
}

/*uint8_t favchannel_from_index(uint8_t channelIndex)
{
    uint8_t loop=0;
    uint8_t channel=0;
    for (loop=0;loop<=numberOfFavs;loop++)
    {
        if(channelFavs[loop] == channelIndex)
        {
            channel=loop;
            break;
        }
    }
    return (channel);
}*/

void wait_rssi_ready()
{
    // CHECK FOR MINIMUM DELAY
    // check if RSSI is stable after tune by checking the time
    uint16_t tune_time = millis()-time_of_tune;
    if(tune_time < MIN_TUNE_TIME)
    {
        // wait until tune time is full filled
        delay(MIN_TUNE_TIME-tune_time);
    }
}

    /************************************************/
    /*   RSSI switching algoritme, heartbeat code   */
    /************************************************/


uint16_t readRSSI()
{
    return readRSSI(-1);
}
uint16_t readRSSI(char receiver)
{
    rssi = 0;
    rssiA = 0;
    rssiB = 0;

    for (uint8_t i = 0; i < RSSI_READS; i++)
    {
        analogRead(rssiPinA);
        rssiA += analogRead(rssiPinA);
        analogRead(rssiPinB);
        rssiB += analogRead(rssiPinB);
    }
    rssiA = rssiA/RSSI_READS;
    rssiB = rssiB/RSSI_READS;

    if(state==STATE_RSSI_SETUP)
    { // RSSI setup
        if(rssiA < rssi_setup_min_a)
        {
            rssi_setup_min_a=rssiA;
        }
        if(rssiA > rssi_setup_max_a)
        {
            rssi_setup_max_a=rssiA;
        }

        if(rssiB < rssi_setup_min_b)
        {
            rssi_setup_min_b=rssiB;
        }
        if(rssiB > rssi_setup_max_b)
        {
            rssi_setup_max_b=rssiB;
        }
    }
//    Serial.print(rssiA);
//    Serial.print(" ");
//    Serial.println(rssiB);
    rssiA = map(rssiA, rssi_min_a, rssi_max_a , 1, 100);   // scale from 1..100%
    rssiB = map(rssiB, rssi_min_b, rssi_max_b , 1, 100);   // scale from 1..100%
    if(receiver == -1) // no receiver was chosen using diversity
    {
        switch(diversity_mode)
        {
            case useReceiverAuto:
                // select receiver
                if((int)abs((float)(((float)rssiA - (float)rssiB) / (float)rssiB) * 100.0) >= DIVERSITY_CUTOVER)
                {
                    if(rssiA > rssiB && diversity_check_count > 0)
                    {
                        diversity_check_count--;
                    }
                    if(rssiA < rssiB && diversity_check_count < DIVERSITY_MAX_CHECKS)
                    {
                        diversity_check_count++;
                    }
                    // have we reached the maximum number of checks to switch receivers?
                    if(diversity_check_count == 0 || diversity_check_count >= DIVERSITY_MAX_CHECKS) {
                        receiver=(diversity_check_count == 0) ? useReceiverA : useReceiverB;
                    }
                    else {
                        receiver=active_receiver;
                    }
                }
                else {
                    receiver=active_receiver;
                }
                break;
            case useReceiverB:
                receiver=useReceiverB;
                break;
            case useReceiverA:
            default:
                receiver=useReceiverA;
        }
        setReceiver(receiver);
    }
    rssiA=constrain(rssiA,1,100);
    rssiB=constrain(rssiB,1,100);
    if(receiver == useReceiverA || state==STATE_RSSI_SETUP){
        rssi = rssiA;
    }else{
        rssi = rssiB;
    }
}

    /*****************************/
    /*   Switch videoreceivers   */
    /*****************************/

void setReceiver(uint8_t receiver) {
    if(receiver == useReceiverA)
    {
        /*
        #define receiverA_led A1
        #define receiverB_led A0
        */
        PORTC = (PORTC & B11111110) | B00000010;
        //digitalWrite(receiverB_led, LOW);
        //digitalWrite(receiverA_led, HIGH);
    }else{
        PORTC = (PORTC & B11111101) | B00000001;
        //digitalWrite(receiverA_led, LOW);
        //digitalWrite(receiverB_led, HIGH);
    }
    active_receiver = receiver;
}

    /**********************/
    /*   SPI maintenace   */
    /**********************/


void setChannelModule(uint8_t channel)
{
  uint8_t i;
  uint16_t channelData;

  channelData = pgm_read_word_near(channelTable + channel);

  // bit bash out 25 bits of data
  // Order: A0-3, !R/W, D0-D19
  // A0=0, A1=0, A2=0, A3=1, RW=0, D0-19=0
  SERIAL_ENABLE_HIGH();
  delayMicroseconds(1);
  //delay(2);
  SERIAL_ENABLE_LOW();

  SERIAL_SENDBIT0();
  SERIAL_SENDBIT0();
  SERIAL_SENDBIT0();
  SERIAL_SENDBIT1();

  SERIAL_SENDBIT0();

  // remaining zeros
  for (i = 20; i > 0; i--)
    SERIAL_SENDBIT0();

  // Clock the data in
  SERIAL_ENABLE_HIGH();
  //delay(2);
  delayMicroseconds(1);
  SERIAL_ENABLE_LOW();

  // Second is the channel data from the lookup table
  // 20 bytes of register data are sent, but the MSB 4 bits are zeros
  // register address = 0x1, write, data0-15=channelData data15-19=0x0
  SERIAL_ENABLE_HIGH();
  SERIAL_ENABLE_LOW();

  // Register 0x1
  SERIAL_SENDBIT1();
  SERIAL_SENDBIT0();
  SERIAL_SENDBIT0();
  SERIAL_SENDBIT0();

  // Write to register
  SERIAL_SENDBIT1();

  // D0-D15
  //   note: loop runs backwards as more efficent on AVR
  for (i = 16; i > 0; i--)
  {
    // Is bit high or low?
    if (channelData & 0x1)
    {
      SERIAL_SENDBIT1();
    }
    else
    {
      SERIAL_SENDBIT0();
    }

    // Shift bits along to check the next one
    channelData >>= 1;
  }

  // Remaining D16-D19
  for (i = 4; i > 0; i--)
    SERIAL_SENDBIT0();

  // Finished clocking data in
  SERIAL_ENABLE_HIGH();
  delayMicroseconds(1);
  //delay(2);

  digitalWrite(slaveSelectPin, LOW);
  digitalWrite(spiClockPin, LOW);
  digitalWrite(spiDataPin, LOW);
}


void SERIAL_SENDBIT1()
{
  digitalWrite(spiClockPin, LOW);
  delayMicroseconds(1);

  digitalWrite(spiDataPin, HIGH);
  delayMicroseconds(1);
  digitalWrite(spiClockPin, HIGH);
  delayMicroseconds(1);

  digitalWrite(spiClockPin, LOW);
  delayMicroseconds(1);
}

void SERIAL_SENDBIT0()
{
  digitalWrite(spiClockPin, LOW);
  delayMicroseconds(1);

  digitalWrite(spiDataPin, LOW);
  delayMicroseconds(1);
  digitalWrite(spiClockPin, HIGH);
  delayMicroseconds(1);

  digitalWrite(spiClockPin, LOW);
  delayMicroseconds(1);
}

void SERIAL_ENABLE_LOW()
{
  delayMicroseconds(1);
  digitalWrite(slaveSelectPin, LOW);
  delayMicroseconds(1);
}

void SERIAL_ENABLE_HIGH()
{
  delayMicroseconds(1);
  digitalWrite(slaveSelectPin, HIGH);
  delayMicroseconds(1);
}
