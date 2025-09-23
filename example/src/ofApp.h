//ofApp.h
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

private:
	ofEasyCam camera;

	ofxPanel gui;

	ofParameterGroup paramsCube;
	ofParameter<float> cubeSize;
	ofParameter<bool> cubeAnim;

	ofParameter<void> vReset;
	ofEventListener vResetListener;

	float oscillation;
	float time;

	int width, height;

	ofParameter<bool> bGui;
	void setupGui();

	void drawScene();

	ofxSurfingDepthMap dm;
};
