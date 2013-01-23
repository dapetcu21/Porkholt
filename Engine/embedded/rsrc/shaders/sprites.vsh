#ifdef proj
varying highp vec3 txc;
#else
varying highp vec2 txc;
#endif

attribute vec4 position;
#ifdef proj
attribute vec3 texCoord;
#else
attribute vec2 texCoord;
#endif

uniform mat4 modelViewProjectionMatrix;

void main(void)
{
	txc = texCoord;
	gl_Position = modelViewProjectionMatrix * position;
}
