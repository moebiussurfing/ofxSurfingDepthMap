#pragma once

#include "ofMain.h"

class ofxSurfingDepthMap : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();

	ofParameterGroup paramsDepthMap;
	ofParameter<float> depthContrast;
	ofParameter<float> depthBrightness;
	ofParameter<float> depthGamma;
	ofParameter<bool> invertDepth;
	ofParameter<void> resetDepthMapButton;

	void reset();

	ofEventListener resetDepthMapButtonListener;
	ofParameter<float> nearPlane;
	ofParameter<float> farPlane;

private:
	void setupFBOs();
	void setupShaders();

public:
	// Toggle between normal and depth view
	bool showDepthMap;

	void begin(ofCamera &cam);
	void end(ofCamera & cam);
	void save();

private:
	ofFbo depthFbo;
	ofFbo colorFbo;

	ofShader depthShader;

	int width, height;

	bool hideGui;
	void setupParams();

};
