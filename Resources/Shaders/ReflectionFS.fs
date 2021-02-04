precision mediump float;

varying vec4 v_Wpos;
varying vec4 v_Wnorm;

uniform vec3 u_cameraPos;
uniform samplerCube u_texture_0;
uniform sampler2D u_fog_texture;
uniform float R;
uniform float r;

varying vec4 v_posL;

void main()
{
	vec4 vectCam = vec4(u_cameraPos,1.0) - v_Wpos;
	vec4 dirReflect = reflect(normalize(vectCam), normalize(v_Wnorm));
	vec4 intermediate_color = textureCube(u_texture_0, dirReflect.stp);
	
	float d = distance(vec4(u_cameraPos, 1.0), v_posL);
	float alpha = (clamp(d, r, R) - r) / (R - r);
	vec4 culoareFog = texture2D(u_fog_texture, vec2(0.1,0.1));

	gl_FragColor = alpha * culoareFog + (1.0 - alpha) * intermediate_color;
}