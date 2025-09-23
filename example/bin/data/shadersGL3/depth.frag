//depth.frag
#version 330

uniform float nearPlane;
uniform float farPlane;
uniform float depthContrast;
uniform float depthBrightness;
uniform float depthGamma;
uniform int invertDepth;

uniform float nearClip;
uniform float farClip;
uniform int useLogDepth;

in float viewZ;
out vec4 fragColor;

void main(){
    // Normalize depth between near and far
    float normalizedDepth = (viewZ - nearPlane) / (farPlane - nearPlane);
    normalizedDepth = clamp(normalizedDepth, 0.0, 1.0);

    // Contrast
    normalizedDepth = ((normalizedDepth - 0.5) * depthContrast) + 0.5;
    normalizedDepth = clamp(normalizedDepth, 0.0, 1.0);

    // Gamma
    normalizedDepth = pow(normalizedDepth, 1.0 / depthGamma);

    // Brightness
    normalizedDepth = clamp(normalizedDepth + depthBrightness, 0.0, 1.0);

    // Invert if needed
	float depthValue = (invertDepth == 1) ? (1.0 - normalizedDepth) : normalizedDepth;

    fragColor = vec4(vec3(depthValue), 1.0);
}
