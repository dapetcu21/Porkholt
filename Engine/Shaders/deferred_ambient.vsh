varying highp vec2 txc;

attribute vec4 pos;

uniform mat4 modelViewProjectionMatrix;
uniform vec2 scale;

void main(void)
{
	txc = pos.xy;
	gl_Position = modelViewProjectionMatrix * (pos * vec4(scale,1.0,1.0));
}