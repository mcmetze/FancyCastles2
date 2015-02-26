#version 330
#define PI 3.14159265358979323846 

layout(points) in;
layout(triangle_strip, max_vertices = 12) out;

in vec3 vColor[];

uniform vec2 selectedPoint;

out vec3 fColor;
out vec2 fTexcoord;

void main() 
{
	//default tex coords for unselected tile
	vec2 texc[6] = vec2[](vec2(0.0, 0.0), 
                          vec2(0.0, 0.0), 
                          vec2(0.0, 0.0), 
                          vec2(0.0, 0.0), 
                          vec2(0.0, 0.0), 
                          vec2(0.0, 0.0)); 
	float w = 1366.f/100.f;
	float h = 768.f/100.f;
	vec2 verts[6]; 

	// create the new verts of the hex and store them in an array to be triangulated 
	for (int corner = 0; corner < 6; corner++)
	{ 
		float angle = 2.0 * PI / 6.0 * (corner + 0.5); 
		float cornerX = gl_in[0].gl_Position.x + cos(angle);	
		float cornerY = gl_in[0].gl_Position.y + sin(angle); 
		verts[corner] = vec2(cornerX/w, cornerY/h); 
	}
    
	// check to see if the tile selected from the pick is the current tile
	// if so, update the tex coordinates for so the selection texture appears  
	if (selectedPoint.x == gl_in[0].gl_Position.x && selectedPoint.y == gl_in[0].gl_Position.y )
	{ 
		texc = vec2[](vec2(1.0, 0.75), 
			          vec2(0.5, 1.0), 
				      vec2(0.0, 0.75), 
	                  vec2(0.0, 0.25), 
					  vec2(0.5, 0.0), 
					  vec2(1.0, 0.25)); 
	}

	//need to triangulate the hex in the right order so it renders properly
	fColor = vColor[0];
	int curVert = 0;
	for(int triangle = 0; triangle < 4; triangle++) 
	{
		gl_Position =  vec4(verts[0].xy, 0.0, 1.0); 
		fTexcoord = texc[0]; 
		EmitVertex(); 
		gl_Position =  vec4(verts[curVert+1].xy, 0.0, 1.0); 
		fTexcoord = texc[curVert+1]; 
		EmitVertex(); 
		gl_Position =  vec4(verts[curVert+2].xy, 0.0, 1.0); 
		fTexcoord = texc[curVert+2]; 
		EmitVertex(); 
		curVert++; 
	} 
	EndPrimitive(); 
}