//ofxSurfingDepthMap.cpp
#include "ofxSurfingDepthMap.h"

//--------------------------------------------------------------
ofxSurfingDepthMap::ofxSurfingDepthMap() {
}

//--------------------------------------------------------------
ofxSurfingDepthMap::~ofxSurfingDepthMap() {
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::setup(ofCamera * cam) {
	if (cam != nullptr) {
		camera = cam;
	} else {
		ofLogError("ofxSurfingDepthMap") << "Failed to load ofCamera";
	}

	width = 1024;
	height = 1024;

	setupParams();
	setupFbo();
	setupShader();
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::setupParams() {
	params.setName("DepthMap");
	params.add(enableDepthMap.set("Enable", true));

	paramsTweaks.setName("FX Tweaks");
	paramsTweaks.add(depthContrast.set("Contrast", 1.0, 0.1, 3.0));
	paramsTweaks.add(depthBrightness.set("Brightness", 0.0, -0.5, 0.5));
	paramsTweaks.add(depthGamma.set("Gamma", 1.0, 0.5, 2.5));
	paramsTweaks.add(invertDepth.set("Invert", false));
	paramsTweaks.add(vResetTweaks.set("Reset"));
	params.add(paramsTweaks);

	// Depth mode parameters
	depthModeNames = { "Linear", "Logarithmic", "Focus Range" };
	paramsDepthMode.setName("Mode");
	paramsDepthMode.add(depthMode.set("Depth Mode", 0, 0, 2));
	paramsDepthMode.add(depthModeName.set("-1"));
	paramsDepthMode.add(logCurvePower.set("Log Power", 1.0f, 0.05f, 10.0f));
	paramsDepthMode.add(vResetMode.set("Reset"));
	params.add(paramsDepthMode);

	// Camera parameters
	paramsCamera.setName("Camera Clips");
	paramsCamera.add(useManualClipPlanes.set("Enable", true));
	paramsCamera.add(manualNear.set("Near", 0.1f, 0.01f, 10.0f));
	paramsCamera.add(manualFar.set("Far", 1500.0f, 10.0f, 4000.0f));
	paramsCamera.add(vResetManual.set("Reset"));
	params.add(paramsCamera);

	// Focus range parameters (only active when depthMode == 2)
	paramsFocus.setName("Focus Range");
	paramsFocus.add(focusNear.set("Near", 0.1f, 0.01f, 10000.0f));
	paramsFocus.add(focusFar.set("Far", 500.0f, 10.0f, 10000.0f));
	paramsFocus.add(vAutoFocus.set("Auto Focus"));
	paramsFocus.add(vResetFocus.set("Reset"));
	params.add(paramsFocus);

	params.add(vResetAll.set("Reset"));

	// Listener for updating focus parameters when camera or manual values change
	useManualCameraClipPlanesListener = useManualClipPlanes.newListener([this](bool & val) {
	});

	manualNearListener = manualNear.newListener([this](float & val) {
	});

	manualFarListener = manualFar.newListener([this](float & val) {
	});

	depthModeListener = depthMode.newListener([this](int & val) {
		updateDepthModeString();
	});

	vAutoFocusListener = vAutoFocus.newListener([this](const void * sender) {
		doAutoFocus();
	});

	updateDepthModeString();

	// Resets

	vResetTweaksListener = vResetTweaks.newListener([this](const void * sender) {
		doResetTweaks();
	});

	vResetModeListener = vResetMode.newListener([this](const void * sender) {
		doResetMode();
	});

	vResetFocusListener = vResetFocus.newListener([this](const void * sender) {
		doResetFocus();
	});

	vResetManualListener = vResetManual.newListener([this](const void * sender) {
		doResetManual();
	});

	vResetAllListener = vResetAll.newListener([this](const void * sender) {
		doResetAll();
	});
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::setupFbo() {
	ofFboSettings s;
	s.width = width;
	s.height = height;
	s.internalformat = GL_RGBA; // RGB 8 bits ready for comfyui
	//s.internalformat = GL_R32F;
	s.useDepth = true; // we want depth testing in the scene
	s.useStencil = false;
	s.numSamples = 0;

	// allocate
	fbo.allocate(s);
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::setupShader() {
	ofLogNotice("ofxSurfingDepthMap") << "setupShader()";

	// Shader must be present in bin/data/shadersGL3/
	// It is used during geometry rendering when enableDepthMap == true.
	// The vertex shader should output view-space z as a varying,
	// and the fragment shader should compute normalized depth and optionally apply contrast/gamma/brightness/invert.
	bool ok = shader.load("shadersGL3/depth.vert", "shadersGL3/depth.frag");
	if (!ok) {
		ofLogError("ofxSurfingDepthMap") << "Failed to load shaders from shadersGL3/";
	}
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::update() {
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::doAutoFocus() {
	ofLogNotice("ofxSurfingDepthMap") << "doAutoFocus()";

	float near_ = manualNear.get();
	float far_ = manualFar.get();

	// Auto-adjust focus range to be in the middle third of the depth range
	float range_ = far_ - near_;
	float centerPoint_ = near_ + range_ * 0.5f;
	float focusWidth_ = range_ * 0.3f;

	// Only update if values seem reasonable and user hasn't manually adjusted
	if (range_ > 0 && focusWidth_ > 0) {
		focusNear.set(centerPoint_ - focusWidth_ * 0.5f);
		focusFar.set(centerPoint_ + focusWidth_ * 0.5f);
		//focusRange.set(focusWidth_);
	}
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::begin() {
	fbo.begin();
	ofClear(0, 0, 0, 0);
	ofEnableDepthTest();

	// If depth effect is enabled, bind shader BEFORE the user draws geometry.
	// The shader will receive model/view/projection matrices from OF automatically,
	// and we will set uniforms from params so the shader can map depth -> grayscale.
	if (enableDepthMap && shader.isLoaded()) {
		shader.begin();

		// Set clip planes
		if (!useManualClipPlanes) {
			shader.setUniform1f("nearClip", camera->getNearClip());
			shader.setUniform1f("farClip", camera->getFarClip());
		} else {
			shader.setUniform1f("nearClip", manualNear);
			shader.setUniform1f("farClip", manualFar);
		}

		// Set basic depth parameters
		shader.setUniform1f("depthContrast", depthContrast);
		shader.setUniform1f("depthBrightness", depthBrightness);
		shader.setUniform1f("depthGamma", depthGamma);
		shader.setUniform1i("invertDepth", invertDepth ? 1 : 0);

		// Set depth mode and related parameters
		shader.setUniform1i("depthMode", depthMode);
		shader.setUniform1f("logCurvePower", logCurvePower);

		// Set focus range parameters (used only when depthMode == 2)
		shader.setUniform1f("focusNear", focusNear);
		shader.setUniform1f("focusFar", focusFar);
	}
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::end() {
	// If shader was bound, unbind it now (so subsequent draws won't be affected)
	if (enableDepthMap && shader.isLoaded()) {
		shader.end();
	}
	ofDisableDepthTest();
	fbo.end();
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::draw(float x, float y, float w, float h) {
	ofSetColor(255);

	if (w <= 0) w = width;
	if (h <= 0) h = height;

	// We rendered into the FBO. If depth effect was enabled,
	// the geometry was already shaded to grayscale depth values.
	// If disabled, the FBO contains the normal colored render.
	fbo.draw(x, y, w, h);
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::doResetTweaks() {
	ofLogNotice() << "doResetTweaks()";

	// Reset to neutral tweak values
	depthContrast = 1.0;
	depthBrightness = 0.0;
	depthGamma = 1.0;
	invertDepth = false;
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::doResetManual() {
	ofLogNotice() << "doResetManual()";

	// Reset manual clip planes to reasonable defaults
	manualNear = 0.1f;
	manualFar = 1500.0f;
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::doResetMode() {
	ofLogNotice() << "doResetMode()";

	// Set default linear mode
	depthMode = 0; // Linear mode
	useManualClipPlanes = true;
	logCurvePower = 1.0f;
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::doResetFocus() {
	ofLogNotice() << "doResetFocus()";

	// Reset focus to reasonable defaults
	focusNear = 0.1f;
	focusFar = 1500.0f;
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::doResetAll() {
	ofLogNotice() << "doResetAll()";

	// Reset all
	doResetTweaks();
	doResetManual();
	doResetMode();
}

// --------------------------------------------------------------
void ofxSurfingDepthMap::save(const std::string & filename) {
	ofPixels pix;
	fbo.readToPixels(pix);

	// GL_R32F, readToPixels(ofFloatPixels &) .exr o.tif

	string out;
	if (filename.empty()) {
		out = "depthmap_" + ofToString(ofGetTimestampString()) + ".png";
	} else {
		out = filename;
	}

	ofSaveImage(pix, out);
	ofLogNotice("ofxSurfingDepthMap") << "Saved buffer to " << out;
}

// --------------------------------------------------------------
void ofxSurfingDepthMap::updateDepthModeString() {
	if (depthMode == 0) { // 0=Linear,
		depthModeName = depthModeNames[0];
	} else if (depthMode == 1) { // 0=Log,
		depthModeName = depthModeNames[1];
	} else if (depthMode == 2) { // 0=FocusRange,
		depthModeName = depthModeNames[2];
	} else {
		depthModeName = "-1";//error
	}
}
