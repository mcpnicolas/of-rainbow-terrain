#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
        ofMesh getThickLines(vector<ofPoint> verts,float w,float alpha);
    
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    ofxPanel gui;
    ofParameter<float> waveWeight, mtnWeight;
    ofParameter<int> r, g, b;
    ofParameter<ofColor> color;
    
    ofColor *colorData;
    float *signedNoiseDataWave;
    float *signedNoiseDataMtn;
    float *summedNoiseData;
    int nSignedNoiseData;
    int nColorData;
    
    float defaultFreqWave, defaultFreqMtn;
    float yFactor;
    int nLines;
    
    vector<ofPoint> shiftPts;
    vector<ofPoint> scalePts;
    vector<ofMesh> meshes;

    
};
