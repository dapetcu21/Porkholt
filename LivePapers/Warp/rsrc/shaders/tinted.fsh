varying highp vec2 txc;

uniform lowp vec4 color;
uniform lowp vec4 color2;
uniform sampler2D texture;

void main()
{
    lowp vec4 t = texture2D(texture, txc);
    gl_FragColor = vec4(t.b, t.b, t.b, 1.0) * color + vec4(t.r, t.r, t.r, 1.0) * color2;
}
