#include "precompiled.h"

#include <win32/window.h>

#include <memory>

#include <math/math_graphics.h>

#include <fnd/fnd_universe.h>

#include <gx/gx_create_draw_calls_pipeline_node.h>
#include <gx/gx_execute_draw_calls_pipeline_node.h>
#include <gx/gx_final_pipeline_node.h>
#include <gx/gx_pinhole_camera.h>
#include <gx/gx_pipeline.h>
#include <gx/gx_render_context.h>
#include <gx/gx_scene.h>
#include <gx/gx_scene_pipeline_node.h>

#include <gx/gx_view.h>
#include <gx/gx_view_pipeline_node.h>

#include <win32/application.h>

namespace wnd
{
	window::window(application& application, dx11::idxgiswapchain_ptr swap_chain, gx::render_context* render_context ) : 
			m_application(application)
			, m_swap_chain(swap_chain)
			, m_render_context(render_context)
			, m_occluded_by_another_window(false)
	{

		math::vector_float4  view_position = math::set( 0.0f, 1.0f,  -5.5f, 0.0f ); //meters
		math::vector_float4  view_look_at  = math::set( 0.0f, 0.0f,  0.0f, 0.0f ); //look along the z
		math::vector_float4  view_up = math::set( 0.0f, 1.0f, 0.0f, 0.0f );  //up vector

		m_main_camera.set_position(view_position);
		m_main_camera.set_look_at_direction(view_look_at);
		m_main_camera.set_up(view_up);

		m_main_camera.set_aspect_ratio(16.0f / 9.0f);
		m_main_camera.set_fov(3.1415f / 4.0f );
		m_main_camera.set_near(0.005f);
		m_main_camera.set_far(400.f); //meters
	
		
	}

	window::~window()
	{

	}

	void window::destroy()
	{
		m_application.delete_window(this);
		delete this;
	}

	void window::render()
	{
		if (m_occluded_by_another_window)
		{
			HRESULT hr = m_swap_chain->Present(0, DXGI_PRESENT_TEST );

			if ( hr == S_OK)
			{
				m_occluded_by_another_window = false;
			}

			if (hr != DXGI_STATUS_OCCLUDED)
			{
				dx11::throw_if_failed<dx11::d3d11_exception>(hr);		
			}
		}
		else
		{
			render_frame();

			HRESULT hr = m_swap_chain->Present(0,0);

			if (hr == DXGI_STATUS_OCCLUDED)
			{
				m_occluded_by_another_window = true;
			}
			else
			{
				dx11::throw_if_failed<dx11::d3d11_exception>(hr);		
			}
		}
	}

	void window::render_frame()
	{
		gx::pipeline    pipeline;
		
		math::matrix_float44 view_matrix = gx::create_view_matrix(&m_main_camera);
		math::matrix_float44 perspective_matrix = gx::create_perspective_matrix(&m_main_camera);

		gx::view			 view(view_matrix, perspective_matrix );

		pipeline.add_node( std::make_shared< gx::scene_pipeline_node>(m_scene.get()) );

		pipeline.add_node( std::make_shared< gx::view_pipeline_node>(&view) );
		pipeline.add_node( std::make_shared< gx::create_draw_calls_pipeline_node>() );
		pipeline.add_node( std::make_shared< gx::execute_draw_calls_pipeline_node>(m_render_context) );

		pipeline.add_node( std::make_shared< gx::final_pipeline_node>() );
		pipeline.process();
	}

	void window::resize_window(std::uint32_t width, std::uint32_t height)
	{
		using namespace dx11;
		DXGI_SWAP_CHAIN_DESC desc = {};

		//disable dxgi errors
		width = std::max(width, (uint32_t)(8));
		height = std::max(height, (uint32_t)(8));

		//release buffers that are window size
		m_render_context->release_swap_chain_buffers();

		throw_if_failed<d3d11_exception>(m_swap_chain->GetDesc(&desc));
		throw_if_failed<d3d11_exception>(m_swap_chain->ResizeBuffers(desc.BufferCount, width, height,  desc.BufferDesc.Format, desc.Flags));

		//create a new viewport
		gx::view_port view_port ( 0, 0, width, height );
		m_render_context->set_view_port(view_port);

		m_main_camera.set_aspect_ratio ( static_cast<float>(width) / static_cast<float>(height) );

		m_render_context->create_swap_chain_buffers();
	}
}