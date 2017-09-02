#version 330 core

layout (location = 0) out vec4 color;

struct DirectionalLight {
    vec3 position; // No longer necessery when using directional lights.
    vec3 direction;
    vec4 color;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec4 color;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
	vec3 position;
	vec3 direction;
	vec4 color;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 


// uniform vec3 light_pos;
// uniform vec3 light_color;
uniform vec3 camera_pos;

uniform int fog_enabled = 0;

// 0 is directional, 1 is point, 2 is point
uniform int light_type = 0;

uniform DirectionalLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;
//uniform Material material;

uniform float gamma = 2.2;

uniform sampler2D shadowMap;

in DATA
{
	vec3 position;
	vec3 normal;
	vec4 color;
	vec4 lightSpacePosition;
} fs_in;
	
vec3 applyFog(vec3  rgb,      // original color of the pixel
              float dist, // camera to point distance
              vec3  rayDir)   // camera to point vector
{
	float b = dist*0.005;
    float fogAmount = 1.0 - exp(-b*b);
//    float sunAmount = max( dot( rayDir, sunDir ), 0.0 );
    vec3 fogColor   = vec3(0.5,0.6,0.7); // bluish
    vec3 finalColor = rgb*fogAmount;
    return mix( rgb, fogColor, fogAmount );
}  

float computeDirectionalShadows(vec4 lightSpacePosition, float bias)
{
	vec3 proj_coord = lightSpacePosition.xyz / lightSpacePosition.w;
	proj_coord = proj_coord * 0.5 + 0.5;

	if( proj_coord.z > 1.0 ) {
		return 0.0;
	}

	float depth_curr = proj_coord.z;

	float shadow = 0.0;
	vec2 size_texel = 1.0 / textureSize( shadowMap, 0 );

	vec2 offset;

	for( offset.x = -1; offset.x <= 1; ++offset.x ) {
		for( offset.y = -1; offset.y <= 1; ++offset.y ) {
			float depth_pcf = texture( shadowMap, proj_coord.xy + offset * size_texel ).r; 
			shadow += depth_curr - bias > depth_pcf ? 1.0 : 0.0;        
		}
	}

	return shadow / 9.0;

}

vec3 computeDirectionalLight(DirectionalLight light)
{
	vec3 ambient = light.ambient * vec3(light.color);

	// diffuse light
	vec3 normal = normalize(fs_in.normal);

	vec3 light_dir = normalize(-light.direction);
	float diff = max(dot(light_dir, normal), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(light.color);

	// specular light
	vec3 camera_dir = normalize(camera_pos - fs_in.position);
	//vec3 reflect_dir = reflect(-light_dir, normal);  
	vec3 halfway_dir = normalize(light_dir + camera_dir);  
	float spec = pow(max(dot(normal, halfway_dir), 0.0), 16);
	vec3 specular =	light.specular * spec * vec3(light.color);
	
	float bias = max(0.000005f * (1.0 - dot(normal, light_dir)), 0.0000005f);
	//float bias = 0.005;
	float shadow = computeDirectionalShadows(fs_in.lightSpacePosition, bias);

	//float shadow = 0;

	return (ambient + (1.0 - shadow) * (diffuse + specular));
}

vec3 computePointLight(PointLight light)
{
	vec3 ambient = light.ambient * vec3(light.color);

	// diffuse light
	vec3 normal = normalize(fs_in.normal);	
	vec3 lightDir = normalize(light.position - fs_in.position);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(light.color);

	// specular light
	vec3 cameraDir = normalize(camera_pos - fs_in.position);
	//vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + cameraDir);  
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 16);
	vec3 specular = light.specular * spec * vec3(light.color);

	float dist = length(light.position - fs_in.position);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

vec3 computeSpotLight(SpotLight light)
{
	vec3 ambient = light.ambient * vec3(light.color);

	// diffuse light
	vec3 normal = normalize(fs_in.normal);	
	vec3 lightDir = normalize(light.position - fs_in.position);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(light.color);

	// specular light
	vec3 cameraDir = normalize(camera_pos - fs_in.position);
	//vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + cameraDir);  
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 16);
	vec3 specular = light.specular * spec * vec3(light.color);

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = (light.cutOff - light.outerCutOff);
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	diffuse *= intensity;
	specular *= intensity;

	float dist = length(light.position - fs_in.position);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
}

void main()
{
	vec3 light;

	switch (light_type) {
		default:
		case 0:
			light = computeDirectionalLight(dirLight);
			break;
		case 1:
			light = computePointLight(pointLight);
			break;
		case 2:
			light = computeSpotLight(spotLight);
			break;
	}

	// light = ambient + diffuse + specular;

	vec3 lightResult = light * vec3(fs_in.color);

	vec3 result = lightResult;

	if (fog_enabled == 1) {
		result = applyFog(lightResult, distance(camera_pos, fs_in.position), camera_pos);
	}

	float gammaAttenuation = 1 / gamma;
	result = pow(result, vec3(gammaAttenuation));

	color = vec4(result, 1.0);
}