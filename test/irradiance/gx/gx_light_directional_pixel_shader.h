#ifndef __GX_LIGHT_DIRECTIONAL_PIXEL_SHADER_H__
#define __GX_LIGHT_DIRECTIONAL_PIXEL_SHADER_H__

#include <cstdint>
#include <iterator>
#include <memory>

#include <dx11/dx11_error.h>
#include <dx11/dx11_pointers.h>

#include <math/math_matrix.h>

#include <gx/gx_constant_buffer_helper.h>

namespace gx
{
    class light_directional_pixel_shader_constant_buffer 
	{
		public:

		explicit light_directional_pixel_shader_constant_buffer ( ID3D11Device* device );

		void set_inverse_projection(math::matrix_float44 value)
		{
			m_inverse_projection = value;
		}

        void set_view(math::matrix_float44 value)
		{
			m_view = value;
		}

        void set_light_count(uint32_t value)
        {
            m_light_count = value;
        }

        void set_light_position_ws( const math::vector_float4* begin, const math::vector_float4* end )
        {
            std::copy(begin, end, stdext::make_checked_array_iterator(&m_light_position_ws[0], 8, 0 ) );
        }

        void set_light_color( const math::vector_float4* begin, const math::vector_float4* end)
        {
            std::copy(begin, end, stdext::make_checked_array_iterator(&m_light_color[0], 8, 0) );
        }

		void update ( ID3D11DeviceContext* context, math::matrix_float44* value )
		{
			constant_buffer_update( context, m_buffer.get(), value);
		}

		void flush ( ID3D11DeviceContext* context )
		{
			update(context, &m_inverse_projection);
		}

		void bind_as_pixel_constant_buffer(ID3D11DeviceContext* context)
		{
			context->PSSetConstantBuffers(0, 1, dx11::get_pointer(m_buffer));
		}

        operator ID3D11Buffer*()
		{
			return m_buffer.get();
		}

		operator const ID3D11Buffer*() const
		{
			return m_buffer.get();
		}

        size_t size() const
        {
            return sizeof(m_inverse_projection) + sizeof(m_view);
        }

		private:

		dx11::id3d11buffer_ptr	m_buffer;
		math::matrix_float44	m_inverse_projection;
        math::matrix_float44	m_view;

        math::vector_float4     m_light_position_ws[8];
        math::vector_float4     m_light_color[8];
        uint32_t                m_light_count;
	};

	class light_directional_pixel_shader
    {
		public:

		explicit light_directional_pixel_shader ( ID3D11Device* device );

		operator dx11::id3d11pixelshader_ptr()
		{
			return m_shader;
		}

		operator ID3D11PixelShader*()
		{
			return m_shader.get();
		}

		operator const ID3D11PixelShader*() const
		{
			return m_shader.get();
		}

		dx11::id3d11pixelshader_ptr							 m_shader;
		const void*											 m_code;
		uint32_t										 m_code_size;
    };

    typedef std::tuple < light_directional_pixel_shader, light_directional_pixel_shader_constant_buffer > light_directional_pixel_pipeline;

    inline light_directional_pixel_pipeline create_light_directional_pixel_pipeline( ID3D11Device* device )
    {
        return std::make_tuple<  light_directional_pixel_shader, light_directional_pixel_shader_constant_buffer >
            (
                light_directional_pixel_shader(device),
                light_directional_pixel_shader_constant_buffer(device)
            ); 
    }
}

#endif