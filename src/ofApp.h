#pragma once
#include "ofMain.h"
#include "ofxOsc.h"

#define OSC_PORT 8000

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();

    void processOSC();

    // OSC
    ofxOscReceiver osc;

    // Bandas
    float low, mid, high;
    float lowSmooth, midSmooth, highSmooth;

    float lowThreshold;
    float midThreshold;
    float highThreshold;

    // ─── CONTROL DE RUIDO ───
    float noiseFloor;     // piso mínimo de energía
    float gateStrength;   // activación global (0–1)

    // Memoria visual
    ofFbo memory;

    float time;
};
