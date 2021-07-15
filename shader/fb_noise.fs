#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D screenNoiseTexture;

uniform vec2 noisePos;

uniform vec2 mousePos;
uniform int bUserDetect;
uniform float variableMouseRadius;


void main()
{   
    // noise patch
    vec2 distanceVec = abs(noisePos-TexCoords);
    float distance = max(distanceVec.x,distanceVec.y);
    if(distance<0.125f)
    {
        FragColor = texture(screenNoiseTexture, TexCoords);
    }
    else
    {
        FragColor = texture(screenTexture, TexCoords);
    }

    // mouse cursor
    distanceVec = abs(mousePos-TexCoords);
    distance = max(distanceVec.x,distanceVec.y);
    if(distance<0.005f*(1+variableMouseRadius))
    {
        if(bUserDetect==1)
        {
            FragColor = vec4(0,1,0,1);
        }
        else
        {
            FragColor = (vec4(1,1,1,1) - texture(screenTexture, TexCoords)) * vec4(0.5,0,0,1);
        }
    }

}