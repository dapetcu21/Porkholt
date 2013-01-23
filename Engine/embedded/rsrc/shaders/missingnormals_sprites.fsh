#ifdef proj
varying highp vec3 txc;
#else
varying highp vec2 txc;
#endif

uniform sampler2D texture;

void main()
{
#ifdef proj
    gl_FragColor = vec4(0.5,0.5,1.0,texture2DProj(texture,txc).a);
#else
    gl_FragColor = vec4(0.5,0.5,1.0,texture2D(texture,txc).a);
#endif

}
