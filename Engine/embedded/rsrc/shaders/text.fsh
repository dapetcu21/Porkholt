varying highp vec2 txc;

uniform lowp vec4 color;
uniform sampler2D texture;

void main()
{
	lowp vec4 clr = color;
	clr.a *= texture2D(texture,txc).a;
    gl_FragColor = clr;
}
