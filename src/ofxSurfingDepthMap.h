//ofxSurfingDepthMap.h
#pragma once

#include "ofMain.h"
#include "ofxSurfingDepthMapSerializers.h"

#define SURFING_NEAR_MIN 0.01f
#define SURFING_NEAR_MAX 10.0f
#define SURFING_FAR_MIN 10.0f
#define SURFING_FAR_MAX 4000.0f

class ofxSurfingDepthMap {
public:
	ofxSurfingDepthMap();
	~ofxSurfingDepthMap();

	void setup(ofCamera * cam);
	void update();
	void draw(float x = 0, float y = 0, float w = -1, float h = -1);
	void drawViewport();
	void drawCentered();
	void begin();
	void end();
	void save();
	void exit();

private:
	void setupFbo();
	void setupShader();
	void setupParams();
	void doAutoFocus();

public:
	ofParameterGroup params;
	// Group available for ui population and saving/loading presets externally if required.

	ofParameter<bool> enableDepthMap; // main enable/disable

	// Tweaks parameters
	ofParameterGroup paramsDepthTone;
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
	ofParameterGroup paramsCameraClipping;
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
	ofRectangle rectViewport;

	ofParameterGroup paramsExport;
	ofParameter<string> pathFolder;
	void setPathFolder(const std::string & path) { pathFolder = path; }
	ofParameter<void> vChooseFolder;
	ofParameter<void> vExport;
	ofParameter<void> vOpenExportFolder;

	ofParameterGroup paramsSettings; // Group available for session saving/loading

	ofParameter<bool> bGui { "DepthMap", true }; // Useful for external show/hide ui management

private:
	void updateDepthModeString();

	// Event listeners
	ofEventListener depthModeListener;
	ofEventListener vAutoFocusListener;

	ofEventListener vResetAllListener;
	ofEventListener vResetTweaksListener;
	ofEventListener vResetModeListener;
	ofEventListener vResetManualListener;
	ofEventListener vResetFocusListener;

	ofEventListener vChooseFolderListener;
	ofEventListener vExportListener;
	ofEventListener vOpenExportFolderListener;

	void doResetTweaks();
	void doResetMode();
	void doResetManual();
	void doResetFocus();
	void doChooseFolder();

public:
	void doOpenExportFolder();

	void doResetAll();

private:
	ofFbo fbo; // single FBO
	ofShader shader; // external file shader
	ofCamera * camera; // pointer to camera

	// for blinking viewport
	float Tick(float delay = 1.0f) {
		return fmodf(ofGetElapsedTimeMillis() / 1000.0f, delay) / delay;
	};
	float Bounce(float delay = 1.0f) {
		return 0.5f * (1.0 + glm::sin(Tick(delay) * glm::two_pi<float>()));
	};

	bool bDoneExit = false;
};

//---

// NOTES

//// In case your GPU gets image aliased...
//ofFbo resolved;
//resolved.allocate(fbo.getWidth(), fbo.getHeight(), fbo.getTexture().getTextureData().glInternalFormat);
//fbo.blitTo(resolved);
//resolved.readToPixels(pix);

//// To get full precision depth values
//GL_R32F, readToPixels(ofFloatPixels &) .exr o.tif
