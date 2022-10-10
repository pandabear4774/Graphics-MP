#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;                 // the precomputed Model-View-Projection Matrix
uniform mat3 normMatrix;
uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 materialColor;             // the material color for our vertex (& whole object)

// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space
in vec3 vecNormal;

// varying outputs
layout(location = 0) out vec3 color;    // color to apply to this vertex

void main() {
    // transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    vec3 reverseLightVector = normalize(-1 * lightDir);
    vec3 trueVecNormal = vecNormal * normMatrix;
    vec3 diffuseComponent = lightColor * materialColor * max(dot(reverseLightVector, trueVecNormal), 0);
    color = diffuseComponent;
}
