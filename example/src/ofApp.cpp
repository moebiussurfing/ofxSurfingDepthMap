//ofApp.cpp
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	width = 1280;
	height = width * (9 / 16.f);
	ofSetWindowShape(width, height);
	int pad = 50;
	ofSetWindowPosition(-width - pad, pad);

	oscillation = 0.0f;
	time = 0.0f;
	bGui.set("Gui", true);

	resetScene();

	dm.setup(&camera);

	setupGui();

	ofEnableDepthTest();

	vResetListener = vReset.newListener([this](const void * sender) {
		resetScene();
	});
}

//--------------------------------------------------------------
void ofApp::setupGui() {
	paramsCube.setName("Cube Scene");
	paramsCube.add(cubeSize.set("Cube Size", 200, 50, 500));
	paramsCube.add(cubeAnim.set("Cube Anim", true));
	paramsCube.add(vReset);

	gui.setup("Example");
	gui.add(paramsCube);
	gui.add(dm.params);
}
//--------------------------------------------------------------
void ofApp::update() {
	time += ofGetLastFrameTime();
	oscillation = sin(time) * 100.0f;

	dm.update();
}

//--------------------------------------------------------------
void ofApp::renderScene() {
	ofPushMatrix();

	if (cubeAnim) {
		ofTranslate(oscillation, oscillation * 0.5f, 0);
		ofRotateYDeg(time * 20);
		ofRotateXDeg(time * 15);
	}

	ofFill();
	ofSetColor(ofColor::darkBlue);
	ofDrawBox(0, 0, 0, cubeSize);

	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	ofDisableDepthTest();

	gui.draw();

	ofSetColor(128);
	int infoY = ofGetHeight() - 20 * 3;
	ofDrawBitmapString("SPACE: Toggle Depth/Color | S: Save | G: Hide GUI | R: Reset", 20, infoY);
	ofDrawBitmapString("Current: " + string(dm.enableDepthMap ? "DEPTH MAP" : "COLOR") + " | FPS: " + ofToString(ofGetFrameRate(), 1), 20, infoY + 20);

	if (dm.enableDepthMap) {
		ofDrawBitmapString("Ready for ComfyUI ControlNet Depth", 20, infoY + 40);
	}
}

//--------------------------------------------------------------
void ofApp::resetScene() {
	cubeSize = 200;

	camera.setupPerspective();
	camera.setDistance(800);
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofClear(0);

	// Start recording into the addon's FBO. The addon will bind the depth shader
	// only if enableDepthMap is true.
	dm.begin();

	// The app still controls when the camera begins/ends.
	camera.begin();
	renderScene();
	camera.end();

	dm.end();

	dm.draw(0, 0, ofGetWidth(), ofGetHeight());

	if (bGui) drawGui();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case ' ':
		dm.enableDepthMap = !dm.enableDepthMap;
		break;
	case 's':
		dm.save();
		break;
	case 'g':
		bGui = !bGui;
		break;
	case 'f':
		ofToggleFullscreen();
		break;
	case 'r':
		resetScene();
		dm.reset();
		break;
	}
}
