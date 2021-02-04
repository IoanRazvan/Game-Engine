attribute vec3 a_posL;
attribute vec3 a_color;
 
varying vec3 v_color;

uniform mat4 u_matrix;

void main()
{
vec4 posL = u_matrix * vec4(a_posL, 1.0);
gl_Position = posL;
v_color = a_color;
}
   