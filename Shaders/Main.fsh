#version 110

##NEWORLD_SHADER_DEFINES MergeFace MERGE_FACE
#define SMOOTH_SHADOW

const mat4 normalize = mat4(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.499, 1.0);
const float delta = 0.05;

uniform sampler2D Tex;
uniform sampler2D DepthTex;
uniform sampler3D Tex3D;
uniform mat4 Depth_proj;
uniform mat4 Depth_modl;
uniform mat4 TransMat;
uniform vec4 SkyColor;
uniform float renderdist;

varying vec4 VertCoords;
varying float facing_float;

void main() {
	
	mat4 transf = normalize * Depth_proj * Depth_modl * TransMat;
	vec4 rel = gl_ModelViewMatrix * VertCoords;
	vec4 ShadowCoords = transf * VertCoords;
#ifdef SMOOTH_SHADOW
	vec4 ShadowCoords01;
	vec4 ShadowCoords21;
	vec4 ShadowCoords10;
	vec4 ShadowCoords12;
#endif
	int facing = int(facing_float + 0.5);
	float shadow = 0.0;
	float dist = length(rel);
	
#ifdef SMOOTH_SHADOW
	//Shadow smoothing (Super-sample)
	if (dist < 16.0) {
		if (facing == 0 || facing == 1) {
			ShadowCoords01 = transf * vec4(VertCoords.x - delta, VertCoords.yzw);
			ShadowCoords21 = transf * vec4(VertCoords.x + delta, VertCoords.yzw);
			ShadowCoords10 = transf * vec4(VertCoords.x, VertCoords.y - delta, VertCoords.zw);
			ShadowCoords12 = transf * vec4(VertCoords.x, VertCoords.y + delta, VertCoords.zw);
		}
		else if (facing == 2 || facing == 3) {
			ShadowCoords01 = transf * vec4(VertCoords.x, VertCoords.y - delta, VertCoords.zw);
			ShadowCoords21 = transf * vec4(VertCoords.x, VertCoords.y + delta, VertCoords.zw);
			ShadowCoords10 = transf * vec4(VertCoords.xy, VertCoords.z - delta, VertCoords.w);
			ShadowCoords12 = transf * vec4(VertCoords.xy, VertCoords.z + delta, VertCoords.w);
		}
		else if (facing == 4 || facing == 5) {
			ShadowCoords01 = transf * vec4(VertCoords.x - delta, VertCoords.yzw);
			ShadowCoords21 = transf * vec4(VertCoords.x + delta, VertCoords.yzw);
			ShadowCoords10 = transf * vec4(VertCoords.xy, VertCoords.z - delta, VertCoords.w);
			ShadowCoords12 = transf * vec4(VertCoords.xy, VertCoords.z + delta, VertCoords.w);
		}
	}
#endif
	
	//Shadow calculation
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
#ifdef MERGE_FACE
	vec4 texel = texture3D(Tex3D, gl_TexCoord[0].stp);
#else
	vec4 texel = texture2D(Tex, gl_TexCoord[0].st);
#endif

	vec4 color = vec4(texel.rgb * shadow, texel.a) * gl_Color;
	
	//Fog calculation & Final color
	//if (color.a < 0.99) color = vec4(color.rgb, mix(1.0, 0.3, clamp((renderdist * 0.5 - dist) / 64.0, 0.0, 1.0)));
	gl_FragColor = mix(SkyColor, color, clamp((renderdist - dist) / 32.0, 0.0, 1.0));
}