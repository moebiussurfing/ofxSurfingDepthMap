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


private:
	void setupFbo();
	void setupShader();
	void setupParams();
	void doAutoFocus();

public:
	ofParameterGroup params;
	ofParameter<bool> enableDepthMap; // main enable/disable

	// Tweaks parameters
	ofParameterGroup paramsTweaks;
	ofParameter<float> depthContrast;
	ofParameter<float> depthBrightness;
	ofParameter<float> depthGamma;
	ofParameter<bool> invertDepth;
	ofParameter<void> vResetTweaks;

	// Depth mode parameters
	ofParameterGroup paramsDepthMode;
	ofParameter<int> depthMode; // 0=Linear, 1=Log, 2=FocusRange
	ofParameter<string> depthModeName;
	vector<string> depthModeNames;
	ofParameter<float> logCurvePower;
	ofParameter<void> vResetMode;

	// Camera parameters
	ofParameterGroup paramsCamera;
	ofParameter<bool> useManualClipPlanes;
	ofParameter<float> manualNear;
	ofParameter<float> manualFar;
	ofParameter<void> vResetManual;

	// Focus range parameters (for depthMode == 2)
	ofParameterGroup paramsFocus;
	ofParameter<float> focusNear;
	ofParameter<float> focusFar;
	ofParameter<void> vAutoFocus;
	ofParameter<void> vResetFocus;

	ofParameter<void> vResetAll;

	int width, height;

private:
	void updateDepthModeString();

	// Event listeners
	ofEventListener useManualCameraClipPlanesListener;
	ofEventListener manualNearListener;
	ofEventListener manualFarListener;
	ofEventListener depthModeListener;
	ofEventListener vAutoFocusListener;

	ofEventListener vResetAllListener;
	ofEventListener vResetTweaksListener;
	ofEventListener vResetModeListener;
	ofEventListener vResetManualListener;
	ofEventListener vResetFocusListener;

	void doResetTweaks();
	void doResetMode();
	void doResetManual();
	void doResetFocus();

public:
	void doResetAll();

private:
	ofFbo fbo; // single FBO
	ofShader shader; // external file shader
	ofCamera * camera; // pointer to camera
};
