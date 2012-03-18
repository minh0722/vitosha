#include "precompiled.h"

#include <gx/gx_screen_space_quad.h>

#include <sys/sys_base.h>
#include <gx/gx_render_context.h>

namespace gx
{
	void screen_space_quad_render::draw_quad(ID3D11DeviceContext* device_context)
	{
		math::matrix_float44 identity = math::identity_matrix();
		draw_quad( device_context, identity);
	}

	void screen_space_quad_render::draw_quad(ID3D11DeviceContext* device_context, math::matrix_float44 transform)
	{
		constant_buffer_update( device_context, m_cbuffer.get(), transform);
		device_context->VSSetConstantBuffers(0, 1, dx11::get_pointer(m_cbuffer) );
		device_context->Draw( 6, 0 );
	}

	void screen_space_quad_render::begin(ID3D11DeviceContext* device_context)
	{
		device_context->IASetInputLayout(m_input_layout.get());
		device_context->VSSetShader(m_vertex_shader.get(), 0, 0) ;

		uint32_t stride = 12;
        uint32_t offset = 0;
		device_context->IASetVertexBuffers( 0, 1, dx11::get_pointer(m_vertex_buffer), &stride, &offset );
	}

	void screen_space_quad_render::end(ID3D11DeviceContext* device_context)
	{
		UNUSED_PARAMETER(device_context);
	}

	screen_space_quad_render	create_screen_space_quad_render(render_context* context)
	{
		return context->create_screen_space_quad_render();		
	}

	void draw_screen_space_quad(ID3D11DeviceContext* device_context, render_context* render_context)
	{
		screen_space_quad_render r = create_screen_space_quad_render(render_context);
		draw_screen_space_quad(&r, device_context);
	}

	void draw_screen_space_quad(ID3D11DeviceContext* device_context, render_context* render_context, math::matrix_float44 transform)
	{
		screen_space_quad_render r = create_screen_space_quad_render(render_context);
		draw_screen_space_quad(&r, device_context, transform);
	}

}