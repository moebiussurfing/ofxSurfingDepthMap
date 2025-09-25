//ofxSurfingDepthMap.cpp
#include "ofxSurfingDepthMap.h"

//--------------------------------------------------------------
ofxSurfingDepthMap::ofxSurfingDepthMap() {
}

//--------------------------------------------------------------
ofxSurfingDepthMap::~ofxSurfingDepthMap() {
	if (!bDoneExit) exit(); //force exit to save settings if not done manually
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
	rectViewport = ofRectangle(0, 0, width, height);

	setupParams();
	setupFbo();
	setupShader();

	ofxSurfingDepthMapSerializers::loadSettings(params);
	ofxSurfingDepthMapSerializers::loadSettings(paramsSettings);
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::setupParams() {
	params.setName("DepthMap");
	params.add(enableDepthMap.set("Enable", true));

	paramsDepthTone.setName("Depth Tone");
	paramsDepthTone.add(depthContrast.set("Contrast", 1.0, 0.1, 3.0));
	paramsDepthTone.add(depthBrightness.set("Brightness", 0.0, -0.5, 0.5));
	paramsDepthTone.add(depthGamma.set("Gamma", 1.0, 0.5, 2.5));
	paramsDepthTone.add(invertDepth.set("Invert", false));
	paramsDepthTone.add(vResetTweaks.set("Reset"));
	params.add(paramsDepthTone);

	// Depth mode parameters
	depthModeNames = { "Linear", "Logarithmic", "Focus Range" };
	paramsDepthMode.setName("Modes");
	paramsDepthMode.add(depthMode.set("Depth Mode", 0, 0, 2));
	paramsDepthMode.add(depthModeName.set("Mode", "-1"));
	paramsDepthMode.add(logCurvePower.set("Log Power", 1.0f, 0.05f, 10.0f));
	paramsDepthMode.add(vResetMode.set("Reset"));
	params.add(paramsDepthMode);

	// Camera parameters
	paramsCameraClipping.setName("Camera Clipping");
	paramsCameraClipping.add(useManualClipPlanes.set("Enable", true));
	paramsCameraClipping.add(manualNear.set("Near", 0.1f, SURFING_NEAR_MIN, SURFING_NEAR_MAX));
	paramsCameraClipping.add(manualFar.set("Far", 1500.0f, SURFING_FAR_MIN, SURFING_FAR_MAX));
	paramsCameraClipping.add(vResetManual.set("Reset"));
	params.add(paramsCameraClipping);

	// Focus range parameters (only active when depthMode == 2)
	paramsFocus.setName("Focus Range");
	paramsFocus.add(focusNear.set("Near", 0.1f, SURFING_NEAR_MIN, SURFING_FAR_MAX));
	paramsFocus.add(focusFar.set("Far", 500.0f, SURFING_FAR_MIN, SURFING_FAR_MAX));
	paramsFocus.add(vAutoFocus.set("Auto Focus"));
	paramsFocus.add(vResetFocus.set("Reset"));
	params.add(paramsFocus);

	params.add(vResetAll.set("Reset"));

	paramsExport.setName("Export PNG");
	paramsExport.add(vChooseFolder.set("Set Folder"));
	paramsExport.add(pathFolder.set("Folder", ""));
	paramsExport.add(vOpenExportFolder.set("Open Folder"));
	paramsExport.add(vExport.set("Export"));
	params.add(paramsExport);

	paramsSettings.setName("ofxSurfingDepthMap");
	paramsSettings.add(bGui);
	//paramsSettings.add(params);//TODO crashes..

	depthModeListener = depthMode.newListener([this](int & val) {
		updateDepthModeString();
	});

	updateDepthModeString();

	vAutoFocusListener = vAutoFocus.newListener([this](const void * sender) {
		doAutoFocus();
	});

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

	vChooseFolderListener = vChooseFolder.newListener([this](const void * sender) {
		doChooseFolder();
	});

	vOpenExportFolderListener = vOpenExportFolder.newListener([this](const void * sender) {
		doOpenExportFolder();
	});

	vExportListener = vExport.newListener([this](const void * sender) {
		save();
	});
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::setupFbo() {
	ofFboSettings s;
	s.width = width;
	s.height = height;

	s.internalformat = GL_RGBA; // RGB 8 bits ready for comfyui
	//s.internalformat = GL_R32F; // R 32 bits float (not comfyui ready)

	s.useDepth = true; // we want depth testing in the scene
	s.useStencil = false;

#if 1
	// Enable multisample (antialiasing)
	s.numSamples = 16; // try 4, 8, or 16 depending on GPU support

	// Optional: make sure texture filtering is smooth
	s.minFilter = GL_LINEAR;
	s.maxFilter = GL_LINEAR;
#endif

	// allocate
	fbo.allocate(s);
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::setupShader() {
	ofLogNotice("ofxSurfingDepthMap") << "OpenGL Version: " << glGetString(GL_VERSION);

	// Detectar si podemos usar GL3 basado en la versión real
	std::string glVersion = (char *)glGetString(GL_VERSION);
	bool useGL3 = (glVersion.find("3.") == 0 || glVersion.find("4.") == 0);

	bool ok = false;
	if (useGL3) {
		ok = shader.load("shadersGL3/depth.vert", "shadersGL3/depth.frag");
		ofLogNotice() << "Using GL3 shaders";
	} else { //TODO
		ok = shader.load("shaders/depth.vert", "shaders/depth.frag");
		ofLogNotice() << "Using GL2 shaders";
	}

	if (!ok) {
		ofLogError("ofxSurfingDepthMap") << "Failed to load shaders";
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
	// the geometry was already shaded to gray scale depth values.
	// If disabled, the FBO contains the normal colored render.
	fbo.draw(x, y, w, h);
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::drawCentered() {
	draw(ofGetWidth() / 2 - width / 2, ofGetHeight() / 2 - height / 2, width, height);
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::drawViewport() {
	ofSetColor(255);
	int x = ofGetWidth() / 2 - width / 2;
	int y = ofGetHeight() / 2 - height / 2;
	rectViewport = ofRectangle(x, y, width, height);

	fbo.draw(rectViewport.getX(), rectViewport.getY(), rectViewport.getWidth(), rectViewport.getHeight());

	// view rect
	if (bGui) {
		ofPushStyle();
		ofNoFill();
		ofSetLineWidth(2);
		//ofColor c = ofColor::yellow;
		ofColor c = ofColor(128);
		ofSetColor(ofColor(c.r, c.g, c.b, 0.25 * c.a * Bounce()));
		ofDrawRectangle(rectViewport);
		ofPopStyle();
	}
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
void ofxSurfingDepthMap::save() {
	ofPixels pix;
	fbo.readToPixels(pix);

	// Build filename
	std::string filename = "depthmap_" + ofToString(ofGetTimestampString()) + ".png";
	std::string out;
	if (pathFolder.get() == "")
		out = ofFilePath::join(ofToDataPath("", true), filename);
	else
		out = ofFilePath::join(pathFolder.get(), filename);

	// Save image
	ofSaveImage(pix, out);
	ofLogNotice("ofxSurfingDepthMap") << "Saved depth-map png image to " << out;

	// Get absolute path
	std::string path_ = ofFilePath::getAbsolutePath(out);

	ofLogNotice("ofxSurfingDepthMap") << "Open saved image from " << path_;

	// Wrap path in quotes to handle spaces
	std::string quotedPath = "\"" + path_ + "\"";

#ifdef TARGET_OSX
	ofSystem("open " + quotedPath);
#elif defined(TARGET_WIN32)
	// Windows start needs empty title before path
	ofSystem("start \"\" " + quotedPath);
#elif defined(TARGET_LINUX)
	ofSystem("xdg-open " + quotedPath);
#endif
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::doOpenExportFolder() {
	std::string folderStr;

	// Get folder path from parameter or default data folder
	if (pathFolder.get() != "") {
		folderStr = pathFolder.get();
	} else {
		folderStr = ofToDataPath("", true);
	}

	// Use ofFile to ensure proper folder path
	ofFile folder(folderStr);
	if (!folder.isDirectory()) {
		folderStr = folder.getEnclosingDirectory(); // get parent if it's a file
	}
	folderStr = folder.getAbsolutePath(); // ensures absolute path with correct slashes

	ofLogNotice("ofxSurfingDepthMap") << "doOpenExportFolder() " << folderStr;

#ifdef TARGET_OSX
	std::string command = "open \"" + folderStr + "\"";
	system(command.c_str());
#elif defined(TARGET_LINUX)
	std::string command = "xdg-open \"" + folderStr + "\"";
	system(command.c_str());
#elif defined(_WIN32)
	// More reliable on Windows using system("start") instead of ShellExecute
	std::string command = "start \"\" \"" + folderStr + "\"";
	system(command.c_str());
#endif
}

// --------------------------------------------------------------
void ofxSurfingDepthMap::doChooseFolder() {
	// Open system dialog to choose a folder
	ofFileDialogResult result = ofSystemLoadDialog("Select output folder", true); // true = folder mode

	if (result.bSuccess) {
		pathFolder = result.getPath(); // Save absolute path
		ofLogNotice("ofxSurfingDepthMap") << "Selected output folder: " << pathFolder.get();
	}
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
		depthModeName = "-1"; //error
	}
}

// --------------------------------------------------------------
void ofxSurfingDepthMap::exit() {
	ofxSurfingDepthMapSerializers::saveSettings(params);
	ofxSurfingDepthMapSerializers::saveSettings(paramsSettings);

	bDoneExit = true;
}
