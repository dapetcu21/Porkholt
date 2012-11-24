varying highp vec3 txc;

uniform lowp vec4 color;
uniform sampler2D texture;

void main()
{
    gl_FragColor = texture2DProj(texture,txc) * color;
}
