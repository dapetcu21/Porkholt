varying highp vec2 txc;

uniform lowp vec4 color;
uniform sampler2D texture;

void main()
{
    gl_FragColor = texture2D(texture,txc) * color;
}
