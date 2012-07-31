varying highp vec2 txc;

uniform lowp vec4 color;
uniform sampler2D texture;

void main()
{
    lowp vec4 tex = texture2D(texture, txc);
    lowp float alpha = 1.0-tex.r;
    gl_FragColor = vec4(color.rgb * vec3(1.0-alpha, 1.0-alpha, 1.0-alpha), tex.a);
}
