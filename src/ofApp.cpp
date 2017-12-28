#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    ofBackground(255);
    ofSetWindowTitle("Rainbow Terrain");
    ofSetVerticalSync(true);
    
    gui.setup();
    //gui.add(freq01.set("frequency 01", 0.025, 0.005, 0.1));
    //gui.add(amp01.set("amplitude 01", 0.4, 0.00, 1.0));
    gui.add(waveWeight.set("rolling hill", 0.5, 0, 1));
    gui.add(mtnWeight.set("rugged mountain", 0.5, 0, 1));
    gui.setDefaultBackgroundColor(255);
    gui.setDefaultHeaderBackgroundColor(150);
    gui.setDefaultTextColor(0);
    gui.add(color.set("color", ofColor(50,50,50,90), ofColor(0,0), ofColor(255,255)));
    
    nSignedNoiseData = 500; // history of noise
    nColorData = nSignedNoiseData + 2;
    signedNoiseDataWave = new float[nSignedNoiseData];
    signedNoiseDataMtn = new float[nSignedNoiseData];
    summedNoiseData = new float[nSignedNoiseData];
    colorData = new ofColor[nColorData];
    
    for (int i=0; i<nSignedNoiseData; i++){
        signedNoiseDataWave[i] = 0;
        signedNoiseDataMtn[i] = 0;
    }
    
    for (int i=0; i<nColorData; i++){
        colorData[i] = color;
    }
    
    defaultFreqWave = 0.0;
    defaultFreqMtn = 0.0;
    yFactor = 300;
    nLines = 60;
    
    for (int i = 0; i < nLines; i++) {
        shiftPts.push_back(ofPoint(0,ofRandom(10))); // no horizontal shift
        // shiftPts.push_back(ofPoint(ofRandom(-10, 10), ofRandom(10)));
        scalePts.push_back(ofPoint(ofRandom(0.95,1.1),ofRandom(1,1.1)));
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    
    for (int i=(nSignedNoiseData-1); i>0; i--){
        signedNoiseDataWave[i] = signedNoiseDataWave[i-1];
        signedNoiseDataMtn[i] = signedNoiseDataMtn[i-1];
    }
    
    for (int i=(nColorData-1); i>0; i--){
        colorData[i] = colorData[i-1];
    }
    
    colorData[0] = color;
    
    float normalizationFactor = 0;
    normalizationFactor = waveWeight + mtnWeight;
    if (normalizationFactor == 0){
        normalizationFactor = 1.0;
    }
    
    // create jagged data
    float ampMtn = 1 * sin(ofNoise(ofGetElapsedTimef() * 0.25)) * sin(ofRandom(1));
    float freqMtn = 0.015 * cos(ofNoise(ofGetElapsedTimef() * 0.1)) * cos(ofRandom(1));
    
    // create wavy data
    float ampWave = 1 * sin(ofNoise(ofGetElapsedTimef() * 0.25));
    float freqWave = 0.015 * cos(ofNoise(ofGetElapsedTimef() * 0.1)) * cos(ofNoise(1));
    
    signedNoiseDataWave[0] = ampWave * ofSignedNoise(defaultFreqWave);
    defaultFreqWave += freqWave;
    signedNoiseDataMtn[0] = ampMtn * ofSignedNoise(defaultFreqMtn);
    defaultFreqMtn += freqMtn;
    
    // sum jagged and wavy data, populate summedNoiseData
    for (int j=0; j< nSignedNoiseData; j++){
        float sumj = 0;
        
        float valWave = signedNoiseDataWave[j];
        float weightWave = waveWeight;
        
        float valMtn = signedNoiseDataMtn[j];
        float weightMtn = mtnWeight;
        
        sumj = (weightWave * valWave) + (weightMtn * valMtn);
        summedNoiseData[j] = sumj / normalizationFactor;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    gui.draw();
    
    ofPushMatrix();
    
    //ofDrawBitmapString("Time: " + ofToString(ofGetElapsedTimef()), 30, 30);
    //ofDrawBitmapString("Amplitude: " + ofToString(amp01), 25, 30);
    //ofDrawBitmapString("Frequency: " + ofToString(freq01), 25, 60);
    
    int alpha = color->a;
    float startWidth = 0.0001;
    float maxWidth = 1.75;
    float width = startWidth;
    
    ofTranslate(0, ofGetHeight()/3);

    ofPath basePath;
    //basePath.setFilled(false);
    //basePath.setStrokeWidth(startWidth);
    //basePath.setStrokeColor(ofColor(color, alpha));
    
    // ofMesh baseMesh;
    // baseMesh.setMode(OF_PRIMITIVE_LINE_STRIP);
    
    for (int i=0; i<nSignedNoiseData; i++){
        
        float px = ofMap(i, 0, nSignedNoiseData-1, ofGetWidth(), 0);
        float py = 0 - yFactor * summedNoiseData[i];
        basePath.lineTo(ofVec2f(px,py));
        // baseMesh.addColor(colorData[i]);
    }
    
    // basePath.draw();
    
    ofMesh baseMesh = getThickLines(basePath.getOutline()[0].getVertices(), startWidth, alpha);
    baseMesh.draw(); // draw original line
    
    for (int i = 0; i < nLines; i++) { // draw manipulated lines
        alpha += (255 - color->a)/nLines;
        width += (maxWidth - startWidth)/nLines;
        //basePath.setStrokeWidth(width);
        //basePath.setStrokeColor(ofColor(color,alpha));
        basePath.translate(shiftPts[i]);
        basePath.scale(scalePts[i].x, 1);
        
        ofMesh newMesh = getThickLines(basePath.getOutline()[0].getVertices(), width, alpha);
        newMesh.draw();
    }
                         
    ofPopMatrix();

}

// return passed in vertices with desired width as mesh
ofMesh ofApp::getThickLines(vector<ofPoint> verts, float width, float alpha) {
    
    ofMesh mesh;
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
    
    float widthSmooth = width;
    float angleSmooth;
    
    for (int i = 0;  i < verts.size(); i++){
        int me_m_one = i-1;
        int me_p_one = i+1;
        if (me_m_one < 0) me_m_one = 0;
        if (me_p_one ==  verts.size()) me_p_one =  verts.size()-1;
        
        ofPoint diff = verts[me_p_one] - verts[me_m_one];
        float angle = atan2(diff.y, diff.x);
        
        if (i == 0) angleSmooth = angle;
        else { angleSmooth = ofLerpDegrees(angleSmooth, angle, 1.0); }
        
        float dist = diff.length();
        float w = ofMap(dist, 0, 10, 1, width, true);
        
        widthSmooth = 0.9f * widthSmooth + 0.1f * w;
        
        ofPoint offset;
        offset.x = cos(angleSmooth + PI/2) * widthSmooth;
        offset.y = sin(angleSmooth + PI/2) * widthSmooth;
        
        mesh.addVertex(  verts[i] +  offset );
        mesh.addColor(ofColor(colorData[i],alpha));
        mesh.addVertex(  verts[i] -  offset );
        mesh.addColor(ofColor(colorData[i],alpha));
        
    }
    return mesh;
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
    if (key == ' ') {
        ofImage screenshot;
        screenshot.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        screenshot.save("screenshot" + ofToString(int(ofRandom(1000))) + ".png");
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
