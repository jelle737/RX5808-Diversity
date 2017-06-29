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

char screens::begin() {
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
    display.print(PSTR2("Diversity:"));
    display.setCursor(display.width()-6*9,8*2+4);
    display.print(PSTR2(" ENABLED"));
    display.display();
    delay(250);
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
    //display.drawRect(0, 0, display.width(), display.height(),WHITE);
    display.fillRect(0, 0, display.width(), 9,WHITE);

    display.setTextSize(1);
    display.setTextColor(BLACK);
    // center text
    display.setCursor(((display.width() - (strlen(title)*6)) / 2),1);
    display.print(title);
    display.setTextColor(WHITE);
}


void screens::modeMenu(uint8_t menu_id) { //Jelle: Main menu screen
//    char m1[] = "MANUAL MODE";
//    char m2[] = "FAVORITES MODE";
//    char m3[] = "SPECTATOR MODE";
//    char m4[] = "BAND SCANNER MODE";
//    char m5[] = "ANTENNA COMPARE MODE";
    //char m6[] = "CUSTOM FREQUENCY";
//    char m6[] = "DIVERSITY MENU";
//    char m7[] = "SETUP MENU";
//    char m8[] = "CALIBRATE RSSI";
//    char* m0[] = {PSTR2("MANUAL MODE"),m2,PSTR2("SPECTATOR MODE"),m4,PSTR2("ANTENNA COMPARE MODE")};
    reset(); // start from fresh screen.
    drawSmallTitleBox(PSTR2("MODE SELECTION"));
    display.fillRect(0, 11*1+9, display.width(), 11,WHITE);
    display.setTextColor(menu_id==0 ? BLACK : WHITE);  
    display.setCursor(5,(5-menu_id+1)%5*11+11);
    display.print(PSTR2("MANUAL MODE"));
    display.setTextColor(menu_id==1 ? BLACK : WHITE);  
    display.setCursor(5,(5-menu_id+2)%5*11+11);
    display.print(PSTR2("FAVORITES MODE"));
    display.setTextColor(menu_id==2 ? BLACK : WHITE);  
    display.setCursor(5,(5-menu_id+3)%5*11+11);
    display.print(PSTR2("SPECTATOR MODE"));
    display.setTextColor(menu_id==3 ? BLACK : WHITE);  
    display.setCursor(5,(5-menu_id+4)%5*11+11);
    display.print(PSTR2("BAND SCANNER MODE"));
    display.setTextColor(menu_id==4 ? BLACK : WHITE);  
    display.setCursor(5,(5-menu_id+5)%5*11+11);
    display.print(PSTR2("ANTENNA COMPARE MODE"));
    display.display();
}

void screens::quickMenu(uint8_t menu_id) { //Jelle: Main menu screen
//    char p1[] = "FASTBOOT HERE";
//    char p2[] = "ADD TO FAVORITE";
//    char p3[] = "REMOVE FROM FAVORITE";
//    char p4[] = "DIVERSITY MENU";
//    char p5[] = "SETTINGS MENU";
//    char* p0[] = {p1,p2,p3,p4,p5};
//    char* p0[] = {PSTR2("FASTBOOT HERE"),PSTR2("ADD TO FAVORITE"),PSTR2("REMOVE FROM FAVORITE"),PSTR2("DIVERSITY MODE"),PSTR2("SETTINGS MENU")};
    reset(); // start from fresh screen.
    drawSmallTitleBox(PSTR2("QUICK MENU"));
    display.fillRect(0, 11*1+9, display.width(), 11,WHITE);
    display.setTextColor(menu_id==0 ? BLACK : WHITE);  
    display.setCursor(5,(5-menu_id+1)%5*11+11);
    display.print(PSTR2("FASTBOOT HERE"));
    display.setTextColor(menu_id==1 ? BLACK : WHITE);  
    display.setCursor(5,(5-menu_id+2)%5*11+11);
    display.print(PSTR2("ADD TO FAVORITE"));
    display.setTextColor(menu_id==2 ? BLACK : WHITE);  
    display.setCursor(5,(5-menu_id+3)%5*11+11);
    display.print(PSTR2("REMOVE FROM FAVORITE"));
    display.setTextColor(menu_id==3 ? BLACK : WHITE);  
    display.setCursor(5,(5-menu_id+4)%5*11+11);
    display.print(PSTR2("DIVERSITY MODE"));
    display.setTextColor(menu_id==4 ? BLACK : WHITE);  
    display.setCursor(5,(5-menu_id+5)%5*11+11);
    display.print(PSTR2("SETTINGS MENU"));
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
    }else if(state == STATE_FAVORITES){
        drawSmallTitleBox(PSTR2("FAVORITE"));
    }
    display.display();
}

