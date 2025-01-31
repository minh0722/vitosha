#ifndef __GX_GBUFFER_DT_NG_SC_GC_PIXEL_SHADER_H__
#define __GX_GBUFFER_DT_NG_SC_GC_PIXEL_SHADER_H__

#include <cstdint>
#include <memory>

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_pointers.h>

#include <math/math_matrix.h>

#include <gx/gx_constant_buffer_helper.h>

namespace gx
{
	class __declspec(align(16)) gbuffer_dt_ng_sc_gc_pixel_shader_constant_buffer 
	{
		public:

		explicit gbuffer_dt_ng_sc_gc_pixel_shader_constant_buffer ( ID3D11Device* device );

		math::float4 get_ks_gloss() const
		{
			return m_ks_gloss;
		}

		void set_ks_gloss(math::float4 value)
		{
			m_ks_gloss = value;
		}


		bool get_has_alpha_test() const
		{
			return m_has_alpha_test;
		}

		void set_has_alpha_test(bool value)
		{
			m_has_alpha_test = value;
		}

		void flush ( ID3D11DeviceContext* context )
		{
			gx::constant_buffer_update(context, m_buffer.get(), (void*) &m_ks_gloss, size() );
		}

		void bind_as_pixel_constant_buffer(ID3D11DeviceContext* context, uint32_t slot)
		{
			context->PSSetConstantBuffers(slot, 1, dx::get_pointer(m_buffer));
		}

		void bind_as_pixel_constant_buffer(ID3D11DeviceContext* context)
		{
			bind_as_pixel_constant_buffer(context, 0);
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
			return sizeof(m_ks_gloss) + sizeof(m_has_alpha_test) + sizeof(m_padding);
		}

		public:
		math::float4		m_ks_gloss;			//specular color + gloss
		bool				m_has_alpha_test;
		uint8_t				m_padding[15];

		d3d11::ibuffer_ptr	m_buffer;
	};


	class gbuffer_dt_ng_sc_gc_pixel_shader final
    {
		public:
		explicit gbuffer_dt_ng_sc_gc_pixel_shader ( ID3D11Device* device );

		operator const ID3D11PixelShader*() const
		{
			return m_shader.get();
		}

		d3d11::ipixelshader_ptr		m_shader;
		const void*					m_code;
		uint32_t					m_code_size;
    };
}

#endif