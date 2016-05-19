#version 430

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 UV;

out vec3 outPosition;
out vec3 outNormal;
out vec2 outUV;

uniform mat4 viewProjection;
uniform mat4 model;
uniform bool reverse;

void main() {
	mat3 normal = transpose(inverse(mat3(model)));
    outNormal = normalize( normal * Normal);
	if(reverse) outNormal = - outNormal;

    outPosition = vec3( model * vec4(Position,1.0) );
	outUV = UV;

    gl_Position = viewProjection * model * vec4(Position,1.0);
}