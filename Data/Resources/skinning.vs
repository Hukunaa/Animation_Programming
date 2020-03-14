
/////////////////////
// INPUT VARIABLES //
/////////////////////
in lowp vec3 inputPosition;
in lowp vec3 normal;
in lowp vec4 boneIndices;
in lowp vec4 boneWeights;

//////////////////////
// OUTPUT VARIABLES //
//////////////////////
smooth out vec2 texCoord;
smooth out vec3 outNormal;
out vec3 weight;

uniform SceneMatrices
{
	uniform mat4 projectionMatrix;
} sm;

uniform mat4 modelViewMatrix;

uniform SkinningMatrices
{
	uniform mat4 mat[64];
} skin;



////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
void main(void)
{
	// Calculate the position of the vertex against the world, view, and projection matrices.
	
	vec4 pos = (boneWeights.x * (vec4(inputPosition, 1.0) * skin.mat[int(boneIndices.x)]))
	+ (boneWeights.y * (vec4(inputPosition, 1.0) * skin.mat[int(boneIndices.x)]))
	+ (boneWeights.z * (vec4(inputPosition, 1.0) * skin.mat[int(boneIndices.x)]))
	+ (boneWeights.w * (vec4(inputPosition, 1.0) * skin.mat[int(boneIndices.x)]));
	
	//vec4(inputPosition, 1.0f);
	
	gl_Position = sm.projectionMatrix * (modelViewMatrix * vec4(pos.xyz, 1.0f));
	weight = vec3(boneWeights.x, boneWeights.y, boneWeights.z);
	outNormal = normalize(normal);
}
