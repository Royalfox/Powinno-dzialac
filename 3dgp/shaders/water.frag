#version 330

// Input Variables (received from Vertex Shader)
in vec4 color;
in vec4 position;
in vec3 normal;
in vec2 texCoord0;
uniform vec3 waterColor;
uniform vec3 skyColor;
in float reflFactor;

// Output Variable (sent down through the Pipeline)
out vec4 outColor;

void main(void) 
{
    // Calculate the final color as a mix of water and sky colors based on reflection factor
    vec4 finalColor = mix(vec4(waterColor, 1.0), vec4(skyColor, 1.0), reflFactor);

    // Set the output color with transparency
    outColor = vec4(finalColor.rgb, 0.5); // Adjust transparency here (0.0 - fully transparent, 1.0 - fully opaque)
}
