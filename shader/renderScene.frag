#version 430

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

vec3 lightPosView;

float calculateShadow() {
    vec3 L = outPosition - lightPosition; 
    float closestDepth = texture(depthCubeMap, L).r;
	closestDepth *= far; 
	float currentDepth = length(L);
    float bias = 0.15; 
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}

void main() { 
	float levels = 5.0;
	float thickness = 0.2;
	
    vec3 n = normalize(outNormal);
	if(cartoon) {
		vec3 viewDir = normalize(viewPos - outPosition);
		if (abs(dot(viewDir, n)) < thickness) {
			outColor.rgb = vec3(0.0);
			return;
		}
	}

	lightPosView = lightPosition;

	vec3 color = Color; //texture(diffuseTexture, outUV).rgb;
    // Ambient
    vec3 ambient = 0.9 * color;
    // Diffuse
    vec3 s = normalize(lightPosView - outPosition);
	float diff = max(dot(s, n), 0.0);
	if(cartoon) {
		diff = floor(diff * levels) * (1.0 / levels);
	}
    vec3 diffuse = diff * lightColor;
    // Specular
    vec3 viewDir = normalize(viewPos - outPosition);
    vec3 reflectDir = reflect(-s, n);
    float spec = 0.0;
    vec3 halfwayDir = normalize(s + viewDir);  
    spec = pow(max(dot(n, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // Calculate shadow
    float shadow = enableShadows ? calculateShadow() : 0.0;                      
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    

    outColor = vec4(lighting, 1.0);
}