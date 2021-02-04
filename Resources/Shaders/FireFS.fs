precision mediump float;

varying vec2 v_uv;
uniform sampler2D u_texture_0;
uniform sampler2D u_texture_1;
uniform sampler2D u_texture_2;
uniform float u_Time;
uniform float u_DispMax;

varying vec4 v_posL;
uniform vec3 u_cameraPos;

uniform sampler2D u_fog_texture;
uniform float r;
uniform float R;

void main()
{
	vec2 disp =  texture2D(u_texture_0, vec2(v_uv.x, v_uv.y + u_Time)).rg;
	vec2 offset = ((disp * 2.0) - 1.0) * u_DispMax;
	vec2 v_uv_displaced = v_uv + offset;
	vec4 c_fire = texture2D(u_texture_1, v_uv_displaced);
	vec4 c_alpha = texture2D(u_texture_2, v_uv);

	float d = distance(vec4(u_cameraPos, 1.0), v_posL);
	float alpha = (clamp(d, r, R) - r) / (R - r);
	vec4 culoareFog = texture2D(u_fog_texture, vec2(0.1, 0.1));

	vec4 finalColor = alpha * culoareFog + (1.0 - alpha) * c_fire;
	finalColor.a = c_fire.a * c_alpha.b;

	gl_FragColor = finalColor;
}