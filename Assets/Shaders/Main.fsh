#version 450 compatibility
#define SMOOTH_SHADOW

const mat4 normalize = mat4(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.499, 1.0);
const float delta = 0.05;

uniform sampler2D Tex;
uniform sampler2D DepthTex;
uniform mat4 Depth_proj;
uniform mat4 Depth_modl;
uniform mat4 TransMat;
uniform vec4 SkyColor;
uniform float renderdist;

in vOut {
	vec2 fTexCrood;
	vec3 fShade;
	vec3 fCrood;
	flat int facing;
};
out vec4 fragment;

void main() {
	vec4 vertex = vec4(fCrood, 1.0);
	mat4 transf = normalize * Depth_proj * Depth_modl * TransMat;
	vec4 ShadowCoords = transf * vertex;
#ifdef SMOOTH_SHADOW
	vec4 ShadowCoords01;
	vec4 ShadowCoords21;
	vec4 ShadowCoords10;
	vec4 ShadowCoords12;
	float dist = length(gl_ModelViewMatrix * vertex);
	//Shadow smoothing (Super-sample)
	if (dist < 16.0) {
		if (facing == 0 || facing == 1) {
			ShadowCoords01 = transf * vec4(vertex.x - delta, vertex.yzw);
			ShadowCoords21 = transf * vec4(vertex.x + delta, vertex.yzw);
			ShadowCoords10 = transf * vec4(vertex.x, vertex.y - delta, vertex.zw);
			ShadowCoords12 = transf * vec4(vertex.x, vertex.y + delta, vertex.zw);
		}
		else if (facing == 2 || facing == 3) {
			ShadowCoords01 = transf * vec4(vertex.x, vertex.y - delta, vertex.zw);
			ShadowCoords21 = transf * vec4(vertex.x, vertex.y + delta, vertex.zw);
			ShadowCoords10 = transf * vec4(vertex.xy, vertex.z - delta, vertex.w);
			ShadowCoords12 = transf * vec4(vertex.xy, vertex.z + delta, vertex.w);
		}
		else if (facing == 4 || facing == 5) {
			ShadowCoords01 = transf * vec4(vertex.x - delta, vertex.yzw);
			ShadowCoords21 = transf * vec4(vertex.x + delta, vertex.yzw);
			ShadowCoords10 = transf * vec4(vertex.xy, vertex.z - delta, vertex.w);
			ShadowCoords12 = transf * vec4(vertex.xy, vertex.z + delta, vertex.w);
		}
	}
#endif

	//Shadow calculation
	float shadow = 0.0;
	if (facing == 1 || facing == 2 || facing == 5) shadow = 0.5;
	else if (ShadowCoords.x >= 0.0 && ShadowCoords.x <= 1.0 &&
			 ShadowCoords.y >= 0.0 && ShadowCoords.y <= 1.0 && ShadowCoords.z <= 1.0) {
		if (ShadowCoords.z < texture2D(DepthTex, ShadowCoords.xy).z) shadow += 1.2; else shadow += 0.5;
#ifdef SMOOTH_SHADOW
		if (dist < 16.0) {
			if (ShadowCoords01.z < texture2D(DepthTex, ShadowCoords01.xy).z) shadow += 1.2; else shadow += 0.5;
			if (ShadowCoords21.z < texture2D(DepthTex, ShadowCoords21.xy).z) shadow += 1.2; else shadow += 0.5;
			if (ShadowCoords10.z < texture2D(DepthTex, ShadowCoords10.xy).z) shadow += 1.2; else shadow += 0.5;
			if (ShadowCoords12.z < texture2D(DepthTex, ShadowCoords12.xy).z) shadow += 1.2; else shadow += 0.5;
			shadow *= 0.2;
		}
#endif
	}
	else shadow = 1.2;
	
	//Texture color
	vec4 texel = texture2D(Tex, fTexCrood);
	vec4 color = vec4(texel.rgb * shadow * fShade, texel.a);
	
	//Fog calculation & Final color
	//if (color.a < 0.99) color = vec4(color.rgb, mix(1.0, 0.3, clamp((renderdist * 0.5 - dist) / 64.0, 0.0, 1.0)));
	fragment = mix(SkyColor, color, clamp((renderdist - dist) / 32.0, 0.0, 1.0));
}