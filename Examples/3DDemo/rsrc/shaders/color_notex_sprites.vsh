varying lowp vec4 c;

attribute vec4 position;
attribute vec4 vColor;

uniform mat4 modelViewProjectionMatrix;

void main(void)
{
	c = vColor;
	gl_Position = modelViewProjectionMatrix * position;
}