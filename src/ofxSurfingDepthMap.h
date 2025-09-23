//ofxSurfingDepthMap.h
#pragma once

#include "ofMain.h"

class ofxSurfingDepthMap {
public:
	ofxSurfingDepthMap();
	~ofxSurfingDepthMap();

	void setup(ofCamera* cam);
	void update();
	void draw(float x = 0, float y = 0, float w = -1, float h = -1);
	void begin();
	void end();
	void save(const std::string & filename = "");
	void reset();

private:
	void setupFbo();
	void setupShader();

public:
	ofParameterGroup params;
	ofParameter<float> depthContrast;
	ofParameter<float> depthBrightness;
	ofParameter<float> depthGamma;
	ofParameter<bool> invertDepth;
	ofParameter<void> resetDepthMapButton;
	ofParameter<bool> enableDepthMap;
	ofParameter<bool> useLogDepth;

	ofParameter<bool> useCameraClipPlanes;
	ofParameter<float> manualNear;
	ofParameter<float> manualFar;
	ofParameterGroup camParams;

	int width, height;

private:
	ofEventListener resetDepthMapButtonListener;


private:
	ofFbo fbo; // single FBO
	ofShader shader; // external file shader
	ofCamera * camera; // pointer to camera

	void setupParams();
};
