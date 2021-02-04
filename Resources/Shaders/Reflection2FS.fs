precision mediump float;

varying vec4 v_Wpos;
varying vec4 v_Wnorm;
varying vec2 v_uv;
varying vec4 v_posL;

uniform vec3 u_cameraPos;
uniform samplerCube u_texture_0;
uniform sampler2D u_texture_1;
uniform float r;
uniform float R;
uniform sampler2D u_fog_texture;

void main()
{
	vec4 vectCam = vec4(u_cameraPos,1.0) - v_Wpos;
	vec4 dirReflect = reflect(normalize(vectCam), normalize(v_Wnorm));
	vec4 c_reflexie = textureCube(u_texture_0, dirReflect.stp);
	vec4 c_proprie = texture2D(u_texture_1, v_uv);
	vec4 c_final = c_reflexie * 0.5 + c_proprie * 0.5;
	c_final.a = 1.0;

	float d = distance(vec4(u_cameraPos, 1.0), v_posL);
	float alpha = (clamp(d, r, R) - r) / (R - r);
	vec4 culoareFog = texture2D(u_fog_texture, vec2(0.1, 0.1)); // random uv coordinates bc the fog has a uniform color
	gl_FragColor = culoareFog * alpha + c_final * (1.0 - alpha);
}