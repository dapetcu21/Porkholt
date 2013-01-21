#if defined(point_light) || defined(directional_light)
varying highp vec3 p;
varying highp vec3 n;
attribute vec4 nrm;
uniform mat4 normalMatrix;
#endif

#ifdef texture
varying highp vec2 txc;
attribute vec4 texCoord;
uniform vec4 texR;
#ifdef normal_map
varying highp vec2 nc;
uniform vec4 nMapR;
#endif
#endif

attribute vec4 posi;

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main(void)
{
#if defined(point_light) || defined(directional_light)
	p = vec3(modelViewMatrix * posi);
	n = vec3(normalMatrix * nrm);
#endif
#ifdef texture
	txc = vec2(texR.x + texCoord.x * texR.z, texR.y + texCoord.y * texR.w);
#endif
#ifdef normal_map
	nc = vec2(nMapR.x + texCoord.x * nMapR.z, nMapR.y + texCoord.y * nMapR.w);
#endif
	gl_Position = (projectionMatrix * modelViewMatrix) * posi;
}
