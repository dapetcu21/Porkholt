#extension GL_OES_standard_derivatives : enable

varying highp vec3 p;
varying highp vec3 n;

varying highp vec2 txc;
uniform sampler2D tex;
#ifdef night_map
uniform sampler2D night;
#endif

varying highp vec2 nc;
#ifdef normal_map
uniform sampler2D nMap;
uniform highp float normalMapStrength;
#endif

uniform highp vec3 lightPosition;
uniform lowp vec4 diffuseColor;

#ifdef specular
#define shininess 5.0
#define specularColor vec4(0.5, 0.5, 0.5, 1.0)
#endif


#ifdef normal_map
highp vec3 adjustN(highp vec3 N, highp vec3 dev)
{
    dev.xy *= normalMapStrength;
	highp vec3 q0 = dFdx(p);
	highp vec3 q1 = dFdy(p);
	highp vec2 st0 = dFdx(nc);
	highp vec2 st1 = dFdy(nc);
	highp vec3 S = normalize( q0 * st1.t - q1 * st0.t);
	highp vec3 T = normalize(-q0 * st1.s + q1 * st0.s);
	return normalize(mat3(S, T, N) * dev);
}
#endif

void main(void)
{
	highp vec3 l = lightPosition; //actually lightDirection
	highp vec3 N = normalize(n);
    #ifdef normal_map
	N = adjustN(N, normalize(texture2D(nMap, nc).rgb * 2.0 - 1.0));
    #endif
	highp float Idiff = max(dot(N,l),0.0);

    #ifdef specular
	highp float Ispec = pow(max(dot(-reflect(l,N),normalize(-p)),0.0), shininess);
    #endif

    gl_FragColor = (
        diffuseColor * Idiff
    #ifdef specular
        + specularColor * Ispec 
    #endif
        )
		* texture2D(tex, txc)
    #ifdef night_map
        + texture2D(night, txc)
    #endif
        ;
}
