/*  Jordan Guzak
 *  Generator 1
 * 
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h>
#include <tables/triangle2048_int8.h>
#include <tables/square_no_alias_2048_int8.h>
#include <tables/saw2048_int8.h>
#include <LowPassFilter.h>
#include <DCfilter.h>

const bool SERIAL_DEBUG = true;
const char volume_pin = 0;
const char pitch_pin = 1;

const char wave_type_pin = 0;
const int WAVE_TYPES = 4;

const int lpf_frequency_pin = 2;
const int lpf_resonance_pin = 3;

byte g_volume = 1;
int g_pitch = 440;
int g_wave_type = 0, g_next_wave_type = 0;
int g_lpf_frequency = 0, g_lpf_resonance = 10;

bool buttonClicked = false;

Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> osc1(SIN2048_DATA);

LowPassFilter lpf;

#define CONTROL_RATE 64

void setup() {
    pinMode(wave_type_pin, INPUT);
    
    osc1.setFreq(g_pitch);
    lpf.setCutoffFreq(g_lpf_frequency);
    lpf.setResonance(g_lpf_resonance);
    
    if (SERIAL_DEBUG) {
        Serial.begin(115200);
    }
    
    startMozzi(CONTROL_RATE);
}

void updateControl() {
    volatile int temp;

    // update volume
    temp = map(mozziAnalogRead(volume_pin), 0, 1023, 0, 255);
    if (g_volume != temp) {
        g_volume = temp;

        if (SERIAL_DEBUG) {
            Serial.print("Volume: ");
            Serial.print(g_volume);
            Serial.print("\n");
        }  
    }
    
    // update pitch
    temp = map(mozziAnalogRead(pitch_pin), 0, 1023, 20, 700);
    if (g_pitch != temp) {
        g_pitch = temp;
        osc1.setFreq(g_pitch);

        if (SERIAL_DEBUG) {
            Serial.print("Pitch: ");
            Serial.print(g_pitch);
            Serial.print("\n");
        }
    }

    // wave type selection
    if (g_wave_type != g_next_wave_type) {
        switch(g_next_wave_type) {
          case 0:
              osc1.setTable(SIN2048_DATA);
              break;
          case 1:
              osc1.setTable(TRIANGLE2048_DATA);
              break;
          case 2:
              osc1.setTable(SQUARE_NO_ALIAS_2048_DATA);
              break;
          case 3:
              osc1.setTable(SAW2048_DATA);
              break;
          default:
              break;
        }

        if (SERIAL_DEBUG) {
            Serial.println("Waveform changed");
            Serial.println();
        }
        
        g_wave_type = g_next_wave_type;
    }

    // update lpf frequency
    temp = map(mozziAnalogRead(lpf_frequency_pin), 0, 1023, 0, 255);
    if (g_lpf_frequency != temp) {
        g_lpf_frequency = temp;
        lpf.setCutoffFreq(g_lpf_frequency);

        if (SERIAL_DEBUG) {
            Serial.print("LPF Frequency: ");
            Serial.print(g_lpf_frequency);
            Serial.print("\n");
        }
    }

    // update lpf resonance
    /*
    temp = map(mozziAnalogRead(lpf_resonance_pin), 0, 1023, 0, 255);
    if (g_lpf_resonance != temp) {
        g_lpf_resonance = temp;
        lpf.setResonance(g_lpf_resonance);

        if (SERIAL_DEBUG) {
            Serial.print("LPF Resonance: ");
            Serial.print(g_lpf_resonance);
            Serial.print("\n");
        }
    }
    */
    
}

int updateAudio() {
    //char sig = lpf.next(osc1.next() * g_volume);
    //return  (int) sig >> 8;
    return (lpf.next(osc1.next()) * g_volume) >> 8;
}

void loop() {

    if (digitalRead(wave_type_pin) == HIGH && !buttonClicked) {
        if (SERIAL_DEBUG) {
            Serial.println("Click");
        }

        g_next_wave_type = (g_next_wave_type + 1) % (WAVE_TYPES);
        
        buttonClicked = true;
    }

    if (digitalRead(wave_type_pin) == LOW && buttonClicked) {
        if (SERIAL_DEBUG) {
            Serial.println("Clack");
        }
        buttonClicked = false;
    }
  
    audioHook();
}
