#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColour;

out struct data 
{
    vec3 position;
    vec3 normal;
    vec4 colour;
} fs_in;

void main()
{    
    // Store the fragment position vector in the first gbuffer texture
    gPosition = fs_in.position;
    // Also store the per-fragment normals into the gbuffer
    gNormal = normalize(fs_in.normal);
    // And the diffuse per-fragment color
    gColour.rgb = fs_in.colour.xyz;
    // Store specular intensity in gColour's alpha component
    gColour.a = fs_in.colour.w;
}