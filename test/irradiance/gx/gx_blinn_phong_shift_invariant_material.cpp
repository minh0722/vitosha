#include "precompiled.h"

#include <gx/gx_blinn_phong_shift_invariant_material.h>

#include <dx11/dx11_error.h>

#include <gx/gx_draw_call_context.h>
#include <gx/gx_material_database.h>
#include <gx/gx_render_context.h>

namespace gx
{
	void blinn_phong_shift_invairant_material::apply(draw_call_context* draw_call_context)
	{
		auto device_context = draw_call_context->m_device_context;

        std::get<1>(m_vertex_pipeline).set_wvp(*draw_call_context->m_wvp_matrix);
        std::get<1>(m_vertex_pipeline).set_world(*draw_call_context->m_world_matrix);

        std::get<1>(m_vertex_pipeline).flush(device_context);
        std::get<1>(m_vertex_pipeline).bind_as_vertex_constant_buffer(device_context);

		m_pixel_cbuffer.flush(device_context);
		m_pixel_cbuffer.bind_as_pixel_constant_buffer(device_context);

		device_context->VSSetShader( std::get<0>(m_vertex_pipeline),  nullptr, 0 );
		device_context->PSSetShader( m_pixel_shader, nullptr, 0 );
		device_context->IASetInputLayout( std::get<2>(m_vertex_pipeline) );

	}

	blinn_phong_shift_invairant_material::blinn_phong_shift_invairant_material
        ( 
                transform_position_normal_uv_vertex_pipeline                vertex_pipeline,
                blinn_phong_shift_invariant_pixel_shader					pixel_shader,
                blinn_phong_shift_invariant_pixel_shader_constant_buffer	pixel_cbuffer,
				math::vector_float4                                         kd,
				math::vector_float4                                         ks_gloss
		
		) :  
             m_vertex_pipeline(vertex_pipeline)
			 , m_pixel_shader(pixel_shader)
			 , m_pixel_cbuffer(pixel_cbuffer)
	{
		m_material_id = gx::create_material_id ( this );
	}

	blinn_phong_shift_invairant_material create_blinn_phong_shift_invairant_material( render_context* context, math::vector_float4 kd, math::vector_float4 ks_gloss )
	{
		    return blinn_phong_shift_invairant_material(
                                std::make_tuple< transform_position_normal_uv_vertex_shader, transform_position_normal_uv_vertex_shader_constant_buffer,  transform_position_normal_uv_input_layout > 
                                ( 
                                    context->m_transform_position_normal_uv_vertex_shader,
                                    context->m_transform_position_normal_uv_vertex_shader_cbuffer,
                                    context->m_transform_position_normal_uv_input_layout
                                ) ,
								context->m_blinn_phong_shift_invariant_pixel_shader,
								context->m_blinn_phong_pixel_cbuffer,
								kd,
                                ks_gloss
                                );
	}
}