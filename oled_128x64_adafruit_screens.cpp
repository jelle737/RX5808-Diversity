/*
 * OLED Screens by Shea Ivey

The MIT License (MIT)

Copyright (c) 2015 Shea Ivey

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

#include "settings.h"

#ifdef OLED_128x64_ADAFRUIT_SCREENS
#include "screens.h" // function headers
#ifdef SH1106
	#include <Adafruit_SH1106.h>
#else
	#include <Adafruit_SSD1306.h>
#endif
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <SPI.h>

// New version of PSTR that uses a temp buffer and returns char *
// by Shea Ivey
#define PSTR2(x) PSTRtoBuffer_P(PSTR(x))
char PSTR2_BUFFER[30]; // adjust size depending on need.
char *PSTRtoBuffer_P(PGM_P str) { uint8_t c='\0', i=0; for(; (c = pgm_read_byte(str)) && i < sizeof(PSTR2_BUFFER); str++, i++) PSTR2_BUFFER[i]=c;PSTR2_BUFFER[i]=c; return PSTR2_BUFFER;}

#define INVERT INVERSE
#define OLED_RESET 4
#ifdef SH1106
	Adafruit_SH1106 display(OLED_RESET);
	#if !defined SH1106_128_64
		#error("Screen size incorrect, please fix Adafruit_SH1106.h!");
	#endif
#else
	Adafruit_SSD1306 display(OLED_RESET);
	#if !defined SSD1306_128_64
		#error("Screen size incorrect, please fix Adafruit_SSD1306.h!");
	#endif
#endif


screens::screens() {
    last_channel = -1;
//    last_rssi = 0;
}

char screens::begin(const char *call_sign) {
    // Set the address of your OLED Display.
    // 128x64 ONLY!!
#ifdef SH1106
    display.begin(SH1106_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D or 0x3C (for the 128x64)
#else
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D or 0x3C (for the 128x64)
#endif


#ifdef USE_FLIP_SCREEN
    flip();
#endif

#ifdef USE_BOOT_LOGO
    //display.display(); // show splash screen
    //delay(3000);
#endif
    // init done
    reset();

    display.fillRect(0, 0, display.width(), 11,WHITE);
    display.setTextColor(BLACK);
    display.setCursor(((display.width() - (10*6)) / 2),2);
    display.print(PSTR2("Boot Check"));

    display.setTextColor(WHITE);
    display.setCursor(0,8*1+4);
    display.print(PSTR2("Power:"));
    display.setCursor(display.width()-6*3,8*1+4);
    display.print(PSTR2("OK"));
    display.setCursor(0,8*2+4);

    //display.display();
//#ifdef USE_DIVERSITY
    display.print(PSTR2("Diversity:"));
    //display.display();
    //delay(250);
    display.setCursor(display.width()-6*9,8*2+4);
    //if(isDiversity()) {
        display.print(PSTR2(" ENABLED"));
    //}
    //else {
    //    display.print(PSTR2("DISABLED"));
    //}
//#endif
    display.setCursor(((display.width() - (strlen(call_sign)*12)) / 2),8*4+4);
    display.setTextSize(2);
    display.print(call_sign);
    display.display();
    delay(1250);
    return 0; // no errors
}

void screens::reset() {
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
}

void screens::flip() {
    display.setRotation(2);
}

//JELLE OK
void screens::drawSmallTitleBox(const char *title) {
    display.drawRect(0, 0, display.width(), display.height(),WHITE);
    display.fillRect(0, 0, display.width(), 9,WHITE);

    display.setTextSize(1);
    display.setTextColor(BLACK);
    // center text
    display.setCursor(((display.width() - (strlen(title)*6)) / 2),1);
    display.print(title);
    display.setTextColor(WHITE);
}

//JELLE OK
void screens::mainMenu(uint8_t menu_id) { //Jelle: Main menu screen
    reset(); // start from fresh screen.
    drawSmallTitleBox(PSTR2("MODE SELECTION"));
    display.fillRect(0, 9*menu_id+9, display.width(), 9,WHITE);
    display.setTextColor(menu_id == 0 ? BLACK : WHITE);
    display.setCursor(5,9*0+10);
    display.print(PSTR2("MANUAL MODE"));
    display.setTextColor(menu_id == 1 ? BLACK : WHITE);
    display.setCursor(5,9*1+10);
    display.print(PSTR2("SPECTATOR MODE"));
    display.setTextColor(menu_id == 2 ? BLACK : WHITE);
    display.setCursor(5,9*2+10);
    display.print(PSTR2("BAND SCANNER MODE"));
    display.setTextColor(menu_id == 3 ? BLACK : WHITE);
    display.setCursor(5,9*3+10);
    display.print(PSTR2("ANTENNA COMPARE MODE"));
    display.setTextColor(menu_id == 4 ? BLACK : WHITE);
    display.setCursor(5,9*4+10);
    display.print(PSTR2("DIVERSITY MENU"));
    display.setTextColor(menu_id == 5 ? BLACK : WHITE);
    display.setCursor(5,9*5+10);
    display.print(PSTR2("SETUP MENU"));
    display.display();
}

//JELLE OK
void screens::seekMode(uint8_t state) {
    last_channel = -1;
    reset(); // start from fresh screen.
    if (state == STATE_MANUAL){
        drawSmallTitleBox(PSTR2("MANUAL"));
    }else if(state == STATE_SEEK){
        drawSmallTitleBox(PSTR2("AUTO SEEK"));
    }else if(state == STATE_ANTENNA){
        drawSmallTitleBox(PSTR2("ANTENNA COMPARE"));
    }
    display.setTextColor(WHITE);
    display.drawLine(0, 18, display.width(), 18, WHITE);
    display.drawLine(0, 30, display.width(), 30, WHITE);
    display.setCursor(5,10);
    display.drawLine(97,9,97,18,WHITE);
    display.print(PSTR2("BAND:"));
    for(uint16_t i=0;i<8;i++) {
        display.setCursor(15*i+8,21);
        display.print((char) (i+'1'));
    }
    display.drawLine(0, 33, display.width(), 33, WHITE);
    display.drawLine(0, display.height()-11, display.width(), display.height()-11, WHITE);
    display.setCursor(2,display.height()-9);
    display.print(PSTR2("5645"));
    display.setCursor(55,display.height()-9);
    display.print(PSTR2("5800"));
    display.setCursor(display.width()-25,display.height()-9);
    display.print(PSTR2("5945"));
    display.display();
}

//char scan_position = 3;

//JELLE OK
void screens::updateSeekMode(uint8_t state, uint8_t channelIndex, uint8_t channel, uint8_t rssi, uint16_t channelFrequency, uint8_t rssi_seek_threshold, bool locked) {
    if(channel != last_channel) // only updated on changes
    {
        display.setTextColor(WHITE,BLACK);
        display.setCursor(36,10);
        // show current used channel of bank
        if(channelIndex > 31){
            display.print(PSTR2("C/Race   "));
        }else if(channelIndex > 23){
            display.print(PSTR2("F/Airwave"));
        }else if (channelIndex > 15){
            display.print(PSTR2("E        "));
        }else if (channelIndex > 7){
            display.print(PSTR2("B        "));
        }else{
            display.print(PSTR2("A        "));
        }

        uint8_t active_channel = channelIndex%CHANNEL_BAND_SIZE; // get channel inside band
        for(int i=0;i<8;i++) {
            display.fillRect(15*i+3,19,15,11,i==active_channel? WHITE:BLACK);
            display.setTextColor(i==active_channel? BLACK:WHITE);
            display.setCursor(15*i+8,21);
            display.print((char) (i+'1'));
        }

        // show frequence
        display.setCursor(101,10);
        display.setTextColor(WHITE,BLACK);
        display.print(channelFrequency);
        display.drawLine(4, display.height()-12, display.width()-5, display.height()-12, BLACK);
    }
    // show signal strength
    uint8_t rssi_scaled=map(rssi, 1, 100, 1, display.width()-3);
    display.fillRect(1, 31, display.width()-2, 2, BLACK);
    display.fillRect(1, 31, rssi_scaled, 2, WHITE);
    rssi_scaled=map(rssi, 1, 100, 1, 17);
    display.fillRect((channel*3)+4,display.height()-12-17,3,17,BLACK);
    display.fillRect((channel*3)+4,display.height()-12-rssi_scaled,3,rssi_scaled+1,WHITE);
    
    // handling for seek mode after screen and RSSI has been fully processed
    if(state == STATE_SEEK) //
    { // SEEK MODE
        if(locked) // search if not found
        {
            display.setTextColor(BLACK,WHITE);
            display.setCursor(((display.width()-9*6)/2),1);
            display.print(PSTR2("AUTO LOCK"));
        }
        else
        {
            display.setTextColor(BLACK,WHITE);
            display.setCursor(((display.width()-9*6)/2),1);
            display.print(PSTR2("AUTO SEEK"));
        }
    }

    last_channel = channel;
    display.display();
}

//JELLE OK
void screens::bandScanMode(uint8_t state) {
    reset(); // start from fresh screen.
    best_rssi = 0;
    last_channel = -1;
    if(state==STATE_SCAN){
        drawSmallTitleBox(PSTR2("BAND SCANNER"));
        display.setCursor(5,10);
        display.print(PSTR2("BEST:"));
    }else{
        drawSmallTitleBox(PSTR2("RSSI SETUP"));
        display.setCursor(5,10);
        display.print(PSTR2("Min:     Max:"));
    }
    display.drawLine(0, 18, display.width(), 18, WHITE);

    display.drawLine(0, display.height()-11, display.width(), display.height()-11, WHITE);
    display.setCursor(2,display.height()-9);
    display.print(PSTR2("5645"));
    display.setCursor(55,display.height()-9);
    display.print(PSTR2("5800"));
    display.setCursor(display.width()-25,display.height()-9);
    display.print(PSTR2("5945"));
    display.display();
}

//JELLE OK
void screens::updateBandScanMode(bool in_setup, uint8_t channel, uint8_t rssi, uint8_t channelName, uint16_t channelFrequency, uint16_t rssi_setup_min_a, uint16_t rssi_setup_max_a) {
    uint8_t rssi_scaled=map(rssi, 1, 100, 1, 32);
    uint16_t hight = (display.height()-12-rssi_scaled);
    if(channel != last_channel) // only updated on changes
    {
        display.fillRect((channel*3)+4,display.height()-12-32,3,32,BLACK);
        display.drawLine(4, display.height()-12, display.width()-5, display.height()-12, BLACK);
        display.fillRect((channel*3)+4,hight,3,rssi_scaled+1,WHITE);
    }
    if(!in_setup) {
        if (rssi > RSSI_SEEK_TRESHOLD) {
            if(best_rssi < rssi) {
                best_rssi = rssi;
                display.setTextColor(WHITE,BLACK);
                display.setCursor(36,10);
                display.print(channelName, HEX);
                display.setCursor(52,10);
                display.print(channelFrequency);
            }

        }
    }else {
        display.setCursor(30,10);
        display.setTextColor(WHITE,BLACK);
        display.print( PSTR2("   ") );
        display.setCursor(30,10);
        display.print( rssi_setup_min_a , DEC);
        display.setCursor(85,10);
        display.print( PSTR2("   ") );
        display.setCursor(85,10);
        display.print( rssi_setup_max_a , DEC);
    }
    display.display();
    last_channel = channel;
}


//JELLE OK
void screens::screenSaver(uint8_t diversity_mode, uint8_t channelName, uint16_t channelFrequency, const char *call_sign) {
    reset();
    display.setTextSize(6);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print(channelName, HEX);
    display.setTextSize(1);
    display.setCursor(70,0);
    display.print(call_sign);
    display.setTextSize(2);
    display.setCursor(70,28);
    display.setTextColor(WHITE);
    display.print(channelFrequency);
    display.setTextSize(1);
    display.setCursor(70,18);
    switch(diversity_mode) {
        case useReceiverAuto:
            display.print(PSTR2("AUTO"));
            break;
        case useReceiverA:
            display.print(PSTR2("ANTENNA A"));
            break;
        case useReceiverB:
            display.print(PSTR2("ANTENNA B"));
            break;
    }
    display.setTextColor(BLACK,WHITE);
    display.fillRect(0, 45, 7, 9, WHITE);
    display.setCursor(1, 46);
    display.print("A");
    display.fillRect(0, 55, 7, 9, WHITE);
    display.setCursor(1, 56);
    display.print("B");
    display.display();
}


//JELLE OK
void screens::screenAntenna(uint8_t diversity_mode, uint8_t channelName, uint16_t channelFrequency, const char *call_sign) {
    reset();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print(channelName, HEX);
    //display.setTextSize(1);
    display.setCursor(6*3,0);
    display.print(channelFrequency);
    display.setCursor(6*14,0);
    display.print(call_sign);
//    display.setTextSize(1);
//#ifdef USE_DIVERSITY
//    if(isDiversity()) {
    display.setCursor(6*8,0);
    switch(diversity_mode) {
        case useReceiverAuto:
            display.print(PSTR2("AUTO"));
            break;
        case useReceiverA:
            display.print(PSTR2("ANT A"));
            break;
        case useReceiverB:
            display.print(PSTR2("ANT B"));
            break;
    }
    Aactive = 1;
    Bactive = 1;
    Arssi = 1;
    Brssi = 1;
    display.setTextColor(WHITE);
    display.setCursor(((display.width()-6*6)/2),display.height()-55);
    display.print(PSTR2("SIGNAL"));
    display.setCursor(((display.width()-6*6)/2),display.height()-37);
    display.print(PSTR2("USAGE"));
    display.setTextColor(BLACK,WHITE);
    display.fillRect(0, display.height()-47, display.width()-1, 9, WHITE);
    display.setCursor(1,display.height()-46);
    display.print("A");
    display.setCursor(display.width()-7,display.height()-46);
    display.print("B");
    display.fillRect(0, display.height()-29, display.width()-1, 9, WHITE);
    display.setCursor(1,display.height()-28);
    display.print("A");
    display.setCursor(display.width()-7,display.height()-28);
    display.print("B");
    display.fillRect(0, display.height()-19, 7, 9, WHITE);
    display.setCursor(1,display.height()-18);
    display.print("A");
    display.fillRect(0, display.height()-9, 7, 9, WHITE);
    display.setCursor(1,display.height()-8);
    display.print("B");
    display.setTextColor(WHITE,BLACK);
    display.setCursor(14,9);
    display.print(PSTR2("  %"));
    display.setCursor(100,9);
    display.print(PSTR2("  %"));
    display.setCursor(14,27);
    display.print(PSTR2("  %"));
    display.setCursor(100,27);
    display.print(PSTR2("  %"));

    display.display();
}



//JELLE OK
void screens::updateScreenAntenna(char active_receiver, uint8_t rssiA, uint8_t rssiB){
    (active_receiver == useReceiverA ? Aactive++ : Bactive++);
    Arssi+=rssiA;
    Brssi+=rssiB;
    display.fillRect(7, 36, 113, 7, BLACK);
    display.fillRect(7, 18, 113, 7, BLACK);
    uint8_t rssi_scaled=map(Arssi, 0, Arssi+Brssi, 0, 113);
    display.fillRect(7, 18, rssi_scaled, 7, WHITE);
    rssi_scaled=map(Arssi, 0, Arssi+Brssi, 1, 99);
    display.setTextColor(WHITE,BLACK);
    display.setCursor(14,9);
    display.print(PSTR2("  "));
    display.setCursor(14,9);
    display.print(rssi_scaled);
    
    display.setCursor(100,9);
    display.print(PSTR2("  "));
    display.setCursor(100,9);
    display.print(100-rssi_scaled);
        
    rssi_scaled=map(Aactive, 0, Aactive+Bactive, 0, 113);
    display.fillRect(7, 36, rssi_scaled, 7, WHITE);
    rssi_scaled=map(Aactive, 0, Aactive+Bactive, 1, 99);
    display.setTextColor(WHITE,BLACK);
    display.setCursor(14,27);
    display.print(PSTR2("  "));
    display.setCursor(14,27);
    display.print(rssi_scaled);
    display.setCursor(100,27);
    display.print(PSTR2("  "));
    display.setCursor(100,27);
    display.print(100-rssi_scaled);
    display.display();
}



//JELLE OK
void screens::updateScreenSaver(char active_receiver, uint8_t rssiA, uint8_t rssiB) {

    uint8_t rssi_scaled=map(rssiA, 1, 100, 3, 121);
    display.fillRect(7, display.height()-19, 121, 19, BLACK);
    if(active_receiver == useReceiverA){
        display.fillRect(6, display.height()-19, rssi_scaled, 9, WHITE);
    }else{
        display.drawRect(6, display.height()-19, rssi_scaled, 9, WHITE);
    }

    // read rssi B
    rssi_scaled=map(rssiB, 1, 100, 3, 121);
    if(active_receiver == useReceiverB){
        display.fillRect(6, display.height()-9, rssi_scaled, 9, WHITE);
    }else{
         display.drawRect(6, display.height()-9, rssi_scaled, 9, WHITE);
    }
    if(rssiA < 20){
        display.setTextColor((millis()%250 < 125) ? WHITE : BLACK, BLACK);
        display.setCursor(50,display.height()-13);
        display.print(PSTR2("LOW SIGNAL"));
    }
    display.display();
}



//JELLE OK
void screens::diversity(uint8_t diversity_mode) {

    reset();
    drawSmallTitleBox(PSTR2("DIVERSITY"));

    //selected
    display.fillRect(0, 9*diversity_mode+9, display.width(), 9, WHITE);

    display.setTextColor(diversity_mode == useReceiverAuto ? BLACK : WHITE);
    display.setCursor(5,9*0+10);
    display.print(PSTR2("AUTO"));

    display.setTextColor(diversity_mode == useReceiverA ? BLACK : WHITE);
    display.setCursor(5,9*1+10);
    display.print(PSTR2("USE RECEIVER A"));
    display.setTextColor(diversity_mode == useReceiverB ? BLACK : WHITE);
    display.setCursor(5,9*2+10);
    display.print(PSTR2("USE RECEIVER B"));

    // RSSI Strength
    display.setTextColor(WHITE);
    display.drawRect(0, display.height()-21, display.width(), 11, WHITE);
    display.setCursor(5,display.height()-19);
    display.print("A:");
    display.setCursor(5,display.height()-9);
    display.print("B:");
    display.display();
}

//JELLE OK
void screens::updateDiversity(char active_receiver, uint8_t rssiA, uint8_t rssiB){
//    #define RSSI_BAR_SIZE 108
    uint8_t rssi_scaled=map(rssiA, 1, 100, 1, 108);

    display.fillRect(18, display.height()-19, 108, 7, BLACK);
    display.fillRect(18, display.height()-9, 108, 7, BLACK);
    if(active_receiver==useReceiverA){
        display.fillRect(18, display.height()-19, rssi_scaled, 7, WHITE);
    }else{
        display.drawRect(18, display.height()-19, rssi_scaled, 7, WHITE);
    }

    // read rssi B
    rssi_scaled=map(rssiB, 1, 100, 1, 108);
    if(active_receiver==useReceiverB){
        display.fillRect(18, display.height()-9, rssi_scaled, 7, WHITE);
    }else{
        display.drawRect(18, display.height()-9, rssi_scaled, 7, WHITE);
    }
    display.display();
}



//void screens::setupMenu(){
//}
//JELLE OK
void screens::SetupMenu(uint8_t menu_id, bool settings_beeps, bool settings_orderby_channel, const char *call_sign, char editing){
    reset();
    drawSmallTitleBox(PSTR2("SETUP MENU"));
    //selected
    display.fillRect(0, 9*menu_id+9, display.width(), 9, WHITE);

    display.setTextColor(menu_id == 0 ? BLACK : WHITE);
    display.setCursor(5,9*0+10);
    display.print(PSTR2("ORDER: "));
    if(settings_orderby_channel) {
        display.print(PSTR2("CHANNEL  "));
    }
    else {
        display.print(PSTR2("FREQUENCY"));
    }

    display.setTextColor(menu_id == 1 ? BLACK : WHITE);
    display.setCursor(5,9*1+10);
    display.print(PSTR2("BEEPS: "));
    if(settings_beeps) {
        display.print(PSTR2("ON "));
    }
    else {
        display.print(PSTR2("OFF"));
    }


    display.setTextColor(menu_id == 2 ? BLACK : WHITE);
    display.setCursor(5,9*2+10);
    display.print(PSTR2("SIGN : "));
    if(editing>=0) {
        display.fillRect(6*6+5, 9*2+10, display.width()-(6*6+6), 7, BLACK);
        display.fillRect(6*7+6*(editing)+4, 9*2+10, 7, 7, WHITE); //set cursor
        for(uint8_t i=0; i<10; i++) {
            display.setTextColor(i == editing ? BLACK : WHITE);
            display.print(call_sign[i]);
        }
    }
    else {
        display.print(call_sign);
    }

    display.setTextColor(menu_id == 3 ? BLACK : WHITE);
    display.setCursor(5,9*3+10);
    display.print(PSTR2("CALIBRATE RSSI"));
    display.setTextColor(menu_id == 4 ? BLACK : WHITE);
    display.setCursor(5,9*4+10);
    display.print(PSTR2("RSSI MENU"));
    display.setTextColor(menu_id == 5 ? BLACK : WHITE);
    display.setCursor(5,9*5+10);
    display.print(PSTR2("SAVE SETUP & EXIT"));
    display.display();
}

//JELLE OK
void screens::save(uint8_t mode, uint8_t channelIndex, uint16_t channelFrequency,const char *call_sign) {
    reset();
    drawSmallTitleBox(PSTR2("SAVE SETTINGS"));
    display.setTextColor(WHITE);
    display.setCursor(5,9*0+10);
    display.print(PSTR2("MODE:"));
    display.setCursor(38,9*0+10);
    switch (mode)
    {
        case STATE_SCAN: // Band Scanner
            display.print(PSTR2("BAND SCANNER"));
        break;
        case STATE_MANUAL: // manual mode
            display.print(PSTR2("MANUAL"));
        break;
        case STATE_SEEK: // seek mode
            display.print(PSTR2("AUTO SEEK"));
        break;
        case STATE_ANTENNA:
            display.print(PSTR2("ANTENNA COMP"));
        break;
    }

    display.setCursor(5,9*1+10);
    display.print(PSTR2("BAND:"));
    display.setCursor(38,9*1+10);
    // print band
    if(channelIndex > 31){
        display.print(PSTR2("C/Race"));
    }else if(channelIndex > 23){
        display.print(PSTR2("F/Airwave"));
    }else if (channelIndex > 15){
        display.print(PSTR2("E"));
    }else if (channelIndex > 7){
        display.print(PSTR2("B"));
    }else{
        display.print(PSTR2("A"));
    }

    display.setCursor(5,9*2+10);
    display.print(PSTR2("CHAN:"));
    display.setCursor(38,9*2+10);
    uint8_t active_channel = channelIndex%CHANNEL_BAND_SIZE+1; // get channel inside band
    display.print(active_channel,DEC);
    display.setCursor(5,9*3+10);
    display.print(PSTR2("FREQ:     GHz"));
    display.setCursor(38,9*3+10);
    display.print(channelFrequency);

    display.setCursor(5,9*4+10);
    display.print(PSTR2("SIGN:"));
    display.setCursor(38,9*4+10);
    display.print(call_sign);

    display.setCursor(((display.width()-11*6)/2),9*5+10);
    display.print(PSTR2("-- SAVED --"));
    display.display();
}


void screens::rssiMenu(uint16_t rssi_min_a, uint16_t rssi_max_a, uint16_t rssi_min_b, uint16_t rssi_max_b,uint8_t menu_id,char editing){
    reset();
    drawSmallTitleBox(PSTR2("RSSI MENU"));  
    display.fillRect(0, 9*menu_id+9, display.width(), 9,WHITE);
    if(editing>=0) {
        display.fillRect(6*6+5, 9*menu_id+10, display.width()-(6*6+6), 7, BLACK);
    }
    
    
    
    display.setTextColor(menu_id == 0 ? BLACK : WHITE);
    display.setCursor(5,9*0+10);
    display.print(PSTR2("A MIN: "));
    if(editing == 0) display.setTextColor(WHITE);
    display.print( rssi_min_a , DEC);
    display.setTextColor(menu_id == 1 ? BLACK : WHITE);
    display.setCursor(5,9*1+10);
    display.print(PSTR2("A MAX: "));
    if(editing == 1) display.setTextColor(WHITE);
    display.print( rssi_max_a , DEC);
    display.setTextColor(menu_id == 2 ? BLACK : WHITE);
    display.setCursor(5,9*2+10);
    display.print(PSTR2("B MIN: "));
    if(editing == 2) display.setTextColor(WHITE);
    display.print( rssi_min_b , DEC);
    display.setTextColor(menu_id == 3 ? BLACK : WHITE);
    display.setCursor(5,9*3+10);
    display.print(PSTR2("B MAX: "));
    if(editing == 3) display.setTextColor(WHITE);
    display.print( rssi_max_b , DEC);
    display.setTextColor(menu_id == 4 ? BLACK : WHITE);
    display.setCursor(5,9*4+10);
    display.print(PSTR2("DISCARD & RETURN"));
    display.setTextColor(menu_id == 5 ? BLACK : WHITE);
    display.setCursor(5,9*5+10);
    display.print(PSTR2("SAVE RSSI & EXIT"));
    display.display();
}


#endif
