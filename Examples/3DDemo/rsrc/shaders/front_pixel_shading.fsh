#extension GL_OES_standard_derivatives : enable

varying highp vec3 p;
varying highp vec3 n;

#ifdef texture
varying highp vec2 txc;
uniform sampler2D tex;
#endif

#ifdef normal_map
varying highp vec2 nc;
uniform sampler2D nMap;
#endif

#if defined(point_light) || defined(directional_light)
uniform highp vec3 lightPosition;
#endif


uniform lowp vec4 diffuseColor;
uniform lowp vec4 ambientColor;

#ifdef specular
uniform lowp vec4 specularColor;
uniform lowp float shininess;
#endif

#ifdef normal_map
highp vec3 adjustN(highp vec3 N, highp vec3 dev)
{
	highp vec3 q0 = dFdx(p);
	highp vec3 q1 = dFdy(p);
	highp vec2 st0 = dFdx(nc);
	highp vec2 st1 = dFdy(nc);
	highp vec3 S = normalize( q0 * st1.t - q1 * st0.t);
	highp vec3 T = normalize(-q0 * st1.s + q1 * st0.s);
	return normalize(mat3(S, -T, N) * dev);
}
#endif

void main(void)
{
#if defined(point_light) || defined(directional_light)
	
	#ifdef point_light
	highp vec3 L = lightPosition-p;
	highp float len = length(L);
	highp vec3 l = normalize(L);
	#else
	highp vec3 l = lightPosition;
	#endif
	highp vec3 N = normalize(n);
	#ifdef normal_map
	N = adjustN(N, normalize(texture2D(nMap, nc).rgb * 2.0 - 1.0));
	#endif
	highp float Idiff = max(dot(N,l),0.0) 
	#ifdef point_light
						/(len*len)
	#endif
						;
						
	
	#ifdef specular
	highp float Ispec = pow(max(dot(-reflect(l,N),normalize(-p)),0.0),shininess)
	#ifdef point_light
						/(len*len)
	#endif
						;
	gl_FragColor = (diffuseColor * Idiff + ambientColor)
	#ifdef texture
		* texture2D(tex, txc)
	#endif
		 + specularColor * Ispec;
	#else
	gl_FragColor = (diffuseColor * Idiff + ambientColor)
	#ifdef texture
		* texture2D(tex, txc)
	#endif
		;
	#endif
	
#else 
	gl_FragColor = ambientColor
	#ifdef texture
		* texture2D(tex, txc)
	#endif
		;
#endif
}
