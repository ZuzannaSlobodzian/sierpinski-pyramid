#version 330

in vec3 outCol;
in vec3 normal;  
in vec3 fragPos;  

vec3 viewPos;
uniform vec3 lightPos; 
uniform vec3 lightColor;

out vec4 fragColor;

void main(){
    viewPos[0] = 10.3f;
    viewPos[1] = 10.3f;
    viewPos[2] = 0.3f;

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor; 
        
    vec3 result = (ambient + diffuse + specular) * outCol;
    fragColor = vec4(result, 1.0);

}  