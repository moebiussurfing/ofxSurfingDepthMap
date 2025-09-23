#version 330

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;

in vec4 position;
out float viewZ;

void main(){
    vec4 viewPos = modelViewMatrix * position;
    viewZ = -viewPos.z; // distance from camera
    gl_Position = modelViewProjectionMatrix * position;
}
