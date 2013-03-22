varying highp vec3 p;
varying highp vec3 n;

uniform lowp vec4 color;
uniform highp vec3 lightPosition;

void main(void)
{
	highp vec3 N = normalize(n);
    highp float intens = abs(dot(normalize(-p), N));
	highp float idiff = max(dot(N, lightPosition),0.0);
    intens = max((intens - 0.1) * 0.6, 0.0);
    gl_FragColor = color;
    gl_FragColor.a = intens * idiff;
}
