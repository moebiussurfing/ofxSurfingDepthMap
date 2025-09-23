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

	doResetScene();

	dm.setup(&camera);

	setupGui();

	ofEnableDepthTest();

	vResetListener = vReset.newListener([this](const void * sender) {
		doResetScene();
	});
}

//--------------------------------------------------------------
void ofApp::setupGui() {
	paramsCube.setName("3D Scene");
	paramsCube.add(cubeSize.set("Size", 200, 50, 500));
	paramsCube.add(cubeAnim.set("Anim", false));
	paramsCube.add(vReset.set("Reset"));

	gui.setup("Example");
	gui.add(paramsCube);
	gui.add(dm.params);

	//gui.getGroup(paramsCube.getName()).minimize();
	gui.minimizeAll();
}

//--------------------------------------------------------------
void ofApp::update() {
	time += ofGetLastFrameTime();
	oscillation = sin(time) * 50.0f;

	dm.update();
}

//--------------------------------------------------------------
void ofApp::drawScene() {
	ofPushMatrix();

	if (cubeAnim) {
		ofTranslate(oscillation, oscillation * 0.5f, 0);
		ofRotateYDeg(time * 20);
		//ofRotateXDeg(time * 15);
	}

	ofFill();
	ofSetColor(ofColor::darkBlue);
	{
		ofPushMatrix();
		ofRotateYDeg(45);			
		ofDrawBox(0, 0, 0, cubeSize);
		ofPopMatrix();

		float o = cubeSize * 0.75f;
		ofDrawBox(0 - o, 0, 0 - o, cubeSize);
		ofDrawBox(0 + o, 0, 0 - o, cubeSize);

		float sz = cubeSize * 0.5f;
		ofDrawCone(0, 0 - cubeSize, 0 - o, sz * 1.25, cubeSize);
	}
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
void ofApp::doResetScene() {
	cubeSize = 200;

	camera.setDistance(800);
	camera.setupPerspective();
	camera.lookAt(glm::vec3(0, 0, 0));
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofClear(48);

	// Start recording into the addon's FBO. The addon will bind the depth shader
	// only if enableDepthMap is true.
	dm.begin();

	// The app still controls when the camera begins/ends.
	camera.begin();
	drawScene();
	camera.end();

	dm.end();

	//dm.draw(0, 0, ofGetWidth(), ofGetHeight());
	dm.draw(ofGetWidth() / 2 - dm.width / 2, ofGetHeight() / 2 - dm.height / 2, dm.width, dm.height);

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
		doResetScene();
		dm.doResetAll();
		break;
	}
}
