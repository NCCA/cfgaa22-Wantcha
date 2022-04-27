#version 430 core

layout(location = 0) in vec3 position;

uniform mat4 MVP;
uniform float reciprScaleOnScreen; // change value to match resolution. = (2 * ObjectSizeOnscreenInPixels / ScreenWidthInPixels)

void main()
{
    //float w = 1;
    float w = (MVP * vec4(0,0,0,1)).w;;
    w *= reciprScaleOnScreen;

    gl_Position = MVP * vec4(position.xyz * w , 1.0);
}