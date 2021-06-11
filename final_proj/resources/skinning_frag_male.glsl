#version 330 core

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;


uniform sampler2D tex;
uniform vec3 camPos;

uniform float time;
uniform float start_interp;

out vec4 fragCol;

void main()
{
	fragCol = vec4(1,0,0,1);
	vec4 tcol = texture(tex, vertex_tex);
	fragCol.rgb = tcol.rgb;
    
    if(start_interp > 0.5)
    {
        fragCol.a = (0.7 * (1-time)) + (0.1 * time);
    }
    else{
        fragCol.a = 0.7;
    }
    //fragCol.a = 0.3;
    
    
}


