#version 330

// Input Variables (received from Vertex Shader)
in vec4 color;
in vec4 position;
in vec3 normal;
in vec2 texCoord0;
uniform vec3 waterColor;
in float waterDepth; 
uniform vec3 fogColour;
in float fogFactor;



// Output Variable (sent down through the Pipeline)
out vec4 outColor;

uniform sampler2D textureBed;
uniform sampler2D textureShore;


void main(void) 
{
	float isAboveWater = clamp(-waterDepth, 0, 1); 
	outColor = color;
	outColor *= mix(texture(textureBed, texCoord0), texture(textureShore, texCoord0), isAboveWater);

	outColor = mix(vec4(fogColour, 1), outColor, fogFactor);
}
