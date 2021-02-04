precision mediump float;

uniform samplerCube u_texture_0;
varying vec3 v_coord;

void main()
{
	gl_FragColor = textureCube(u_texture_0, v_coord);
}