//char scan_position = 3;

//JELLE OK
void screens::updateSeekMode(uint8_t state, uint8_t channelIndex, uint8_t channel, uint8_t rssi, uint16_t channelFrequency, uint8_t rssi_seek_threshold, bool locked, bool *favor) {
    if(channel != last_channel) // only updated on changes
    {
        char m1[] = "ABEFRL";
        uint8_t active_band = channelIndex/CHANNEL_BAND_SIZE;
        uint8_t active_channel = channelIndex%CHANNEL_BAND_SIZE; // get channel inside band
        display.drawLine(0, 27, display.width(), 27, BLACK);
        for(int i=0;i<8;i++) {
            display.fillRect(15*i+3,18,15,9,i==active_channel? WHITE:BLACK);
            display.setTextColor(i==active_channel? BLACK:WHITE);
            display.setCursor(15*i+8,19);
            display.print((char) (i+'1'));
            if(favor[active_band*8+i]){
                //favorite markings here
                display.drawRect(15*i+3,17,15,11,WHITE);
            }
        }
        display.drawLine(0, 17, display.width(), 17, BLACK);
        for(int i=0; i<6; i++){
            display.fillRect(15*i+3,9,15,9,i==active_band? WHITE:BLACK);
            display.setTextColor(i==active_band? BLACK:WHITE);
            display.setCursor(15*i+8,10);
            display.print(m1[i]);
        }
        // show frequence
        display.setCursor(101,10);
        display.setTextColor(WHITE,BLACK);
        display.print(channelFrequency);
        display.drawLine(0, display.height()-1, display.width(), display.height()-1, BLACK); //clear the position holding indicator
    }
    // show signal strength
    uint8_t rssi_scaled=map(rssi, 1, 100, 1, display.width());
    display.fillRect(0, 29, display.width(), 3, BLACK);
    display.fillRect(0, 29, rssi_scaled, 3, WHITE);
    rssi_scaled=map(rssi, 1, 100, 1, 30);
    display.fillRect((channel*128/48),display.height()-30,((channel+1)*128/48-channel*128/48),30,BLACK);
    display.fillRect((channel*128/48),display.height()-1-rssi_scaled,((channel+1)*128/48-channel*128/48),rssi_scaled+1,WHITE);
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
    display.display();
}

