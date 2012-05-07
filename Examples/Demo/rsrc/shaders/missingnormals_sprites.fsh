varying highp vec2 txc;

uniform sampler2D texture;

void main()
{
    gl_FragColor = vec4(0.5,0.5,1.0,texture2D(texture,txc).a);
}
