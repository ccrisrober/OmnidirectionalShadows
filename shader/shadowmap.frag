#version 430 core
in vec3 gsPosition;

uniform vec3 lightPosition;
uniform float far;

void main() {
    float Ldist = length(gsPosition - lightPosition);
    
    Ldist = Ldist / far;	// Normalize value with far to range [0, 1]
    
    gl_FragDepth = Ldist;
}