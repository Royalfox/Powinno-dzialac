#version 330

// Uniforms: Transformation Matrices
uniform mat4 matrixProjection;
uniform mat4 matrixView;
uniform mat4 matrixModelView;

// Uniforms: Material Colours
uniform vec3 materialAmbient;

in vec3 aVertex;
in vec3 aNormal;
in vec2 aTexCoord;

out vec4 color;
out vec4 position;
out vec3 normal;
out vec2 texCoord0;
out float reflFactor;

uniform float t; 

float wave(float A, float x, float y, float t)
{
t *= 2;
return A * (
sin(2.0 * (x * 0.2 + y * 0.7) + t * 1.0) +
sin(2.0 * (x * 0.7 + y * 0.2) + t * 0.8) +
pow(sin(2.0 * (x * 0.6 + y * 0.5) + t * 1.2), 2) +
pow(sin(2.0 * (x * 0.8 + y * 0.2) + t * 1.1), 2));
}


// Light declarations
struct AMBIENT
{	
	vec3 color;
};
uniform AMBIENT lightAmbient;

vec4 AmbientLight(AMBIENT light)
{
	// Calculate Ambient Light
	return vec4(materialAmbient * light.color, 1);
}

void main(void) 
{
    // Calculate time-dependent wave displacement
    float waveAmplitude = 0.05; // Adjust as needed
    float waveTime = t; // Use the time variable from the application
    float waveY = wave(waveAmplitude, aVertex.x, aVertex.z, waveTime);
    
    // Apply the wave displacement to the vertex position
    vec3 newVertex = vec3(aVertex.x, waveY, aVertex.z);
    
    // Transform the vertex position to the eye space
    position = matrixModelView * vec4(newVertex, 1.0);
    
    // Calculate the final vertex position in clip space
    gl_Position = matrixProjection * position;
    
    // Calculate the normal of the water surface
    float waveDisturbance = 0.05; // Adjust as needed
    float dx = (wave(waveAmplitude, aVertex.x + waveDisturbance, aVertex.z, waveTime) - wave(waveAmplitude, aVertex.x - waveDisturbance, aVertex.z, waveTime)) / (2.0 * waveDisturbance);
    float dz = (wave(waveAmplitude, aVertex.x, aVertex.z + waveDisturbance, waveTime) - wave(waveAmplitude, aVertex.x, aVertex.z - waveDisturbance, waveTime)) / (2.0 * waveDisturbance);
    vec3 newNormal = normalize(vec3(-dx, 1.0, -dz));
    
    // Transform the normal to the eye space and normalize it
    normal = normalize(mat3(matrixModelView) * newNormal);
    
    // Pass texture coordinates to the fragment shader
    texCoord0 = aTexCoord;
    
    // Calculate reflection factor for water reflections
    float cosTheta = dot(normal, normalize(-position.xyz));
    float R0 = 0.02; // Adjust as needed
    reflFactor = R0 + (1.0 - R0) * pow(1.0 - cosTheta, 5.0);
}