//JELLE OK
void screens::updateBandScanMode(bool in_setup, uint8_t channel, uint8_t rssi, uint8_t channelIndex, uint16_t channelFrequency, uint16_t rssi_setup_min_a, uint16_t rssi_setup_max_a) {
    uint8_t rssi_scaled=map(rssi, 1, 100, 1, 44);
    uint16_t hight = (display.height()-rssi_scaled);
    if(channel != last_channel) // only updated on changes
    {
       // display.fillRect((channel*3)+4,display.height()-12-32,3,32,BLACK);
        display.drawLine(0, display.height()-1, display.width(), display.height()-1, BLACK);
       // display.fillRect((channel*3)+4,hight,3,rssi_scaled+1,WHITE);
    display.fillRect((channel*128/48),display.height()-44,((channel+1)*128/48-channel*128/48),44,BLACK);
    display.fillRect((channel*128/48),display.height()-rssi_scaled,((channel+1)*128/48-channel*128/48),rssi_scaled+1,WHITE);
 
    }
    if(!in_setup) {
        if (rssi > RSSI_SEEK_TRESHOLD) {
            if(best_rssi < rssi) {
                best_rssi = rssi;
                display.setTextColor(WHITE,BLACK);
                display.setCursor(36,10);
                char m1[] = "ABEFRL";
                display.print(m1[channelIndex/CHANNEL_BAND_SIZE]);
                display.print((char) (channelIndex%CHANNEL_BAND_SIZE+'1'));
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
void screens::screenSaver(uint8_t diversity_mode, uint8_t channelIndex, uint16_t channelFrequency) {
    reset();
    display.setTextSize(6);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    char m1[] = "ABEFRL";
    display.print(m1[channelIndex/CHANNEL_BAND_SIZE]);
    int number = channelIndex%CHANNEL_BAND_SIZE;
    display.print((char) (number+'1'));
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
void screens::screenAntenna(uint8_t diversity_mode, uint8_t channelIndex, uint16_t channelFrequency) {
    reset();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    char m1[] = "ABEFRL";
    display.print(m1[channelIndex/CHANNEL_BAND_SIZE]);
    int number = channelIndex%CHANNEL_BAND_SIZE;
    display.print((char) (number+'1'));
    //display.setTextSize(1);
    display.setCursor(6*3,0);
    display.print(channelFrequency);
    display.setCursor(6*14,0);
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

//    char n1[] = "AUTO";
//    char n2[] = "USE RECEIVER A";
//    char n3[] = "USE RECEIVER B";
//    char* n0[] = {n1,n2,n3};
//    char* n0[] = {PSTR2("AUTO"),PSTR2("USA RECEIVER A"),PSTR2("USE RECEIVER B")};
    display.fillRect(0, 11*diversity_mode+9, display.width(), 11,WHITE);
    //for(int i=0; i<3; i++){
    display.setTextColor(0 == diversity_mode ? BLACK : WHITE);
    display.setCursor(5,11*0+11);
    display.print(PSTR2("AUTO"));
    display.setTextColor(1 == diversity_mode ? BLACK : WHITE);
    display.setCursor(5,11*1+11);
    display.print(PSTR2("USA RECEIVER A"));
    display.setTextColor(2 == diversity_mode ? BLACK : WHITE);
    display.setCursor(5,11*2+11);
    display.print(PSTR2("USA RECEIVER B"));
    
    //}
    // RSSI Strength
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
void screens::updateDiversity(char active_receiver, uint8_t rssiA, uint8_t rssiB){
    updateScreenSaver(active_receiver, rssiA, rssiB);
}




//JELLE OK
void screens::setupMenu(uint8_t menu_id, bool settings_beeps, bool settings_orderby_channel){
//    char s1[] = "ORDER: ";
//    char s2[] = "BEEPS: ";
    //char s3[] = "SPEED: ";
//    char s3[] = "RSSI MENU";
//    char s4[] = "CALLIBRATE RSSI";
//    char s5[] = "EXIT";
//    char* s0[] = {s1,s2,s3,s4,s5};
    char* s0[] = {PSTR2("ORDER: "),PSTR2("BEEPS: "),PSTR2("RSSI MENU"),PSTR2("CALLIBRATE RSSI"),PSTR2("EXIT")};
    reset(); // start from fresh screen.
    drawSmallTitleBox(PSTR2("SETTINGS"));
    display.fillRect(0, 11*1+9, display.width(), 11,WHITE);
    display.setTextColor(menu_id==0 ? BLACK : WHITE);  
    display.setCursor(5,(5-menu_id+1)%5*11+11);
    display.print(PSTR2("ORDER: "));
    if(settings_orderby_channel) {
        display.print(PSTR2("CHANNEL"));
    }
    else {
        display.print(PSTR2("FREQUENCY"));
    }  
    display.setTextColor(menu_id==1 ? BLACK : WHITE);  
    display.setCursor(5,(5-menu_id+2)%5*11+11);
    display.print(PSTR2("BEEPS: "));
    if(settings_beeps) {
        display.print(PSTR2("ON "));
    }
    else {
        display.print(PSTR2("OFF"));
    }
    display.setTextColor(menu_id==2 ? BLACK : WHITE);  
    display.setCursor(5,(5-menu_id+3)%5*11+11);
    display.print(PSTR2("RSSI MENU"));
    display.setTextColor(menu_id==3 ? BLACK : WHITE);  
    display.setCursor(5,(5-menu_id+4)%5*11+11);
    display.print(PSTR2("CALIBRATE RSSI"));
    display.setTextColor(menu_id==4 ? BLACK : WHITE);  
    display.setCursor(5,(5-menu_id+5)%5*11+11);
    display.print(PSTR2("EXIT"));
    display.display();
}

void screens::rssiMenu(uint8_t menu_id, uint16_t rssi_min_a, uint16_t rssi_max_a, uint16_t rssi_min_b, uint16_t rssi_max_b,bool editing){
//    char o1[] = "A MIN: ";
//    char o2[] = "A MAX: ";
//    char o3[] = "B MIN: ";
//    char o4[] = "B MAX: ";
//    char o5[] = "DISCARD CHANGES";
//    char o6[] = "SAVE CHANGES";
//    char* o0[] = {o1,o2,o3,o4,o5,o6};
//    char* o0[] = {PSTR2("A MIN: "),PSTR2("A MAX: "),PSTR2("B MIN: "),PSTR2("B MAX: "),PSTR2("DISCARD CHANGES"),PSTR2("SAVE CHANGES")};
    reset(); // start from fresh screen.
    drawSmallTitleBox(PSTR2("RSSI MENU"));
    display.fillRect(0, 11*1+9, display.width(), 11,WHITE);
    if(editing){
        display.fillRect(6*6+5, 11*1+10, display.width()-(6*6+6), 9,BLACK);
    }
    display.setTextColor(menu_id==0 ? BLACK : WHITE);  
    display.setCursor(5,(6-menu_id+1)%6*11+11);
    display.print(PSTR2("A MIN: "));
    display.setTextColor(!editing&&menu_id==0?BLACK:WHITE);
    display.print( rssi_min_a , DEC);
    display.setTextColor(menu_id==1 ? BLACK : WHITE);  
    display.setCursor(5,(6-menu_id+2)%6*11+11);
    display.print(PSTR2("A MAX: "));
    display.setTextColor(!editing&&menu_id==1?BLACK:WHITE);
    display.print( rssi_max_a , DEC);
    display.setTextColor(menu_id==2 ? BLACK : WHITE);  
    display.setCursor(5,(6-menu_id+3)%6*11+11);
    display.print(PSTR2("B MIN: "));
    display.setTextColor(!editing&&menu_id==2?BLACK:WHITE);
    display.print( rssi_min_b , DEC);
    display.setTextColor(menu_id==3 ? BLACK : WHITE);  
    display.setCursor(5,(6-menu_id+4)%6*11+11);
    display.print(PSTR2("B MAX: "));
    display.setTextColor(!editing&&menu_id==3?BLACK:WHITE);
    display.print( rssi_max_b , DEC);
    display.setTextColor(menu_id==4 ? BLACK : WHITE);  
    display.setCursor(5,(6-menu_id+5)%6*11+11);
    display.print(PSTR2("DISCARD CHANGES"));
    display.setTextColor(menu_id==5 ? BLACK : WHITE);  
    display.setCursor(5,(6-menu_id+6)%6*11+11);
    display.print(PSTR2("SAVE CHANGES"));
    display.display();

/*

    for(int i=0; i<5; i++){
        display.setTextColor(i == 1 ? BLACK : WHITE);  
        display.setCursor(5,11*i+11);
        display.print(o0[(menu_id+6-1+i)%6]);
        display.setTextColor(i == 1&&!editing ? BLACK : WHITE);
        if((menu_id+6-1+i)%6==0){
            display.print( rssi_min_a , DEC);           
        }else if((menu_id+6-1+i)%6==1){
            display.print( rssi_max_a , DEC);
        }else if((menu_id+6-1+i)%6==2){
            display.print( rssi_min_b , DEC);
        }else if((menu_id+6-1+i)%6==3){
            display.print( rssi_max_b , DEC);
        }
    }      
    display.display();*/
}

#endif
