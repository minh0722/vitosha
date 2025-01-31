#ifndef __GX_POINT_LIGHT_SPHERE_DEBUG_VERTEX_SHADER_H__
#define __GX_POINT_LIGHT_SPHERE_DEBUG_VERTEX_SHADER_H__

#include <cstdint>

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_pointers.h>

#include <math/math_matrix.h>


#include <gx/gx_constant_buffer_helper.h>

class point_light_sphere_debug_vertex_shader_constant_buffer 
{
	public:

	explicit point_light_sphere_debug_vertex_shader_constant_buffer ( ID3D11Device* device );

	void set_vp(math::float4x4 value)
	{
		m_vp = value;
	}

	void flush ( ID3D11DeviceContext* context )
	{
		gx::constant_buffer_update(context, m_buffer.get(), (void*) &m_vp, size() );
	}

	void bind_as_vertex_constant_buffer(ID3D11DeviceContext* context)
	{
		context->VSSetConstantBuffers(1, 1, dx::get_pointer(m_buffer));
	}

	operator ID3D11Buffer*()
	{
		return m_buffer.get();
	}

	operator const ID3D11Buffer*() const
	{
		return m_buffer.get();
	}

	inline size_t size() const
	{
			return sizeof(m_vp);
	}

	public:

	d3d11::ibuffer_ptr	m_buffer;
	math::float4x4		m_vp;		//view projection
};


class point_light_sphere_debug_vertex_shader
{
	public:

	explicit point_light_sphere_debug_vertex_shader ( ID3D11Device* device );

	operator d3d11::ivertexshader_ptr()
	{
		return m_shader;
	}

	operator ID3D11VertexShader*()
	{
		return m_shader.get();
	}

	operator const ID3D11VertexShader*() const
	{
		return m_shader.get();
	}

	d3d11::ivertexshader_ptr	m_shader;
	const void*						m_code;
	uint32_t						m_code_size;
};

#endif