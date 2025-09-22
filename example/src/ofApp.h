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
	void keyPressed(int key);

private:
	ofEasyCam cam;

	ofxPanel gui;

	ofParameterGroup paramsCube;
	ofParameter<float> cubeSize;
	ofParameter<bool> cubeAnim;

	ofParameter<float> nearPlane;
	ofParameter<float> farPlane;

	float oscillation;
	float time;

	int width, height;

	bool hideGui;
	void setupGui();

	void renderScene();

	ofxSurfingDepthMap dm;
};
