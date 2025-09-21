// Basic Texture Shader

#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TextCoord;
layout(location = 3) in float a_TextIndex;
layout(location = 4) in float a_TilingFactor;
layout(location = 5) in int a_EntityID;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjection;
};

struct VertexOutput
{
	vec4 Color;
	vec2 TextCoord;
	float TilingFactor;
};

layout (location = 0) out VertexOutput Output;
layout (location = 3) out flat float v_TextIndex;
layout (location = 4) out flat int v_EntityID;

void main()
{
	Output.Color = a_Color;
	Output.TextCoord = a_TextCoord;
	Output.TilingFactor = a_TilingFactor;
	v_TextIndex = a_TextIndex;
	v_EntityID = a_EntityID;

	gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 1) out int o_EntityID;

struct VertexOutput
{
	vec4 Color;
	vec2 TextCoord;
	float TilingFactor;
};

layout (location = 0) in VertexOutput Input;
layout (location = 3) in flat float v_TextIndex;
layout (location = 4) in flat int v_EntityID;

layout (binding = 0) uniform sampler2D u_Textures[32];

void main()
{
	vec4 textColor = Input.Color;

	if (textColor.a < 0.1) discard;

	switch(int(v_TextIndex))
	{
		case  0: textColor *= texture(u_Textures[ 0], Input.TextCoord * Input.TilingFactor); break;
		case  1: textColor *= texture(u_Textures[ 1], Input.TextCoord * Input.TilingFactor); break;
		case  2: textColor *= texture(u_Textures[ 2], Input.TextCoord * Input.TilingFactor); break;
		case  3: textColor *= texture(u_Textures[ 3], Input.TextCoord * Input.TilingFactor); break;
		case  4: textColor *= texture(u_Textures[ 4], Input.TextCoord * Input.TilingFactor); break;
		case  5: textColor *= texture(u_Textures[ 5], Input.TextCoord * Input.TilingFactor); break;
		case  6: textColor *= texture(u_Textures[ 6], Input.TextCoord * Input.TilingFactor); break;
		case  7: textColor *= texture(u_Textures[ 7], Input.TextCoord * Input.TilingFactor); break;
		case  8: textColor *= texture(u_Textures[ 8], Input.TextCoord * Input.TilingFactor); break;
		case  9: textColor *= texture(u_Textures[ 9], Input.TextCoord * Input.TilingFactor); break;
		case 10: textColor *= texture(u_Textures[10], Input.TextCoord * Input.TilingFactor); break;
		case 11: textColor *= texture(u_Textures[11], Input.TextCoord * Input.TilingFactor); break;
		case 12: textColor *= texture(u_Textures[12], Input.TextCoord * Input.TilingFactor); break;
		case 13: textColor *= texture(u_Textures[13], Input.TextCoord * Input.TilingFactor); break;
		case 14: textColor *= texture(u_Textures[14], Input.TextCoord * Input.TilingFactor); break;
		case 15: textColor *= texture(u_Textures[15], Input.TextCoord * Input.TilingFactor); break;
		case 16: textColor *= texture(u_Textures[16], Input.TextCoord * Input.TilingFactor); break;
		case 17: textColor *= texture(u_Textures[17], Input.TextCoord * Input.TilingFactor); break;
		case 18: textColor *= texture(u_Textures[18], Input.TextCoord * Input.TilingFactor); break;
		case 19: textColor *= texture(u_Textures[19], Input.TextCoord * Input.TilingFactor); break;
		case 20: textColor *= texture(u_Textures[20], Input.TextCoord * Input.TilingFactor); break;
		case 21: textColor *= texture(u_Textures[21], Input.TextCoord * Input.TilingFactor); break;
		case 22: textColor *= texture(u_Textures[22], Input.TextCoord * Input.TilingFactor); break;
		case 23: textColor *= texture(u_Textures[23], Input.TextCoord * Input.TilingFactor); break;
		case 24: textColor *= texture(u_Textures[24], Input.TextCoord * Input.TilingFactor); break;
		case 25: textColor *= texture(u_Textures[25], Input.TextCoord * Input.TilingFactor); break;
		case 26: textColor *= texture(u_Textures[26], Input.TextCoord * Input.TilingFactor); break;
		case 27: textColor *= texture(u_Textures[27], Input.TextCoord * Input.TilingFactor); break;
		case 28: textColor *= texture(u_Textures[28], Input.TextCoord * Input.TilingFactor); break;
		case 29: textColor *= texture(u_Textures[29], Input.TextCoord * Input.TilingFactor); break;
		case 30: textColor *= texture(u_Textures[30], Input.TextCoord * Input.TilingFactor); break;
		case 31: textColor *= texture(u_Textures[31], Input.TextCoord * Input.TilingFactor); break;
	}

	o_Color = textColor;

	o_EntityID = v_EntityID;
}
