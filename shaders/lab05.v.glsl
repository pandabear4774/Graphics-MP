#version 410 core




// uniform inputs
uniform mat4 mvpMatrix;                 // the precomputed Model-View-Projection Matrix
uniform mat3 normMatrix;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 cameraPosition;
uniform mat4 modelMatrix;

uniform vec3 materialColor;             // the material color for our vertex (& whole object)

// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space
in vec3 vecNormal;

// varying outputs
layout(location = 0) out vec3 color;    // color to apply to this vertex

void main() {
    color = vec3(0,0,0);

    // transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    //ambient
    float ambientStrength = 0.15;
    vec3 ambientComponent = ambientStrength * lightColor;

    //diffuse
    vec3 reverseLightVector = normalize(-1 * lightDir);
    vec3 trueVecNormal = vecNormal * normMatrix;
    vec3 diffuseComponent = lightColor * max(dot(reverseLightVector, trueVecNormal), 0);

    //specular
    float specularStrength = 0.2;
    vec3 worldCords = vec3(modelMatrix * vec4(vPos,1.0));
    vec3 viewDir = normalize(cameraPosition - worldCords);
    vec3 reflectDir = reflect(-reverseLightVector, trueVecNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 3);
    vec3 specularComponent = specularStrength * spec * lightColor;

    color += (ambientComponent + diffuseComponent + specularComponent) * 0.75;


    vec3 lightPos = vec3(-55,30,-55);

    vec3 lightDir2 = normalize(lightPos - worldCords);
    // diffuse shading
    float diff = max(dot(trueVecNormal, lightDir2), 0.0);
    // specular shading
    vec3 reflectDir2 = reflect(-lightDir2, trueVecNormal);
    float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), 3);
    // attenuation
    float distance    = length(lightPos - worldCords);
    float attenuation = 1.0 / (1 + 2 * distance +
                 1 * (distance * distance));
    // combine results
    vec3 ambient  = vec3(1.0,1.0,1.0) * 0.15;
    vec3 diffuse  = vec3(1.0,1.0,1.0) * diff;
    vec3 specular = vec3(1.0,1.0,1.0) * spec2;
    ambient  *= attenuation * 200;
    diffuse  *= attenuation * 500;
    specular *= attenuation * 600;

    color += (ambient + diffuse + specular);

    color = materialColor * color;
}
