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
    float ambientStrength = 0.05;
    vec3 ambientComponent = ambientStrength * lightColor;

    //diffuse
    vec3 reverseLightVector = normalize(-1 * lightDir);
    vec3 trueVecNormal = vecNormal * normMatrix;
    vec3 diffuseComponent = lightColor * max(dot(reverseLightVector, trueVecNormal), 0);

    //specular
    float specularStrength = 0.1;
    vec3 worldCords = vec3(modelMatrix * vec4(vPos,1.0));
    vec3 viewDir = normalize(cameraPosition - worldCords);
    vec3 reflectDir = reflect(-reverseLightVector, trueVecNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2);
    vec3 specularComponent = specularStrength * spec * lightColor;

    color += (ambientComponent + diffuseComponent + specularComponent) / 4;


    vec3 lightPos = vec3(-60,30,-60);

    vec3 lightDir2 = normalize(lightPos - worldCords);
    // diffuse shading
    float diff = max(dot(vecNormal, lightDir2), 0.0);
    // specular shading
    vec3 reflectDir2 = reflect(-lightDir2, vecNormal);
    float spec2 = pow(max(dot(viewDir, reflectDir2), 0.0), 3);
    // attenuation
    float distance    = length(lightPos - worldCords);
    float attenuation = 1.0 / (1 + 0.005 * distance +
                 0.008 * (distance * distance));
    // combine results
    vec3 ambient  = vec3(1.0,1.0,1.0) * 0.15;
    vec3 diffuse  = vec3(1.0,1.0,1.0) * diff;
    vec3 specular = vec3(1.0,1.0,1.0) * spec2;
    ambient  *= attenuation;
    diffuse  *= attenuation * 4;
    specular *= attenuation / 5;

    color += (ambient + diffuse + specular);


    //spot light
    vec3 spotLightPosition = vec3(0,20,0);
    vec3 spotLightDirection = vec3(0,-1,0);
    float cutoffAngle = 0.8;

    vec3 lightDir3 = normalize(spotLightPosition - worldCords);

    float theta = dot(spotLightDirection, normalize(-lightDir3));

    if(theta > cutoffAngle){
        // diffuse shading
        float diff2 = max(dot(vecNormal, lightDir3), 0.0);
        // specular shading
        vec3 reflectDir3 = reflect(-lightDir3, trueVecNormal);
        float spec3 = pow(max(dot(viewDir, reflectDir3), 0.0), 3);
        // attenuation
        float distance2    = length(spotLightPosition - worldCords);
        float attenuation2 = 1.0 / (1 + 2 * distance2 +
                     1 * (distance2 * distance2));
        // combine results
        vec3 ambient2  = vec3(1.0,1.0,1.0) * 0.15;
        vec3 diffuse2  = vec3(1.0,1.0,1.0) * diff2;
        vec3 specular2 = vec3(1.0,1.0,1.0) * spec3;
        ambient2  *=  attenuation2 * 400;
        diffuse2  *= attenuation2 * 600;
        specular2 *= attenuation2 * 200;

        color += (diffuse2 + specular2 + ambient2);
    }


    color = materialColor * color;
}
