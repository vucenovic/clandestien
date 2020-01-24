#version 450 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in ParticleDataGeom
{
    float size;
    vec2 upVector;
} particle[];

out ParticleData
{
    vec2 textureCoord;
} pOut;

vec4 position;
float size;
vec2 upVec;
vec2 rightVec;

void generateBillboard() {    
    gl_Position = position + vec4(-(upVec + rightVec) * size,0,0); 
    pOut.textureCoord = vec2(0,0);
    EmitVertex();

    gl_Position = position + vec4((rightVec - upVec) * size,0,0);
    pOut.textureCoord = vec2(1,0);
    EmitVertex();

    gl_Position = position + vec4((upVec - rightVec) * size,0,0);
    pOut.textureCoord = vec2(0,1);
    EmitVertex();

    gl_Position = position + vec4((upVec + rightVec) * size,0,0);
    pOut.textureCoord = vec2(1,1);
    EmitVertex();
    
    EndPrimitive();
}

void main() {
    position = gl_in[0].gl_Position;
    size = particle[0].size;
    upVec = particle[0].upVector;
    rightVec = vec2(upVec.y,-upVec.x);
    generateBillboard();
}