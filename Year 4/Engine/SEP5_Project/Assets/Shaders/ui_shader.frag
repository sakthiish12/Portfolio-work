#version 450

layout (location = 0) in vec2 uvIn;
layout (location = 1) in vec2 posIn;

layout (location = 0) out vec4 outColor;

layout (binding = 1)    uniform     sampler2D 	uiTexture1;
layout (binding = 2)    uniform     sampler2D 	uiTexture2;
layout (binding = 3)    uniform     sampler2D 	uiTexture3;

layout(push_constant) uniform Push {
	vec4 posAndScale; //pos.x pos.y scale.x scale.y
	vec4 flickerColor;
	int textureToUse; //1~3
	float percentage;
	float flickerOpacity;
    float opacity;
	int isVertical;
	int uiType; 
	float brightness;
	float rotationRad;
    vec4 colorIntensityFactors; //only rgb
	// 1 - button | 2 - font | 3 - image 
	//| 4 - progressbarBG | 5 - progressbarFG | 6 - progressbarBorder
	//| 7 - sliderBG | 8 - sliderKnob | 9 - toggleBG | 10 - toggleCheckmark
} push;

void main() 
{
    if(push.textureToUse == 1)
    {
        outColor= vec4(texture(uiTexture1,uvIn).xyz,texture(uiTexture1,uvIn).w*push.opacity);
    }
    else if(push.textureToUse == 2)
    {
        if(push.uiType == 5)
        {
            if(push.isVertical == 1)
            {
                //uvIn is from 0 ~ 1
                if(posIn.y > (push.percentage*2.0-1.0))
                {
                    outColor=vec4(0.0);
                }
                else
                {
                    outColor=vec4((texture(uiTexture2,uvIn).xyz*push.flickerOpacity + push.flickerColor.xyz * (1.0-push.opacity)),texture(uiTexture2,uvIn).w * push.opacity);
                }
            }
            else
            {
                //uvIn is from 0 ~ 1
                if(posIn.x > (push.percentage * 2.0 - 1.0))
                {
                    outColor=vec4(0.0);
                }
                else
                {
                    outColor=vec4((texture(uiTexture2,uvIn).xyz*push.flickerOpacity + push.flickerColor.xyz * (1.0-push.opacity)),texture(uiTexture2,uvIn).w* push.opacity);
                }
            }
        }
        else
        {
            outColor = vec4(texture(uiTexture2,uvIn).xyz,texture(uiTexture2,uvIn).w* push.opacity);
        }
    }
    else if(push.textureToUse==3)
    {
        outColor = vec4(texture(uiTexture3,uvIn).xyz,texture(uiTexture3,uvIn).w*push.opacity);
    }
    else
    {
        outColor = vec4(1.0,1.0,1.0,1.0);
    }

    outColor = vec4(outColor.xyz * push.brightness,outColor.w);
    outColor = vec4(outColor.x * push.colorIntensityFactors.x,outColor.y * push.colorIntensityFactors.y,outColor.z * push.colorIntensityFactors.z,outColor.w);
}
