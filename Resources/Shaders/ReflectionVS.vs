attribute vec3 a_posL;
attribute vec3 a_norm;

uniform mat4 u_model;
uniform mat4 u_matrix;

varying vec4 v_Wpos;
varying vec4 v_Wnorm;
varying vec4 v_posL;

void main()
{
	v_Wpos = u_model * vec4(a_posL, 1.0);
	v_Wnorm = u_model * vec4(a_norm, 0.0);
	gl_Position = u_matrix * vec4(a_posL, 1.0);

	v_posL = vec4(a_posL, 1.0);
}