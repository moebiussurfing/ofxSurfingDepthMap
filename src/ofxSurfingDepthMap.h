//ofxSurfingDepthMap.h
#pragma once

#include "ofMain.h"

class ofxSurfingDepthMap {
public:
	ofxSurfingDepthMap();
	~ofxSurfingDepthMap();

	void setup(ofCamera* cam);
	void update();
	void draw(float x = 0, float y = 0, float w = -1, float h = -1);
	void begin();
	void end();
	void save(const std::string & filename = "");


private:
	void setupFbo();
	void setupShader();
	void setupParams();
	void updateFocusDefaults();

public:
	ofParameterGroup params;
	ofParameter<bool> enableDepthMap;

	ofParameterGroup paramsTweaks;
	ofParameter<float> depthContrast;
	ofParameter<float> depthBrightness;
	ofParameter<float> depthGamma;
	ofParameter<bool> invertDepth;
	ofParameter<void> vResetTweaks;

	// Depth mode parameters
	ofParameterGroup paramsDepthMode;
	ofParameter<int> depthMode; // 0=Linear, 1=Log, 2=FocusRange
	ofParameter<bool> useLogDepth;
	ofParameter<string> depthModeName;
	vector<string> depthModeNames;
	ofParameter<void> vResetMode;

	// Camera parameters
	ofParameterGroup paramsCamera;
	ofParameter<bool> useManualClipPlanes;
	ofParameter<float> manualNear;
	ofParameter<float> manualFar;
	ofParameter<void> vResetManual;

	// Focus range parameters (for depthMode == 2)
	ofParameterGroup paramsFocus;
	ofParameter<float> focusNear;
	ofParameter<float> focusFar;
	ofParameter<float> focusRange;
	ofParameter<void> vResetFocus;

	ofParameter<void> vResetAll;

	int width, height;

private:
	void updateDepthModeString();

	// Event listeners
	ofEventListener useManualCameraClipPlanesListener;
	ofEventListener manualNearListener;
	ofEventListener manualFarListener;
	ofEventListener depthModeListener;

	ofEventListener vResetAllListener;
	ofEventListener vResetTweaksListener;
	ofEventListener vResetModeListener;
	ofEventListener vResetManualListener;
	ofEventListener vResetFocusListener;

	void doResetTweaks();
	void doResetMode();
	void doResetManual();
	void doResetFocus();

public:
	void doResetAll();

private:
	ofFbo fbo; // single FBO
	ofShader shader; // external file shader
	ofCamera * camera; // pointer to camera
};

/*
Principales mejoras implementadas:
1. Modos de profundidad mejorados:

Linear: Distribución lineal tradicional
Logarithmic: Mejor para perspectiva, evita zonas planas
Focus Range: Concentra el contraste en un rango específico de profundidad

2. Control manual de near/far:

Toggle useCameraClipPlanes para alternar entre valores de cámara y manuales
Parámetros manualNear y manualFar completamente funcionales
El shader usa correctamente estos valores según el toggle

3. Sistema de enfoque para ControlNet:

Parámetros focusNear, focusFar y focusRange para el modo Focus Range
Auto-ajuste inteligente de los valores de enfoque basado en el rango de profundidad
Mapeo optimizado: rango de enfoque usa 0.2-0.8, zonas exteriores se comprimen a 0.0-0.2 y 0.8-1.0

4. Mejoras en la interfaz:

Organización de parámetros en grupos lógicos
Listeners para auto-actualizar valores cuando cambias entre manual/automático
Información del modo actual en el guardado de archivos

Para usar las nuevas funcionalidades:

Modo Linear: Funciona como antes, distribución lineal simple
Modo Logarithmic: Activa useLogDepth para mejor distribución en perspectiva
Modo Focus Range: Ajusta focusNear y focusFar para concentrar el contraste en objetos específicos

El Focus Range es especialmente útil para ControlNet porque evita que objetos lejanos
o muy cercanos dominen el mapa de profundidad, manteniendo el contraste donde realmente importa
para el control de la generación de imagen.
*/
