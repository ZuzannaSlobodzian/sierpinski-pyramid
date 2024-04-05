#version 330 

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 inCol;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec4 inColTransp;

uniform mat4 MVP;

out vec3 outCol;
out vec4 outColTransp;
out vec3 normal;
out vec3 fragPos;

void main(){
    outCol = inCol;
    outColTransp = inColTransp;
    normal = vNormal;
    fragPos = aPos;

    gl_Position = MVP*vec4(aPos.x, aPos.y, aPos.z, 1.0);
}