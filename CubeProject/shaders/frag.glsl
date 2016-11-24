#version 330 core

layout (location = 0) out vec4 color;

struct DirectionalLight {
    // vec3 position; // No longer necessery when using directional lights.
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
	float b = dist*0.005;
    float fogAmount = 1.0 - exp(-b*b);
//    float sunAmount = max( dot( rayDir, sunDir ), 0.0 );
    vec3 fogColor   = vec3(0.5,0.6,0.7); // bluish
    vec3 finalColor = rgb*fogAmount;
    return mix( rgb, fogColor, fogAmount );
}  

vec3 computeDirectionalLight(DirectionalLight light)
{

	vec3 ambient = light.ambient * vec3(light.color);

	// diffuse light
	vec3 normal = normalize(fs_in.normal);

	vec3 light_dir = normalize(-light.direction);
	float diff = max(dot(normal, light_dir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(light.color);

	// specular light
	vec3 camera_dir = normalize(camera_pos - fs_in.position);
	vec3 reflect_dir = reflect(-light_dir, normal);  
	float spec = pow(max(dot(camera_dir, reflect_dir), 0.0), 32);
	vec3 specular =	light.specular * spec * vec3(light.color);
	// todo

	return (ambient + diffuse + specular);
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
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(cameraDir, reflectDir), 0.0), 32);
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
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(cameraDir, reflectDir), 0.0), 32);
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
	// ambient light
	// float ambient_strength = 0.8;
	// vec3 ambient = ambient_strength * light_color;

	// // deffuse light
	// vec3 normal = normalize(fs_in.normal);
	// vec3 light_dir = normalize(light_pos - fs_in.position);
	// float diff = max(dot(normal, light_dir), 0.0);
	// vec3 diffuse = 0.5 * diff * light_color;

	// // specular light
	// float specular_strength = 0.2;
	// vec3 camera_dir = normalize(camera_pos - fs_in.position);
	// vec3 reflect_dir = reflect(-light_dir, normal);  
	// float spec = pow(max(dot(camera_dir, reflect_dir), 0.0), 32);
	// vec3 specular =	specular_strength * spec * light_color;
	// // todo

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

	color = vec4(result, 1.0);
}