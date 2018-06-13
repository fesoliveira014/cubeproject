#version 330 core
in vec2 v_uv;

uniform sampler2D textureSource;
uniform vec2 scaleU;
//uniform vec2 viewport;
//uniform int filter_type = 0;

out vec4 outColor;

void main()
{
	vec4 color = vec4(0.0);
	color += texture2D( textureSource, v_uv.st + vec2( -3.0*scaleU.x, -3.0*scaleU.y ) ) * 0.015625;
	color += texture2D( textureSource, v_uv.st + vec2( -2.0*scaleU.x, -2.0*scaleU.y ) )*0.09375;
	color += texture2D( textureSource, v_uv.st + vec2( -1.0*scaleU.x, -1.0*scaleU.y ) )*0.234375;
	color += texture2D( textureSource, v_uv.st + vec2( 0.0 , 0.0) )*0.3125;
	color += texture2D( textureSource, v_uv.st + vec2( 1.0*scaleU.x,  1.0*scaleU.y ) )*0.234375;
	color += texture2D( textureSource, v_uv.st + vec2( 2.0*scaleU.x,  2.0*scaleU.y ) )*0.09375;
	color += texture2D( textureSource, v_uv.st + vec2( 3.0*scaleU.x, -3.0*scaleU.y ) ) * 0.015625;
	outColor = vec4(color.xyz, 1.0);
};

// vec3 get(float x, float y)
// {
// 	vec2 off = vec2(x,y);
// 	return texture2D(textureSource, v_uv + off / viewport).rgb;
// }

// vec3 get(int x, int y)
// {
// 	vec2 off = vec2(x,y);
// 	return texture2D(textureSource, v_uv + off / viewport).rgb;
// }

// vec3 downsample128()
// {
// 	return get(0.0, 0.0);
// }

// vec3 downsample64()
// {
// 	return get(0.0, 0.0);
// }

// vec3 box_filter()
// {
// 	vec3 result = vec3(0.0);

//     for(int x =- 1; x <= 1; x++) {
//         for(int y =- 1; y <= 1; y++) {
//             result += get(x,y);
//         }
//     }

//     return result/9.0;
// }

// vec3 filter(int filter)
// {
// 	switch(filter) {
// 	case 0:
// 		return box_filter();
// 	case 1:
// 		return downsample64();
// 	case 2:
// 		return downsample128();
// 	}
// }

// void main()
// {
// 	gl_FragColor = vec4(filter(filter_type), 1.0);
// }
