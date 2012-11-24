varying highp vec2 txc;

uniform sampler2D mask;
uniform sampler2D oldmask;
uniform sampler2D map;
uniform sampler2D map2;

uniform highp float screenW, screenH;

void main()
{
    highp float screenWInv = 1.0/screenW;
    highp float screenHInv = 1.0/screenH;

    highp float sum = 
        texture2D(map, vec2(txc.x + screenWInv, txc.y)).r +
        texture2D(map, vec2(txc.x - screenWInv, txc.y)).r +
        texture2D(map, vec2(txc.x, txc.y + screenHInv)).r +
        texture2D(map, vec2(txc.x, txc.y - screenHInv)).r ;

    highp float delta = (texture2D(mask, txc).r - texture2D(oldmask, txc).r);
    delta *= 10.0/60.0;
    delta += (sum * 2.0 - 2.0 - texture2D(map2, txc).r * 4.0)*0.98;
    gl_FragColor = vec4((delta + 2.0)* 0.25, 0.0, 0.0, 1.0); 
}



