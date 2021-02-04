precision mediump float;

varying vec2 v_uv;
varying vec4 v_posL;

uniform sampler2D u_texture_0;
uniform vec3 u_cameraPos; 
uniform sampler2D u_fog_texture; // fog texture
uniform float r;
uniform float R;

void main()
{
	float d = distance(vec4(u_cameraPos, 1.0), v_posL);
	vec4 culoareObiect = texture2D(u_texture_0, v_uv);
	vec4 fogColor = texture2D(u_fog_texture, vec2(0.5, 0.5));
	float alpha = (clamp(d, r, R) - r) / (R - r);
	if (culoareObiect.w < 0.5)
		discard;
	gl_FragColor = alpha * fogColor + (1.0-alpha) * culoareObiect;
}