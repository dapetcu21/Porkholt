varying highp vec3 p;
varying highp vec3 n;
attribute vec4 nrm;
uniform mat4 normalMatrix;

attribute vec4 posi;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
	p = vec3(modelViewMatrix * posi);
	n = vec3(normalMatrix * nrm);
	highp vec4 pos = (projectionMatrix * modelViewMatrix) * posi;
    pos.z = pos.w;
    gl_Position = pos;
}
