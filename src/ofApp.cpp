#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetWindowTitle("Geometrias en Escucha - Emanuel Bellanti");
    ofSetFrameRate(120);
    ofBackground(0);

    osc.setup(OSC_PORT);

    low = mid = high = 0.0f;
    lowSmooth = midSmooth = highSmooth = 0.0f;

    lowThreshold = 0.06f;
    midThreshold = 0.045f;
    highThreshold = 0.025f;

    // ─── PISO DE RUIDO ───
    noiseFloor = 0.4f;
    gateStrength = 0.0f;

    memory.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    memory.begin();
    ofClear(0, 0, 0, 255);
    memory.end();

    time = 0.0f;
}

//--------------------------------------------------------------
void ofApp::update() {
    processOSC();

    // Suavizado = duración perceptual
    lowSmooth = ofLerp(lowSmooth, low, 0.05f);
    midSmooth = ofLerp(midSmooth, mid, 0.07f);
    highSmooth = ofLerp(highSmooth, high, 0.15f);

    // ─── GATE GLOBAL POR ENERGÍA ───
    float energy = lowSmooth + midSmooth + highSmooth;

    if (energy > noiseFloor) {
        gateStrength = ofLerp(
            gateStrength,
            ofMap(energy, noiseFloor, noiseFloor + 0.3f, 0.0f, 1.0f, true),
            0.08f
        );
    }
    else {
        gateStrength = ofLerp(gateStrength, 0.0f, 0.1f);
    }

    memory.begin();

    // Olvido lento (independiente del gate)
    ofSetColor(0, 0, 0, 12);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

    // Si el sistema no está activo, no genera eventos nuevos
    if (gateStrength > 0.02f) {

        // Cantidad de eventos depende de duración * gate
        int lowCount = ofMap(lowSmooth * gateStrength, 0, 1, 0, 6, true);
        int midCount = ofMap(midSmooth * gateStrength, 0, 1, 0, 5, true);
        int highCount = ofMap(highSmooth * gateStrength, 0, 1, 0, 8, true);

        // ───────── LOW → CÍRCULOS ─────────
        ofSetColor(80, 80, 120, 140);
        for (int i = 0; i < lowCount; i++) {
            float x = ofRandomWidth();
            float y = ofRandomHeight();
            float r = 20 + lowSmooth * 120;
            ofDrawCircle(x, y, r);
        }

        // ───────── MID → CUADRADOS ─────────
        ofSetColor(120, 220, 160, 160);
        for (int i = 0; i < midCount; i++) {
            float x = ofRandomWidth();
            float y = ofRandomHeight();
            float s = 20 + midSmooth * 100;
            ofDrawRectangle(x - s * 0.5f, y - s * 0.5f, s, s);
        }

        // ───────── HIGH → TRIÁNGULOS ─────────
        ofSetColor(220, 220, 255, 200);
        for (int i = 0; i < highCount; i++) {
            float x = ofRandomWidth();
            float y = ofRandomHeight();
            float s = 10 + highSmooth * 60;

            ofBeginShape();
            ofVertex(x, y - s);
            ofVertex(x - s, y + s);
            ofVertex(x + s, y + s);
            ofEndShape(true);
        }
    }

    memory.end();

    time += 0.01f;
}

//--------------------------------------------------------------
void ofApp::processOSC() {
    while (osc.hasWaitingMessages()) {
        ofxOscMessage m;
        osc.getNextMessage(m);

        if (m.getNumArgs() == 0) continue;

        float v = 0.0f;

        if (m.getArgType(0) == OFXOSC_TYPE_FLOAT)
            v = m.getArgAsFloat(0);
        else if (m.getArgType(0) == OFXOSC_TYPE_DOUBLE)
            v = (float)m.getArgAsDouble(0);
        else
            continue;

        v = ofClamp(v, 0.0f, 1.0f);

        if (m.getAddress() == "/track/2/vu")
            low = (v > lowThreshold) ? v : 0.0f;
        else if (m.getAddress() == "/track/3/vu")
            mid = (v > midThreshold) ? v : 0.0f;
        else if (m.getAddress() == "/track/4/vu")
            high = (v > highThreshold) ? v : 0.0f;
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofSetColor(255);
    memory.draw(0, 0);
}
