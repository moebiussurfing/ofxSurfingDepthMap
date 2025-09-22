#include "ofxSurfingDepthMap.h"

//--------------------------------------------------------------
void ofxSurfingDepthMap::setup() {
	setupParams();
	setupFBOs();
	setupShaders();

	width = 1024;
	height = 1024;
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::setupParams() {
	paramsDepthMap.setName("DepthMap");
	paramsDepthMap.add(depthContrast.set("Depth Contrast", 1.0, 0.1, 3.0));
	paramsDepthMap.add(depthBrightness.set("Depth Brightness", 0.0, -0.5, 0.5));
	paramsDepthMap.add(depthGamma.set("Depth Gamma", 1.0, 0.5, 2.5));
	paramsDepthMap.add(invertDepth.set("Invert Depth", false));
	paramsDepthMap.add(resetDepthMapButton.set("Reset DepthMap"));

	resetDepthMapButtonListener = resetDepthMapButton.newListener([this](const void * sender) {
		reset();
	});
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::setupFBOs() {
	ofFboSettings settings;
	settings.width = width;
	settings.height = height;
	settings.useDepth = true;
	settings.internalformat = GL_RGBA;
	settings.numSamples = 0;

	depthFbo.allocate(settings);
	colorFbo.allocate(settings);
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::setupShaders() {
	// Simple depth rendering shader
	string depthVertShader = R"(
        #version 330
        
        uniform mat4 modelViewProjectionMatrix;
        uniform mat4 modelViewMatrix;
        
        in vec4 position;
        out float viewZ;
        
        void main(){
            vec4 viewPos = modelViewMatrix * position;
            viewZ = -viewPos.z; // Store view space Z (positive = distance from camera)
            gl_Position = modelViewProjectionMatrix * position;
        }
    )";

	string depthFragShader = R"(
        #version 330
        
        uniform float nearPlane;
        uniform float farPlane;
        uniform float depthContrast;
        uniform float depthBrightness;
        uniform float depthGamma;
        uniform int invertDepth;
        
        in float viewZ;
        out vec4 fragColor;
        
        void main(){
            // Normalize depth from near to far plane
            float normalizedDepth = (viewZ - nearPlane) / (farPlane - nearPlane);
            normalizedDepth = clamp(normalizedDepth, 0.0, 1.0);
            
            // Apply contrast (safe - maintains spatial relationships)
            normalizedDepth = ((normalizedDepth - 0.5) * depthContrast) + 0.5;
            normalizedDepth = clamp(normalizedDepth, 0.0, 1.0);
            
            // Apply gamma correction (safe - improves gradients)
            normalizedDepth = pow(normalizedDepth, 1.0 / depthGamma);
            
            // Apply brightness offset (safe - shifts range)
            normalizedDepth = clamp(normalizedDepth + depthBrightness, 0.0, 1.0);
            
            // Invert based on preference
            float depthValue = (invertDepth == 1) ? normalizedDepth : (1.0 - normalizedDepth);
            
            fragColor = vec4(vec3(depthValue), 1.0);
        }
    )";

	depthShader.setupShaderFromSource(GL_VERTEX_SHADER, depthVertShader);
	depthShader.setupShaderFromSource(GL_FRAGMENT_SHADER, depthFragShader);
	depthShader.linkProgram();
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::update() {
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::begin(ofCamera & cam) {
	ofClear(0);

	depthFbo.begin();
	ofClear(0);

	ofEnableDepthTest();

	cam.begin();

	depthShader.begin();
	// Pass camera parameters to shader
	depthShader.setUniform1f("nearPlane", nearPlane);
	depthShader.setUniform1f("farPlane", farPlane);

	// Pass depth mapping parameters
	depthShader.setUniform1f("depthContrast", depthContrast);
	depthShader.setUniform1f("depthBrightness", depthBrightness);
	depthShader.setUniform1f("depthGamma", depthGamma);
	depthShader.setUniform1i("invertDepth", invertDepth ? 1 : 0);

	//renderScene(true);
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::end(ofCamera & cam) {

	depthShader.end();
	cam.end();

	depthFbo.end();

	//// 2. Render normal color pass
	//if (!showDepthMap) {
	//	colorFbo.begin();
	//	ofClear(50, 50, 50); // Dark gray background
	//	cam.begin();
	//	renderScene(false);
	//	cam.end();
	//	colorFbo.end();
	//}
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::draw() {

	ofSetColor(255);

	depthFbo.draw(0, 0);

	//if (showDepthMap) {
	//	depthFbo.draw(0, 0);
	//} else {
	//	colorFbo.draw(0, 0);
	//}
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::reset() {
	// Valores que NO añaden distorsión a distancias ni grises
	depthContrast = 1.0; // Sin modificar contraste (lineal)
	depthBrightness = 0.0; // Sin offset de brillo
	depthGamma = 1.0; // Sin corrección gamma (lineal)
	invertDepth = false; // Convención estándar: blanco=cerca, negro=lejos

	ofLogNotice() << "DepthMap parameters reset to neutral values (no distortion)";
}

//--------------------------------------------------------------
void ofxSurfingDepthMap::save() {
	// Save depth map
	if (showDepthMap) {
		ofPixels pixels;
		depthFbo.readToPixels(pixels);
		string filename = "depth_map_" + ofToString(ofGetTimestampString()) + ".png";
		ofSaveImage(pixels, filename);
		ofLogNotice() << "Depth map saved: " << filename;
	}
}
