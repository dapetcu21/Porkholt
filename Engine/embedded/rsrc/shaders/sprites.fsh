#ifdef proj
varying highp vec3 txc;
#else
varying highp vec2 txc;
#endif

uniform lowp vec4 color;
uniform sampler2D texture;

void main()
{
#ifdef proj
    gl_FragColor = texture2DProj(texture,txc) * color;
#else
    gl_FragColor = texture2D(texture,txc) * color;
#endif
}
