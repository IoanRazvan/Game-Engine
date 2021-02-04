precision mediump float;

varying vec3 v_color;
varying vec2 v_uv;
varying vec2 v_uv2;
varying vec4 v_posL;

uniform sampler2D u_texture_0;
uniform sampler2D u_texture_1;
uniform sampler2D u_texture_2;
uniform sampler2D u_texture_3;
uniform vec3 u_cameraPos;
uniform float r;
uniform float R;
uniform sampler2D u_fog_texture;

void main()
{
	float d = distance(vec4(u_cameraPos, 1.0), v_posL);
	float alpha = (clamp(d, r, R) - r) / (R - r);
	vec4 c_blend = texture2D(u_texture_0, v_uv2);
	vec4 c_r = texture2D(u_texture_1, v_uv);
	vec4 c_g = texture2D(u_texture_2, v_uv);
	vec4 c_b = texture2D(u_texture_3, v_uv);
	vec4 culoare_fog = texture2D(u_fog_texture, v_uv);
	
	c_blend = c_blend.r * c_r + c_blend.g * c_g + c_blend.b * c_b;
	c_blend.a = 1.0;

	culoare_fog.a = 1.0;
	gl_FragColor = alpha * culoare_fog + (1.0 - alpha) * c_blend;
}