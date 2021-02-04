attribute vec2 a_uv;
attribute vec3 a_posL;

varying vec2 v_uv;
varying vec4 v_posL;

uniform mat4 u_matrix;
uniform mat4 u_model;

void main()
{
	gl_Position = u_matrix * vec4(a_posL, 1.0);
	v_uv = a_uv;
	v_posL = u_model * vec4(a_posL, 1.0);
}