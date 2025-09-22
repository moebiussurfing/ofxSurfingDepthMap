#include "ofApp.h"
#include "ofMain.h"

//========================================================================
int main() {
	// Setup OpenGL context
	ofGLWindowSettings settings;
	settings.setGLVersion(3, 3); // Use OpenGL 3.3 for shader compatibility
	settings.setSize(1024,1024);
	settings.windowMode = OF_WINDOW;

	ofCreateWindow(settings);

	// Start the app
	ofRunApp(new ofApp());
}
