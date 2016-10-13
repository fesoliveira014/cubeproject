#version 330 core

layout (location = 0) out vec4 color;

uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 camera_pos;

uniform int fog_enabled = 0;

in DATA
{
	vec3 position;
	vec3 normal;
	vec4 color;
} fs_in;

	

vec3 applyFog(vec3  rgb,      // original color of the pixel
              float dist, // camera to point distance
              vec3  rayDir)   // camera to point vector
{
	float b = dist*0.01;
    float fogAmount = 1.0 - exp(-b*b);
//    float sunAmount = max( dot( rayDir, sunDir ), 0.0 );
    vec3  fogColor  = vec3(0.5,0.6,0.7); // bluish
    vec3 finalColor = rgb*fogAmount;
    return mix( rgb, fogColor, fogAmount );
}  

void main()
{
	// ambient light
	float ambient_strength = 1.0f;
	vec3 ambient = ambient_strength * light_color;

	// deffuse light
	vec3 normal = normalize(fs_in.normal);
	vec3 light_dir = normalize(light_pos - fs_in.position);
	float diff = max(dot(normal, light_dir), 0.0);
	vec3 diffuse = diff * light_color;

	// specular light
	float specular_strength = 0.5f;
	vec3 camera_dir = normalize(camera_pos - fs_in.position);
	vec3 reflect_dir = reflect(-light_dir, normal);  
	float spec = pow(max(dot(camera_dir, reflect_dir), 0.0), 32);
	vec3 specular =	specular_strength * spec * light_color;
	// todo

	vec3 lightResult = (ambient + diffuse + specular) * vec3(fs_in.color);

	vec3 result = lightResult;

	if (fog_enabled == 1) {
		result = applyFog(lightResult, distance(camera_pos, fs_in.position), camera_pos);
	}

	color = vec4(result, 1.0f);
}