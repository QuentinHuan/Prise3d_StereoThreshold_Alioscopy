#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D screenNoiseTexture;

uniform vec2 noisePos;

uniform vec2 mousePos;
uniform int bUserDetect;

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
    if(bUserDetect==1)
    {
        distanceVec = abs(mousePos-TexCoords);
        distance = max(distanceVec.x,distanceVec.y);
        if(distance<0.005f)
        {
            FragColor = vec4(0,1,0,1);
        }
    }

}