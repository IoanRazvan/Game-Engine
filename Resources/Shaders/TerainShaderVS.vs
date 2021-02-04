attribute vec3 a_posL;
attribute vec3 a_color;
attribute vec2 a_uv;
attribute vec2 a_uv2;

uniform mat4 u_matrix;
uniform vec3 u_height;
uniform mat4 u_model;
uniform sampler2D u_texture_0;
uniform float u_numberOfCells;
uniform vec2 u_offset;

varying vec3 v_color;
varying vec4 v_posL;
varying vec2 v_uv;
varying vec2 v_uv2;

void main()
{
	vec4 pos_nou = vec4(a_posL, 1.0);
	vec4 c_blend = texture2D(u_texture_0, (a_uv2 + u_offset / u_numberOfCells));
	pos_nou.y += c_blend.r * u_height.r + c_blend.g * u_height.g + c_blend.b * u_height.b;

	gl_Position = u_matrix * pos_nou;
	
	v_color = a_color;
	v_uv = a_uv;
	v_uv2 = a_uv2 + u_offset / u_numberOfCells;
	v_posL = u_model * vec4(a_posL, 1.0);
}