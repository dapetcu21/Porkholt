varying highp vec3 txc;

attribute vec4 position;
attribute vec3 texCoord;

uniform mat4 modelViewProjectionMatrix;

void main(void)
{
	txc = texCoord;
	gl_Position = modelViewProjectionMatrix * position;
}
