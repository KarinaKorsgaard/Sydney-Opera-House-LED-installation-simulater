
#ifdef GL_ES
precision mediump float;
#endif


uniform vec2 iResolution;

uniform float blurSize;
uniform float intensity;

uniform float saturation;
uniform float brightness;
uniform float contrast;

uniform sampler2D tex;


mat4 brightnessMatrix( float brightness )
{
    return mat4( 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, 1, 0,
                brightness, brightness, brightness, 1 );
}

mat4 contrastMatrix( float contrast )
{
    float t = ( 1.0 - contrast ) / 2.0;
    
    return mat4( contrast, 0, 0, 0,
                0, contrast, 0, 0,
                0, 0, contrast, 0,
                t, t, t, 1 );
    
}

mat4 saturationMatrix( float saturation )
{
    vec3 luminance = vec3( 0.3086, 0.6094, 0.0820 );
    
    float oneMinusSat = 1.0 - saturation;
    
    vec3 red = vec3( luminance.x * oneMinusSat );
    red+= vec3( saturation, 0, 0 );
    
    vec3 green = vec3( luminance.y * oneMinusSat );
    green += vec3( 0, saturation, 0 );
    
    vec3 blue = vec3( luminance.z * oneMinusSat );
    blue += vec3( 0, 0, saturation );
    
    return mat4( red,     0,
                green,   0,
                blue,    0,
                0, 0, 0, 1 );
}

vec4 GetBloom(vec2 coord)
{
    vec4 bloom = vec4(0.0);
    
    //Reconstruct bloom from multiple blurred images
    bloom += vec4(coord, 1.0, vec2((0.0))) * 1.0;
    bloom += vec4(coord, 2.0, vec2((1.0))) * 1.5;
    bloom += vec4(coord, 3.0, vec2((2.0))) * 1.0;
    bloom += vec4(coord, 4.0, vec2((3.0))) * 1.5;
    bloom += vec4(coord, 5.0, vec2((4.0))) * 1.8;
    bloom += vec4(coord, 6.0, vec2((5.0))) * 1.0;
    bloom += vec4(coord, 7.0, vec2((6.0))) * 1.0;
    bloom += vec4(coord, 8.0, vec2((7.0))) * 1.0;
    
    return bloom;
}


void main( )
{
    vec2 texcoord = gl_FragCoord.xy/iResolution.xy;

    vec4 color = brightnessMatrix( brightness ) *
    contrastMatrix( contrast ) *
    saturationMatrix( saturation ) *
    texture2D(tex,texcoord);
    
//    color += GetBloom(texcoord) * 0.18;
    
    
//    uniform float kernel[KERNEL_SIZE];
//    uniform SAMPLER_TYPE readTex;
//    uniform vec2 imageIncrement;
//    
//    varying vec2 vUv;
//    varying vec2 scaledImageIncrement;
//    
//    void main()
//    {
//        
//        vec2 imageCoord = vUv;
//        vec4 sum = vec4( 0.0, 0.0, 0.0, 0.0 );
//        
//        for( int i = 0; i < KERNEL_SIZE; i++ )
//        {
//            sum += TEXTURE_FN( readTex, imageCoord ) * kernel[ i ];
//            imageCoord += scaledImageIncrement;
//        }
//        
//        gl_FragColor = sum;
//    }

    
//
//
//    vec4 sum = vec4(0.);
//    int j;
//    int i;
//
//    //thank you! http://www.gamerendering.com/2008/10/11/gaussian-blur-filter-shader/ for the
//    //blur tutorial
//    // blur in y (vertical)
//    // take nine samples, with the distance blurSize between them
//    sum += texture(tex, vec2(texcoord.x - 4.0*blurSize, texcoord.y)) * 0.05;
//    sum += texture(tex, vec2(texcoord.x - 3.0*blurSize, texcoord.y)) * 0.09;
//    sum += texture(tex, vec2(texcoord.x - 2.0*blurSize, texcoord.y)) * 0.12;
//    sum += texture(tex, vec2(texcoord.x - blurSize, texcoord.y)) * 0.15;
//    
//    sum += texture(tex, vec2(texcoord.x, texcoord.y)) * 0.16;
//    
//    sum += texture(tex, vec2(texcoord.x + blurSize, texcoord.y)) * 0.15;
//    sum += texture(tex, vec2(texcoord.x + 2.0*blurSize, texcoord.y)) * 0.12;
//    sum += texture(tex, vec2(texcoord.x + 3.0*blurSize, texcoord.y)) * 0.09;
//    sum += texture(tex, vec2(texcoord.x + 4.0*blurSize, texcoord.y)) * 0.05;
//    
//    // blur in y (vertical)
//    // take nine samples, with the distance blurSize between them
//    sum += texture(tex, vec2(texcoord.x, texcoord.y - 4.0*blurSize)) * 0.05;
//    sum += texture(tex, vec2(texcoord.x, texcoord.y - 3.0*blurSize)) * 0.09;
//    sum += texture(tex, vec2(texcoord.x, texcoord.y - 2.0*blurSize)) * 0.12;
//    sum += texture(tex, vec2(texcoord.x, texcoord.y - blurSize)) * 0.15;
//    sum += texture(tex, vec2(texcoord.x, texcoord.y)) * 0.16;
//    sum += texture(tex, vec2(texcoord.x, texcoord.y + blurSize)) * 0.15;
//    sum += texture(tex, vec2(texcoord.x, texcoord.y + 2.0*blurSize)) * 0.12;
//    sum += texture(tex, vec2(texcoord.x, texcoord.y + 3.0*blurSize)) * 0.09;
//    sum += texture(tex, vec2(texcoord.x, texcoord.y + 4.0*blurSize)) * 0.05;
//
//
//    
//    
    
    gl_FragColor = color;
    
    
}
