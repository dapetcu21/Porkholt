varying highp vec2 txc;

uniform sampler2D texture;
uniform lowp vec4 color;

void main(void)
{
    gl_FragColor = texture2D(texture,txc) * color;
}
