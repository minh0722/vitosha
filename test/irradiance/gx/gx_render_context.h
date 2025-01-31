#ifndef __GX_RENDER_CONTEXT_H__
#define __GX_RENDER_CONTEXT_H__

#include <memory>
#include <vector>
#include <cstdint>
#include <boost/noncopyable.hpp>

#include <d3d11/d3d11_pointers.h>
#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_system.h>

#include <gx/gx_render_resource.h>

#include <gx/gx_screen_space_quad.h>
#include <gx/gx_shader_database.h>

#include <gx/gx_view_port.h>

namespace gx
{
    struct gbuffer_render_data
    {
        explicit gbuffer_render_data ( ID3D11Device* device, d3d11::ibuffer_ptr per_pass_buffer ) : 
          m_normal( create_normal_resource(device, 320, 240 ) )
        , m_diffuse( create_diffuse_resource( device, 320, 240 ) )
        , m_specular( create_specular_resource( device, 320, 240 ) )
        , m_opaque( create_gbuffer_opaque_blend_state(device) )
        , m_per_pass_buffer ( per_pass_buffer )
        {

        }


        target_render_resource  m_normal;
        target_render_resource  m_diffuse;
        target_render_resource  m_specular;

        d3d11::iblendstate_ptr  m_opaque;

        d3d11::ibuffer_ptr      m_per_pass_buffer;
    };

    struct light_buffer_render_data
    {
        explicit light_buffer_render_data ( ID3D11Device* device) : 
          m_additive_blend ( create_additive_blend_state(device) ) 
        , m_light_buffer( create_light_buffer_resource(device, 320, 240) )
        {

        }

        d3d11::iblendstate_ptr              m_additive_blend;
        target_render_resource              m_light_buffer;

        d3d11::idepthstencilview_ptr        m_read_depth_dsv;
        d3d11::ishaderresourceview_ptr      m_depth_srv;
    };

    struct depth_render_data
    {
        explicit depth_render_data  ( ID3D11Device* device ) :
            m_depth_vertex_shader(device)
        ,   m_depth_constant_buffer(device)
        ,   m_input_layout(device, &m_depth_vertex_shader )
        {

        }

        transform_position_vertex_shader                    m_depth_vertex_shader;
        transform_position_vertex_shader_constant_buffer    m_depth_constant_buffer;
        transform_position_input_layout	                    m_input_layout;
    };

    struct screen_space_render_data
    {
        explicit screen_space_render_data (ID3D11Device* device) :
            m_screen_space_vertex_shader(device)
            , m_screen_space_constant_buffer(device)
        {

        }

        d3d11::iinputlayout_ptr                                 m_screen_space_input_layout;
        transform_position_uv_vertex_shader                     m_screen_space_vertex_shader;
        transform_position_uv_vertex_shader_constant_buffer     m_screen_space_constant_buffer;
        d3d11::ibuffer_ptr                                      m_screen_space_vertex_buffer;
    };

    struct debug_render_data
    {
        debug_render_data ( ID3D11Device* device ) : 
            m_depth_buffer_copy( create_target_render_resource( device, 320, 240, DXGI_FORMAT_R32_FLOAT ) ) 
        {

        }

        inline void reset()
        {

        }

        //copy of the depth buffer
        target_render_resource                  m_depth_buffer_copy;
        d3d11::idepthstencilview_ptr            m_read_depth_dsv;
        d3d11::ishaderresourceview_ptr          m_depth_srv;
    };

    struct per_view_data
    {
        math::float4x4      m_view_matrix;
        math::float4x4      m_projection_matrix;
        math::float4x4      m_inverse_view_matrix;
        math::float4x4      m_inverse_projection_matrix;

        view_port           m_view_port;
        float               m_zn;
        float               m_zf;
    };

    class thread_render_context;

    //performs management of surfaces, gpu memory buffers, shaders, textures on a higher level than a 3d device
    //has knowledge about a fixed rendering method and material system
    class render_context : public boost::noncopyable
    {
        public:
        typedef  std::vector< std::unique_ptr<thread_render_context> > thread_render_context_container;

