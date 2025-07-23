# Basic Texture Shader

#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TextCoord;
layout(location = 3) in float a_TextIndex;
layout(location = 4) in float a_TilingFactor;

uniform mat4 u_ViewProjection;

out vec4 v_Color;
out vec2 v_TextCoord;
out flat float v_TextIndex;
out float v_TilingFactor;

void main()
{
	v_Color = a_Color;
	v_TextCoord = a_TextCoord;
	v_TextIndex = a_TextIndex;
	v_TilingFactor = a_TilingFactor;
	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450
			
layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in vec4 v_Color;
in vec2 v_TextCoord;
in flat float v_TextIndex;
in float v_TilingFactor;

uniform sampler2D u_Textures[32];

void main()
{
	vec4 textColor = v_Color;
	switch(int(v_TextIndex))
	{
		case 0: textColor *= texture(u_Textures[0], v_TextCoord * v_TilingFactor); break;
		case 1: textColor *= texture(u_Textures[1], v_TextCoord * v_TilingFactor); break;
		case 2: textColor *= texture(u_Textures[2], v_TextCoord * v_TilingFactor); break;
		case 3: textColor *= texture(u_Textures[3], v_TextCoord * v_TilingFactor); break;
		case 4: textColor *= texture(u_Textures[4], v_TextCoord * v_TilingFactor); break;
		case 5: textColor *= texture(u_Textures[5], v_TextCoord * v_TilingFactor); break;
		case 6: textColor *= texture(u_Textures[6], v_TextCoord * v_TilingFactor); break;
		case 7: textColor *= texture(u_Textures[7], v_TextCoord * v_TilingFactor); break;
		case 8: textColor *= texture(u_Textures[8], v_TextCoord * v_TilingFactor); break;
		case 9: textColor *= texture(u_Textures[9], v_TextCoord * v_TilingFactor); break;
		case 10: textColor *= texture(u_Textures[10], v_TextCoord * v_TilingFactor); break;
		case 11: textColor *= texture(u_Textures[11], v_TextCoord * v_TilingFactor); break;
		case 12: textColor *= texture(u_Textures[12], v_TextCoord * v_TilingFactor); break;
		case 13: textColor *= texture(u_Textures[13], v_TextCoord * v_TilingFactor); break;
		case 14: textColor *= texture(u_Textures[14], v_TextCoord * v_TilingFactor); break;
		case 15: textColor *= texture(u_Textures[15], v_TextCoord * v_TilingFactor); break;
		case 16: textColor *= texture(u_Textures[16], v_TextCoord * v_TilingFactor); break;
		case 17: textColor *= texture(u_Textures[17], v_TextCoord * v_TilingFactor); break;
		case 18: textColor *= texture(u_Textures[18], v_TextCoord * v_TilingFactor); break;
		case 19: textColor *= texture(u_Textures[19], v_TextCoord * v_TilingFactor); break;
		case 20: textColor *= texture(u_Textures[20], v_TextCoord * v_TilingFactor); break;
		case 21: textColor *= texture(u_Textures[21], v_TextCoord * v_TilingFactor); break;
		case 22: textColor *= texture(u_Textures[22], v_TextCoord * v_TilingFactor); break;
		case 23: textColor *= texture(u_Textures[23], v_TextCoord * v_TilingFactor); break;
		case 24: textColor *= texture(u_Textures[24], v_TextCoord * v_TilingFactor); break;
		case 25: textColor *= texture(u_Textures[25], v_TextCoord * v_TilingFactor); break;
		case 26: textColor *= texture(u_Textures[26], v_TextCoord * v_TilingFactor); break;
		case 27: textColor *= texture(u_Textures[27], v_TextCoord * v_TilingFactor); break;
		case 28: textColor *= texture(u_Textures[28], v_TextCoord * v_TilingFactor); break;
		case 29: textColor *= texture(u_Textures[29], v_TextCoord * v_TilingFactor); break;
		case 30: textColor *= texture(u_Textures[30], v_TextCoord * v_TilingFactor); break;
		case 31: textColor *= texture(u_Textures[31], v_TextCoord * v_TilingFactor); break;
	}
	color = textColor;

	color2 = 50;
}