#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	width = 1024;
	height = 1024;
	ofSetWindowShape(width, height);
	int pad = 50;
	ofSetWindowPosition(-width - pad, pad);

	oscillation = 0.0f;
	time = 0.0f;
	hideGui = false;

	dm.setup();

	setupGui();

	cam.setDistance(800);
	cam.setNearClip(nearPlane);
	cam.setFarClip(farPlane);

	ofSetBackgroundAuto(false);

	ofEnableDepthTest();
}

//--------------------------------------------------------------
void ofApp::setupGui() {
	paramsCube.setName("Cube");
	paramsCube.add(cubeSize.set("Cube Size", 200, 50, 500));
	paramsCube.add(cubeAnim.set("Cube Anim", true));

	paramsCube.add(nearPlane.set("Near Plane", 100, 10, 500));
	paramsCube.add(farPlane.set("Far Plane", 2000, 500, 5000));

	gui.setup("Depth Map Controls");
	gui.add(paramsCube);
	gui.add(dm.paramsDepthMap);
}
//--------------------------------------------------------------
void ofApp::update() {
	time += ofGetLastFrameTime();
	oscillation = sin(time) * 100.0f;

	cam.setNearClip(nearPlane);
	cam.setFarClip(farPlane);

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
	ofSetColor(255, 100, 50); // Orange color for normal view
	ofDrawBox(0, 0, 0, cubeSize);

	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::drawGui() {
	ofDisableDepthTest();

	gui.draw();

	ofSetColor(255, 255, 0);
	int infoY = height - 120;
	ofDrawBitmapString("SPACE: Toggle Depth/Color | S: Save | G: Hide GUI | R: Reset", 20, infoY);
	ofDrawBitmapString("Current: " + string(dm.showDepthMap ? "DEPTH MAP" : "COLOR") + " | FPS: " + ofToString(ofGetFrameRate(), 1), 20, infoY + 20);

	if (dm.showDepthMap) {
		ofDrawBitmapString("Ready for ComfyUI ControlNet Depth", 20, infoY + 40);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofClear(0);

	dm.begin(cam);
	{
		cam.begin();
		ofEnableDepthTest();
		renderScene();
		cam.end();
	}
	dm.end(cam);

	ofSetColor(255);
	dm.draw();

	if (!hideGui) drawGui();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case ' ':
		dm.showDepthMap = !dm.showDepthMap;
		break;
	case 's':
		dm.save();
		break;
	case 'g':
		hideGui = !hideGui;
		break;
	case 'f':
		ofToggleFullscreen();
		break;
	case 'r':
		cubeSize = 200;
		nearPlane = 100;
		farPlane = 2000;
		dm.reset();
		break;
	}
}
