varying highp vec3 p;
varying highp vec3 n;
attribute vec4 nrm;
uniform mat4 normalMatrix;

varying highp vec2 txc;
attribute vec4 texCoord;
#ifdef normal_map
varying highp vec2 nc;
#endif

attribute vec4 posi;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
	p = vec3(modelViewMatrix * posi);
	n = vec3(normalMatrix * nrm);
	txc = vec2(texCoord.x, texCoord.y);
#ifdef normal_map
	nc = vec2(texCoord.x, texCoord.y);
#endif
	gl_Position = (projectionMatrix * modelViewMatrix) * posi;
}
