#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in ParticleDataGeom
{
    float size;
} particle[];

out ParticleData
{
    vec2 textureCoord;
} pOut;

vec4 position;
float size;

void generateBillboard() {    
    gl_Position = position + vec4( -size, -size, 0.0, 0.0); 
    pOut.textureCoord = vec2(0,0);
    EmitVertex();

    gl_Position = position + vec4( size, -size, 0.0, 0.0);
    pOut.textureCoord = vec2(1,0);
    EmitVertex();

    gl_Position = position + vec4( -size, size, 0.0, 0.0);
    pOut.textureCoord = vec2(0,1);
    EmitVertex();

    gl_Position = position + vec4( size, size, 0.0, 0.0);
    pOut.textureCoord = vec2(1,1);
    EmitVertex();
    
    EndPrimitive();
}

void main() {
    position = gl_in[0].gl_Position;
    size = particle[0].size;
    generateBillboard();
}