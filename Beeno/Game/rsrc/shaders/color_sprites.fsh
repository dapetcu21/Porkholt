varying highp vec2 txc;
varying lowp vec4 c;

uniform sampler2D texture;

void main()
{
    gl_FragColor = c * texture2D(texture,txc);
}
