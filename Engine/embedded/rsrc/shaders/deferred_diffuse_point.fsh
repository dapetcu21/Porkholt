varying highp vec2 txc;
varying highp vec3 p;

uniform sampler2D texture;
uniform lowp vec4 diffuseColor;
uniform highp vec3 lightPosition;
uniform highp float lightIntensity; //z*r^2


void main(void)
{
    highp float len = length(lightPosition-p);   
    highp float intensity = lightIntensity/(len*len*len);
    
    gl_FragColor = texture2D(texture,txc) * diffuseColor * intensity;
}
