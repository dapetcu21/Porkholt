varying highp vec2 txc;
varying highp vec3 p;

attribute vec4 posi;

uniform mat4 modelViewProjectionMatrix;
uniform vec2 scale;
uniform vec4 clip;

void main(void)
{
	vec4 ps = posi * vec4(clip.zw,1.0,1.0) + vec4(clip.xy,0.0,0.0);
	vec4 pos = ps * vec4(scale,1.0,1.0);
	txc = ps.xy;
    p = pos.xyz;
	gl_Position = modelViewProjectionMatrix * pos;
}