varying highp vec2 txc;

uniform sampler2D texture;

void main()
{
    gl_FragColor = vec4(texture2D(texture,txc).a, 0, 0, 1);
}
