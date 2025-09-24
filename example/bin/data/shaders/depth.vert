#version 120
uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;
attribute vec4 position;
varying float viewZ;

void main(){
    vec4 viewPos = modelViewMatrix * position;
    viewZ = -viewPos.z; // distance from camera
    gl_Position = modelViewProjectionMatrix * position;
}