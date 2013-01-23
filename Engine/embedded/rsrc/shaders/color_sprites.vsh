#ifdef proj
varying highp vec3 txc;
#else
varying highp vec2 txc;
#endif
varying lowp vec4 c;

attribute vec4 position;
#ifdef proj
attribute vec3 texCoord;
#else
attribute vec2 texCoord;
#endif
attribute vec4 vColor;

uniform mat4 modelViewProjectionMatrix;

void main(void)
{
	txc = texCoord;
	c = vColor;
	gl_Position = modelViewProjectionMatrix * position;
}
