#include "gx_shader_geometry_pass_common.hlsl"

cbuffer per_object : register(SLOT_PER_DRAW_CALL)
{
	row_major float4x4 m_w; 
	row_major float4x4 m_normal_transform; 
}

struct vs_input
{
	half4	position_os	: position;
	half4	normal_os	: normal;
	half2	uv			: texcoord;
};

struct vs_output
{
    float4	position_ps	: sv_position; 
	float3  normal_vs	: normal;
	float2	uv			: texcoord; 
};

vs_output main( in  vs_input input)
{
	vs_output output;

	//get rid of translation (assume no scaling)
	float4x4 normal_transform = m_normal_transform;
	normal_transform[3] = float4(0.0f, 0.0f, 0.0f, 1.0f);

	output.position_ps	=   project_vertex(input.position_os, m_w, m_v, m_p);
	output.uv			=	float2(input.uv);
	output.normal_vs	=	mul ( float4(input.normal_os.xyz, 0.0f), normal_transform ).xyz;
    
	return output;
}

