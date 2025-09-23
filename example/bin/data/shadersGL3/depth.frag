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
uniform int depthMode; // 0=Linear, 1=Log, 2=FocusRange
uniform float focusNear;
uniform float focusFar;
uniform float focusRange;

in float viewZ;
out vec4 fragColor;

void main(){
    // Use manual clipping planes if enabled, otherwise use camera planes
    float near = nearClip;
    float far = farClip;
    
    float normalizedDepth;
    
    if (depthMode == 0) {
        // LINEAR MODE - Traditional linear mapping
        normalizedDepth = (viewZ - near) / (far - near);
        normalizedDepth = clamp(normalizedDepth, 0.0, 1.0);
        
    } else if (depthMode == 1) {
        // LOG MODE - Logarithmic depth for better perspective distribution
        if (useLogDepth == 1) {
            float logNear = log(near);
            float logFar = log(far);
            float logViewZ = log(max(viewZ, near)); // Avoid log(0)
            normalizedDepth = (logViewZ - logNear) / (logFar - logNear);
        } else {
            // Alternative log mapping
            float c = 0.1; // Log curve factor
            normalizedDepth = (viewZ - near) / (far - near);
            normalizedDepth = log(1.0 + c * normalizedDepth) / log(1.0 + c);
        }
        normalizedDepth = clamp(normalizedDepth, 0.0, 1.0);
        
    } else if (depthMode == 2) {
        // FOCUS RANGE MODE - Enhanced contrast in a specific depth range
        float focusCenter = (focusNear + focusFar) * 0.5;
        float focusWidth = focusFar - focusNear;
        
        if (focusWidth > 0.0) {
            // Distance from focus center
            float distFromFocus = abs(viewZ - focusCenter);
            
            if (distFromFocus <= focusWidth * 0.5) {
                // Inside focus range - high contrast mapping
                float localPos = (viewZ - focusNear) / focusWidth;
                normalizedDepth = 0.2 + localPos * 0.6; // Map to 0.2-0.8 range
            } else {
                // Outside focus range - compress to edges
                if (viewZ < focusNear) {
                    float ratio = (viewZ - near) / (focusNear - near);
                    normalizedDepth = ratio * 0.2; // Map to 0.0-0.2
                } else {
                    float ratio = (viewZ - focusFar) / (far - focusFar);
                    normalizedDepth = 0.8 + ratio * 0.2; // Map to 0.8-1.0
                }
            }
        } else {
            // Fallback to linear if focus range is invalid
            normalizedDepth = (viewZ - near) / (far - near);
        }
        normalizedDepth = clamp(normalizedDepth, 0.0, 1.0);
    }
    
    // Apply contrast adjustment
    normalizedDepth = ((normalizedDepth - 0.5) * depthContrast) + 0.5;
    normalizedDepth = clamp(normalizedDepth, 0.0, 1.0);
    
    // Apply gamma correction
    normalizedDepth = pow(normalizedDepth, 1.0 / depthGamma);
    
    // Apply brightness adjustment
    normalizedDepth = clamp(normalizedDepth + depthBrightness, 0.0, 1.0);
    
    // Apply inversion if needed
    float depthValue = (invertDepth == 1) ? (1.0 - normalizedDepth) : normalizedDepth;
    
    // Output as grayscale
    fragColor = vec4(vec3(depthValue), 1.0);
}
