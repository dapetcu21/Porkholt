varying highp vec2 txc;

uniform sampler2D texture;

void main()
{
    gl_FragColor = texture2D(texture,txc);
}
