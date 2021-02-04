attribute vec3 a_posL;
varying vec3 v_coord;

uniform mat4 u_matrix;

void main()
{
	gl_Position = u_matrix * vec4(a_posL, 1.0);
	v_coord = a_posL;
}