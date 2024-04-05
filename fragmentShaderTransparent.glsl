#version 330

in vec4 outColTransp;
out vec4 fragColor;

void main(){
    fragColor  = vec4(outColTransp.x, outColTransp.y, outColTransp.z, outColTransp.w);
}  