varying highp vec2 txc;
varying lowp vec4 c;

attribute vec4 position;
attribute vec2 texCoord;
attribute vec4 vColor;

uniform mat4 modelViewProjectionMatrix;

void main(void)
{
	txc = texCoord;
	c = vColor;
	gl_Position = modelViewProjectionMatrix * position;
}