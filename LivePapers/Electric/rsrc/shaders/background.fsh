varying highp vec2 txc;

uniform lowp vec4 color;
uniform sampler2D texture;

void main()
{
    //R: mask
    //G: glow
    //B: overlay color
    //A: overlay alpha
    lowp vec4 t = texture2D(texture, txc);
    if (t.r > 0.1)
        discard;
        
    lowp vec4 prem_color = color;
    prem_color.rgb *= prem_color.a;
    
    lowp float a = 1.0 - t.b;
    gl_FragColor = vec4((vec3(t.b, t.b, t.b) + prem_color.rgb * vec3(a, a, a)) * vec3(t.b, t.b, t.b), t.a);
    gl_FragColor += vec4(color.rgb, t.g * color.a);
}
