#version 430 core
layout (triangles, invocations=6) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowTransforms[6];

out vec3 gsPosition;

#define ID gl_InvocationID

void main() {
    gl_Layer = ID;
    for(int i = 0; i < 3; i++) {
        vec4 FragPos = gl_in[i].gl_Position;
        gl_Position = shadowTransforms[ID] * FragPos;
		gsPosition = FragPos.xyz;
        EmitVertex();
    }    
    EndPrimitive();
} 