#version 330 

in vec3 fColor;
in vec2 fTexcoord;
out vec4 fragColor;

uniform sampler2D tileOutlineTex;
void main()
{
   vec4 outlineColor = texture(tileOutlineTex, fTexcoord);
   if(outlineColor.a == 0)
   { 
       outlineColor = vec4(fColor, 1.0);
   }
   fragColor = outlineColor; 
}