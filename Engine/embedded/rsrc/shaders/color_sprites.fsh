#ifdef proj
varying highp vec3 txc;
#else
varying highp vec2 txc;
#endif

varying lowp vec4 c;

uniform sampler2D texture;

void main()
{
#ifdef proj
    gl_FragColor = texture2DProj(texture,txc) * c;
#else
    gl_FragColor = texture2D(texture,txc) * c;
#endif
}
