varying highp vec2 txc;
varying highp vec3 p;

uniform sampler2D texture;
uniform sampler2D normalMap;
uniform lowp vec4 diffuseColor;
uniform highp vec3 lightPosition;
uniform highp float lightIntensity; //r^2

void main(void)
{
	highp vec3 L = lightPosition-p;
    highp float len = length(L);
   	highp vec3 N = normalize( texture2D(normalMap,txc).xyz * 2.0 - 1.0 );
    highp float intensity = lightIntensity/(len*len) * max(dot(N,normalize(L)),0.0);
    
    gl_FragColor = texture2D(texture,txc) * diffuseColor * intensity;
}
