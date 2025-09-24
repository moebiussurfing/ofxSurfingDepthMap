#include "ofApp.h"
#include "ofMain.h"

//========================================================================
int main() {
	// Setup OpenGL context
	ofGLWindowSettings settings;

	settings.setGLVersion(3, 3); // Use OpenGL 3.3
	//settings.setGLVersion(2, 1); 

	settings.setSize(1024, 1024);
	settings.windowMode = OF_WINDOW;

	ofCreateWindow(settings);

	// Start the app
	ofRunApp(new ofApp());
}

//// TODO: Use OpenGL 2.1 / GLSL 120
////========================================================================
//int main() {
//	ofSetupOpenGL(1024, 1024, OF_WINDOW); // <-------- setup the GL context
//
//	// this kicks off the running of my app
//	// can be OF_WINDOW or OF_FULLSCREEN
//	// pass in width and height too:
//	ofRunApp(new ofApp());
//}
