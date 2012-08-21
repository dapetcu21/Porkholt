varying highp vec2 txc;

uniform sampler2D texture;
uniform sampler1D cellmap;

void main()
{
    lowp vec4 clr = texture2D(texture,txc);
    gl_FragColor = vec4(texture1D(cellmap, clr.r).r,
                        texture1D(cellmap, clr.g).r,
                        texture1D(cellmap, clr.b).r,
                        clr.a);
}
