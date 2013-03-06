varying highp vec2 txc;

attribute vec4 position;
attribute vec2 texCoord;

uniform mat4 modelViewProjectionMatrix;

void main(void)
{
	txc = texCoord;
	gl_Position = modelViewProjectionMatrix * position;
}
