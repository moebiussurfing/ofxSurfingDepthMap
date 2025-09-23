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
	params.add(enableDepthMap.set("Enable", false));
	params.add(depthContrast.set("Contrast", 1.0, 0.1, 3.0));
	params.add(depthBrightness.set("Brightness", 0.0, -0.5, 0.5));
	params.add(depthGamma.set("Gamma", 1.0, 0.5, 2.5));
	params.add(invertDepth.set("Invert", false));
	params.add(resetDepthMapButton.set("Reset"));

	camParams.setName("Camera");
	camParams.add(useCameraClipPlanes.set("Use Camera", true));
	camParams.add(manualNear.set("Manual Near", 0.1f, 0.01f, 10.0f));
	camParams.add(manualFar.set("Manual Far", 2000.0f, 10.0f, 10000.0f));
	camParams.add(useLogDepth.set("Log Depth", false));

	params.add(camParams);

	resetDepthMapButtonListener = resetDepthMapButton.newListener([this](const void * sender) {
		reset();
	});
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::setupFbo() {
	ofFboSettings s;
	s.width = width;
	s.height = height;
	s.internalformat = GL_RGBA;// RGB 8 bits ready for comfyui
	//s.internalformat = GL_R32F;
	s.useDepth = true; // we want depth testing in the scene
	s.useStencil = false;
	s.numSamples = 0;

	// allocate or re-allocate
	fbo.allocate(s);
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::setupShader() {
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
void ofxSurfingDepthMap::begin() {
	if (!camera) return;

	fbo.begin();
	ofClear(0, 0, 0, 255);
	ofEnableDepthTest();
	// If depth effect is enabled, bind shader BEFORE the user draws geometry.
	// The shader will receive model/view/projection matrices from OF automatically,
	// and we will set uniforms from params so the shader can map depth -> grayscale.
	if (enableDepthMap && shader.isLoaded()) {
		shader.begin();
		shader.setUniform1f("nearPlane", camera->getNearClip());
		shader.setUniform1f("farPlane", camera->getFarClip());
		shader.setUniform1f("depthContrast", depthContrast);
		shader.setUniform1f("depthBrightness", depthBrightness);
		shader.setUniform1f("depthGamma", depthGamma);
		shader.setUniform1i("invertDepth", invertDepth ? 1 : 0);
		shader.setUniform1i("useLogDepth", useLogDepth ? 1 : 0);

		if (useCameraClipPlanes) {
			shader.setUniform1f("nearClip", camera->getNearClip());
			shader.setUniform1f("farClip", camera->getFarClip());
		} else {
			shader.setUniform1f("nearClip", manualNear);
			shader.setUniform1f("farClip", manualFar);
		}
	}
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::end() {
	if (!camera) return;

	// If shader was bound, unbind it now (so subsequent draws won't be affected)
	if (enableDepthMap && shader.isLoaded()) {
		shader.end();
	}
	ofDisableDepthTest();
	fbo.end();
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::draw(float x , float y, float w , float h) {
	ofSetColor(255);

	if (w <= 0) w = width;
	if (h <= 0) h = height;

	// We rendered into the FBO. If depth effect was enabled,
	// the geometry was already shaded to grayscale depth values.
	// If disabled, the FBO contains the normal colored render.
	fbo.draw(x, y, w, h);
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::reset() {
	// reset to neutral values that don't distort distance mapping
	depthContrast = 1.0;
	depthBrightness = 0.0;
	depthGamma = 1.0;
	invertDepth = false;

	//camera->setupPerspective();

	ofLogNotice() << "DepthMap parameters reset to neutral values (no distortion)";
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
