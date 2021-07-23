#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D screenNoiseTexture;

uniform vec2 noisePos;

uniform vec2 mousePos;
uniform int bUserDetect;
uniform float variableMouseRadius;

uniform int visible;
uniform float flash;

float border_r = 0.25;
float border_width = 0.005;

uniform bool activateBorder;

vec2 borderCoord(int ID)
{
    ID=ID+1;
    if(ID==1 || ID==2 || ID==5 || ID==6) return vec2(0.25,0.25);
    if(ID==3 || ID==4 || ID==7 || ID==8) return vec2(0.75,0.25);
    if(ID==9 || ID==10 || ID==13 || ID==14) return vec2(0.25,0.75);
    if(ID==11 || ID==12 || ID==15 || ID==16) return vec2(0.75,0.75);
    return vec2(-1,-1);
}

int XYtoID(vec2 position)
{
    float x=position.x;
    float y=position.y;
    if(x<0 || y<0) return -1;
    else
    {
        x=int(ceil((x+0.05)*4 -1));
        y=int(ceil((y+0.05)*4 -1));
        return int(abs(x+(4*y)));
    }
}
    
    
            

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


    vec2 borderPosition = borderCoord(XYtoID(noisePos));
    distanceVec = abs(borderPosition-TexCoords);
    distance = max(distanceVec.x,distanceVec.y);
    if(distance > border_r-border_width && distance < border_r && activateBorder)
    {
        FragColor = mix(FragColor,FragColor * vec4(1,0,0,1),0.5);
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