        render_context(d3d11::system_context sys_context, std::shared_ptr<shader_database> shader_database, uint32_t render_context_count, view_port view_port);
        ~render_context();

        void begin_frame();
        void end_frame();

        void create_swap_chain_buffers();
        void release_swap_chain_buffers();

        void reset_state( ID3D11DeviceContext* device_context );
        void clear_buffers( ID3D11DeviceContext* device_context);

        void select_depth_pass(ID3D11DeviceContext* device_context, const per_view_data& view);
        void select_gbuffer(ID3D11DeviceContext* device_context, const per_view_data& view);
        void select_back_buffer_target(ID3D11DeviceContext* device_context, const per_view_data& view);
        void select_light_buffer(ID3D11DeviceContext* device_context, const per_view_data& view);
        void select_debug_target(ID3D11DeviceContext* device_context, const per_view_data& view);

        void end_depth_pass(ID3D11DeviceContext* device_context, const per_view_data& view);
        void end_light_buffer(ID3D11DeviceContext* device_context, const per_view_data& view);
        void end_gbuffer(ID3D11DeviceContext* device_context, const per_view_data& view);
        void compose_light_buffer(ID3D11DeviceContext* device_context, const per_view_data& view);


        screen_space_quad_render	create_screen_space_quad_render();

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

        inline std::unique_ptr<thread_render_context>& front()
        {
            return m_render_contexts.front();
        }

        inline const std::unique_ptr<thread_render_context>& front() const
        {
            return m_render_contexts.front();
        }

        inline void set_view_port ( view_port value )
        {
            m_view_port = value;
        }

        inline ID3D11DeviceContext* get_immediate_context()
        {
            return m_system_context.m_immediate_context.get();
        }

        inline const ID3D11DeviceContext* get_immediate_context() const
        {
            return m_system_context.m_immediate_context.get();
        }

        inline ID3D11Device* get_device()
        {
            return m_system_context.m_device.get();
        }

        inline const ID3D11Device* get_device() const
        {
            return m_system_context.m_device.get();
        }

        inline const shader_database* get_shader_database() const
        {
            return m_shader_database.get();
        }

        inline shader_database* get_shader_database()
        {
            return m_shader_database.get();
        }

        private:

        render_context();

        void create_back_buffer_render_target();
        void create_depth_buffer();

        void create_diffuse_buffer();
        void create_specular_buffer();
        void create_normal_depth_buffer();
        void create_light_buffer();
        void create_debug_buffer();

        void create_gbuffer_state();
        void create_depth_state();
        void create_light_buffer_state();
       

        void create_depth_buffer_layout();
        void create_screen_space_input_layout();

        void create_screen_space_vertex_buffer();

        void create_default_render_data();
        void create_light_buffer_render_data();
        void create_debug_render_data();

        void select_view_port(ID3D11DeviceContext* device_context);

        d3d11::system_context                   m_system_context;
        thread_render_context_container         m_render_contexts;

        public:

        d3d11::id3d11rendertargetview_ptr       m_back_buffer;
        depth_resource                          m_depth_buffer;
        d3d11::idepthstencilstate_ptr           m_depth_enable_state;
        d3d11::idepthstencilstate_ptr           m_depth_disable_state;
        d3d11::ibuffer_ptr                      m_per_pass_vertex_buffer;
        d3d11::ibuffer_ptr                      m_per_pass_pixel_buffer;


        d3d11::iblendstate_ptr                  m_opaque_state;
        d3d11::iblendstate_ptr                  m_additive_state;

        d3d11::isamplerstate_ptr                m_default_sampler_state;
        d3d11::isamplerstate_ptr                m_point_sampler_state;

        d3d11::irasterizerstate_ptr             m_cull_back_raster_state;

        gbuffer_render_data                     m_gbuffer_render_data;
        depth_render_data                       m_depth_render_data;
        light_buffer_render_data                m_light_buffer_render_data;
        debug_render_data                       m_debug_render_data;

        view_port                               m_view_port;
        screen_space_render_data                m_screen_space_render_data;
        std::shared_ptr<shader_database>        m_shader_database;
    };
}



#endif

