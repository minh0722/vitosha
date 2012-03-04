#ifndef __GX_RENDER_CONTEXT_H__
#define __GX_RENDER_CONTEXT_H__

#include <memory>
#include <vector>
#include <cstdint>
#include <boost/noncopyable.hpp>

#include <dx11/dx11_pointers.h>
#include <dx11/dx11_error.h>
#include <dx11/dx11_system.h>

namespace gx
{
	struct gbuffer_render_set
	{
		inline void reset()
		{
			m_normal_depth_target.reset();
			m_diffuse_target.reset();
			m_specular_target.reset();

			m_depth_stencil_target.reset();
			m_depth_stencil.reset();
			
			m_normal_depth.reset();
			m_diffuse.reset();
			m_specular.reset();
		}

		dx11::id3d11rendertargetview_ptr		m_normal_depth_target;
		dx11::id3d11rendertargetview_ptr		m_diffuse_target;
		dx11::id3d11rendertargetview_ptr		m_specular_target;

		dx11::id3d11texture2d_ptr				m_depth_stencil;
		dx11::id3d11depthstencilview_ptr		m_depth_stencil_target;

		dx11::id3d11texture2d_ptr				m_normal_depth;
		dx11::id3d11texture2d_ptr				m_diffuse;
		dx11::id3d11texture2d_ptr				m_specular;
	};

	struct depth_render_set
	{
		void reset()
		{
			m_depth_stencil_target.reset();
			m_depth_stencil.reset();
		}

		dx11::id3d11texture2d_ptr				m_depth_stencil;
		dx11::id3d11depthstencilview_ptr		m_depth_stencil_target;
	};

    class thread_render_context;

    class render_context : public boost::noncopyable
    {
        public:
		typedef  std::vector< std::unique_ptr<thread_render_context> > thread_render_context_container;

		render_context(dx11::system_context sys_context, std::uint32_t render_context_count);
		~render_context();

        void begin_frame();
		void end_frame();

		void create_swap_chain_buffers();
		void release_swap_chain_buffers();

		void select_gbuffer(ID3D11DeviceContext* device_context);
		void select_main_target(ID3D11DeviceContext* device_context);


		inline thread_render_context_container::iterator begin()
		{
			return m_render_contexts.begin();
		}

		inline thread_render_context_container::const_iterator begin() const
		{
			return m_render_contexts.begin();
		}


		inline thread_render_context_container::iterator end()
		{
			return m_render_contexts.end();
		}

		inline thread_render_context_container::const_iterator end() const
		{
			return m_render_contexts.end();
		}

		private:

		render_context();

		void create_back_buffer_render_target();
		void create_depth_buffer();

		void create_diffuse_buffer();
		void create_specular_buffer();
		void create_normal_depth_buffer();

		void clear_buffers( ID3D11DeviceContext* device_context);


        dx11::system_context									m_system_context;
        thread_render_context_container							m_render_contexts;

		dx11::id3d11rendertargetview_ptr						m_back_buffer_render_target;

		dx11::id3d11texture2d_ptr								m_depth_stencil;
		dx11::id3d11depthstencilview_ptr						m_depth_stencil_target;

		gbuffer_render_set										m_gbuffer_render_set;
		depth_render_set										m_depth_render_set;
    };
}



#endif

