varying highp vec2 txc;

//#define shade_only

uniform sampler2D texture;
uniform sampler2D map;

uniform highp float screenWInv, screenHInv;

void main()
{
    highp float offsetX = texture2D(map, txc + vec2(-screenWInv, 0.0)).r - texture2D(map, txc + vec2(screenWInv, 0.0)).r;
    highp float offsetY = texture2D(map, txc + vec2(0.0, -screenHInv)).r - texture2D(map, txc + vec2(0.0, screenHInv)).r;
    offsetX *= 4.0;
    offsetY *= 4.0;
    
    highp float shading = offsetX;

#ifdef shade_only
    highp float abs_shading = abs(shading);
    highp float sign = max(shading / abs_shading, 0.0);
    gl_FragColor = vec4(sign, sign, sign, abs_shading);
#else
    lowp vec3 pixel = texture2D(texture, txc + vec2(offsetX * screenWInv, offsetY * screenHInv)).rgb;
    pixel.r += shading;
    pixel.g += shading;
    pixel.b += shading;
    gl_FragColor.rgb =  pixel;
    gl_FragColor.a = 1.0;
#endif
}


