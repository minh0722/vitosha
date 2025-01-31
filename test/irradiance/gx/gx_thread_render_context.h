#ifndef __GX_THREAD_RENDER_CONTEXT_H__
#define __GX_THREAD_RENDER_CONTEXT_H__

#include <memory>
#include <vector>
#include <cstdint>
#include <boost/noncopyable.hpp>

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_system.h>

namespace gx
{
	class render_context;

    class thread_render_context : public boost::noncopyable
    {
        public:

        explicit thread_render_context( render_context* render_context, d3d11::idevicecontext_ptr d3d11_device_context) : 
				m_render_context(render_context)
				, m_d3d11_device_context(d3d11_device_context)
        {

        }

        void begin_frame()
        {
            m_d3d11_device_context->ClearState();
        }

        void end_frame()
        {
            dx::throw_if_failed<d3d11::d3d11_exception>( m_d3d11_device_context->FinishCommandList(FALSE, dx::get_pointer(m_command_list)));
        }

        void submit( d3d11::idevicecontext_ptr immediate_context)
        {
            if (m_command_list)
            {
                immediate_context->ExecuteCommandList(m_command_list.get(), FALSE);
                m_command_list.reset();
            }
        }

        void reset()
        {
            m_command_list.reset();
            m_command_list_submit.reset();
        }

        void swap()
        {
            m_command_list_submit.swap( m_command_list );
        }

		inline d3d11::idevicecontext_ptr	get_device_context()
		{
			return m_d3d11_device_context;
		}

		inline const d3d11::idevicecontext_ptr	get_device_context() const
		{
			return m_d3d11_device_context;
		}

        private:
		render_context*				m_render_context;
        d3d11::idevicecontext_ptr	m_d3d11_device_context;

        d3d11::icommandlist_ptr		m_command_list;
        d3d11::icommandlist_ptr		m_command_list_submit;
    };
}



#endif

