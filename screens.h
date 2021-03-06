/*
 * Screens Class by Shea Ivey

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

#ifndef display_h
#define display_h


#include <avr/pgmspace.h>

//Each screen has the following
// public entry method
// private draw method
// public update method
// private update draw method

class screens
{
    private: // helper functions for screens.
 //       uint8_t last_rssi;
        uint8_t best_rssi;
        uint8_t last_channel;
        uint32_t Aactive = 1;
        uint32_t Bactive = 1;
        uint32_t Arssi = 1;
        uint32_t Brssi = 1;
        void reset();
        //void drawTitleBox(const char *title);
        void drawSmallTitleBox(const char *title);

    public:
        screens();
        char begin();
        void flip();

        // MAIN MENU
        //void mainMenu(uint8_t menu_id, char* menu_items[],uint8_t menu_size, uint8_t menu_show);
        void modeMenu(uint8_t menu_id);

        void quickMenu(uint8_t menu_id);

        void settingsMenu(uint8_t menu_id);

        // SEEK & MANUAL MODE
        void seekMode(uint8_t state); // seek and manual mode
        void updateSeekMode(uint8_t state, uint8_t channelIndex, uint8_t channel, uint8_t rssi, uint16_t channelFrequency, uint8_t rssi_seek_threshold, bool locked, bool *favor); // seek and manual mode

        // BAND SCAN
        void bandScanMode(uint8_t state);
        void updateBandScanMode(bool in_setup, uint8_t channel, uint8_t rssi, uint8_t channelIndex, uint16_t channelFrequency, uint16_t rssi_setup_min_a, uint16_t rssi_setup_max_a);

        // SCREEN SAVER

        void screenSaver(uint8_t diversity_mode, uint8_t channelIndex, uint16_t channelFrequency);

        void updateScreenSaver(char active_receiver, uint8_t rssiA, uint8_t rssiB); // diversity

        //ANTENNA

        void screenAntenna(uint8_t diversity_mode, uint8_t channelIndex, uint16_t channelFrequency);
        void updateScreenAntenna(char active_receiver, uint8_t rssiA, uint8_t rssiB);


        // DIVERSITY
        void diversity(uint8_t diversity_mode);
        void updateDiversity(char active_receiver, uint8_t rssiA, uint8_t rssiB);

        // SETUP MENU

        //void SetupMenu(uint8_t menu_id,bool settings_beeps,bool settings_orderby_channel, char editing);

        // SAVE
//        void save(uint8_t mode, uint8_t channelIndex, uint16_t channelFrequency);
//        void save2(uint8_t menu_id);
        void setupMenu(uint8_t menu_id, bool settings_beeps, bool settings_orderby_channel);


        // RSSI MENU
        //void rssiMenu(uint16_t rssi_min_a, uint16_t rssi_max_a, uint16_t rssi_min_b, uint16_t rssi_max_b, uint8_t menu_id, char editing);
        void rssiMenu(uint8_t menu_id,  uint16_t rssi_min_a, uint16_t rssi_max_a, uint16_t rssi_min_b, uint16_t rssi_max_b,bool editing);


};
#endif
