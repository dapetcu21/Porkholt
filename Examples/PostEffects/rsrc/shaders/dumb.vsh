attribute vec4 position;

uniform mat4 modelViewProjectionMatrix;

void main(void)
{
	gl_Position = modelViewProjectionMatrix * position;
}
