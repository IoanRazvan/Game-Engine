attribute vec3 a_posL;
attribute vec3 a_color;
uniform mat4 u_matrix;

varying vec4 v_color;

void main()
{
	gl_Position = u_matrix * vec4(a_posL, 1.0);
	v_color = vec4(a_color, 1.0);
}