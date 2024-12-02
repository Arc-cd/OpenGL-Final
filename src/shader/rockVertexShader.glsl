#version 450 core
layout(location=0) in vec3 v_vertex;
layout(location=1) in vec3 v_normal ;
layout(location=2) in vec2 v_uv ;
layout(location=6) in vec3 aTangent;


out vec3 f_viewVertex ;
out vec3 f_viewNormal ;
out vec3 f_worldVertex ;
out vec3 f_worldNormal ;
out vec2 f_uv ;
out mat3 TBN;  // Tangent-Bitangent-Normal


uniform mat4 projMat ;
uniform mat4 viewMat ;
uniform mat4 modelMat ;
uniform vec4 v_worldPosOffset; 


out VS_OUT
{
vec3 N;
vec3 L;
vec3 V;
vec3 eyeDir;
vec3 lightDir;
} vs_out;

// Position of light
uniform vec3 light_pos = vec3(0.0, 0.0, 100.0);


void main() {
	// Calculate view-space coordinate
	vec4 P = modelMat * vec4(v_vertex, 1.0);
	// Calculate normal in view-space
	// Using only the rotational components of mv_matrix
	vs_out.N = mat3(modelMat) * v_normal;
	// Calculate light vector
	vs_out.L = light_pos - P.xyz;
	// Calculate view vector
	vs_out.V = -P.xyz;
	
	//Normal Mapping
	mat4 mv_matrix = viewMat * modelMat;

	vec3 T = normalize(mat3(mv_matrix) * aTangent);
	vec3 N = normalize(mat3(mv_matrix) * v_normal);
	vec3 B = cross(N, T);

	vec3 L = light_pos - P.xyz;
	vec3 V = -P.xyz;

	vs_out.lightDir = normalize(vec3(dot(L, T), dot(L, B), dot(L, N)));
	vs_out.eyeDir = normalize(vec3(dot(V, T), dot(V, B), dot(V, N)));





	vec4 worldVertex = modelMat * vec4(v_vertex, 1.0) + v_worldPosOffset ;
	// the vertex¡¦s w must be 1
	worldVertex.w = 1.0;
	vec4 worldNormal = modelMat * vec4(v_normal, 0.0) ;
	f_worldVertex = worldVertex.xyz ;
	f_worldNormal = worldNormal.xyz ;
	vec4 viewVertex = viewMat * worldVertex ;
	vec4 viewNormal = viewMat * worldNormal;
	f_viewVertex = viewVertex.xyz ;
	f_viewNormal = viewNormal.xyz ;
	f_uv = v_uv ;
	gl_Position = projMat * viewVertex ;
	//gl_Position = vec4(1.0,1.0,1.0,1.0) ;

}