varying highp vec2 txc;

attribute vec4 position;
attribute vec2 texCoord;

void main(void)
{
	txc = texCoord;
	gl_Position = position;
}
