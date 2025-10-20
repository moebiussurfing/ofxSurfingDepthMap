// file: src/ofApp.h
#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxSurfingDepthMap.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void drawGui();
	void doResetScene();
	void keyPressed(int key);
	void windowResized(int w, int h);
	void exit();
	void drawInteractionArea();

private:
	ofEasyCam camera;

	ofxPanel gui;

	ofParameterGroup paramsScene;
	ofParameter<float> cubeSize;
	ofParameter<bool> cubeAnim;
	ofParameter<void> vReset;
	ofEventListener e_vReset;
	ofParameter<bool> bGui;
	void setupGui();

	float oscillation;
	float time;
	void drawScene();

	ofxSurfingDepthMap dm;
};
