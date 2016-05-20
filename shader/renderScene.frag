#version 430

//#define ONLY_SHADOW

out vec4 outColor;

in vec3 outPosition;
in vec3 outNormal;
in vec2 outUV;

uniform samplerCube depthCubeMap;

uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform float far;

uniform mat4 view;

uniform bool enableShadows;
uniform bool cartoon;
uniform vec3 Color;
uniform bool reverse;

float levels = 5.0;
float thickness = 0.2;

float constant = 1.0;
float linear = 0.045;
float quadratic = 0.007;

float fDepthBias = 0.15;
float samples = 15;
float bias = 0.05; 

vec3 gridSamplingDisk[20] = vec3[] (
   vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1), 
   vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
   vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
   vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
);

// http://http.developer.nvidia.com/GPUGems/gpugems_ch12.html
float calculateShadowValue() {
	float shadow = 0.0;
	
	vec3 L = outPosition - lightPosition; 
	float currentDepth = length(L);

#ifndef ONLY_SHADOW
	float viewDistance = length(viewPos - outPosition);
	float diskRadius = 0.07;
	for(int i = 0; i < samples; i++) {
		float vShadowSample = texture(depthCubeMap, L + gridSamplingDisk[i] * diskRadius).r;
		vShadowSample *= far;
		if(currentDepth - fDepthBias > vShadowSample) {
			shadow += 1.0;
		}
	}
	shadow /= float(samples);
#endif
	
#ifdef ONLY_SHADOW
	// Discomment to show shadow value
    float vShadowSample = texture(depthCubeMap, L).r;
    vShadowSample *= far;
    shadow = currentDepth - bias > vShadowSample ? 1.0 : 0.0;
	outColor = vec4(vec3(vShadowSample/far), 1.0);
#endif
	return shadow;
}

void main() { 
    vec3 n = normalize(outNormal);
	if(cartoon) {
		vec3 viewDir = normalize(viewPos - outPosition);
		if (abs(dot(viewDir, n)) < thickness) {
			outColor.rgb = vec3(0.0);
			return;
		}
	}

	float dist = length(lightPosition - outPosition);

	float attenuation = 1.0f / (constant + linear * dist + quadratic * (dist * dist));    

	vec3 color = Color;
    // Ambient
    vec3 ambient = 1.0 * color;
    // Diffuse
    vec3 s = normalize(lightPosition - outPosition);
	float diff = max(dot(s, n), 0.0);
	if(cartoon) {
		diff = floor(diff * levels) * (1.0 / levels);
	}
    vec3 diffuse = diff * lightColor;
    // Specular
    vec3 viewDir = normalize(viewPos - outPosition);
    vec3 reflectDir = reflect(-s, n);
    vec3 halfwayDir = normalize(s + viewDir);  
    float spec = pow(max(dot(n, halfwayDir), 0.0), 128.0);
    vec3 specular = spec * lightColor;    

	// Apply attenuation
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	float shadow = calculateShadowValue();

	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
	
#ifndef ONLY_SHADOW
	outColor = vec4(lighting, 1.0);
#endif
}