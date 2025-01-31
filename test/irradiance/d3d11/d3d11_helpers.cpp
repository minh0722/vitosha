#include "precompiled.h"

#include <d3d11/d3d11_helpers.h>

#include <d3d11/d3d11_error.h>
#include <d3d11/d3d11_pointers.h>

namespace d3d11
{
	namespace
	{
		static d3d11::ibuffer_ptr create_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size, D3D11_USAGE d3d11_bind_flags, uint32_t  cpu_access_flags )
		{
			D3D11_BUFFER_DESC desc = {};
			d3d11::ibuffer_ptr result;
			desc.ByteWidth = static_cast<UINT> (size);
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.CPUAccessFlags = cpu_access_flags;
			desc.Usage = d3d11_bind_flags;
			D3D11_SUBRESOURCE_DATA initial_data_dx = { initial_data, 0, 0};
			dx::throw_if_failed<d3d11::create_buffer_exception> (device->CreateBuffer(&desc, &initial_data_dx, dx::get_pointer(result)));
			return result;
		}

		static d3d11::ibuffer_ptr create_index_buffer(ID3D11Device* device, const void* initial_data, size_t size, D3D11_USAGE d3d11_bind_flags, uint32_t cpu_access_flags  )
		{
			D3D11_BUFFER_DESC desc = {};
			d3d11::ibuffer_ptr result;
			desc.ByteWidth = static_cast<UINT> (size);
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER ;
			desc.CPUAccessFlags = cpu_access_flags;
			desc.Usage = d3d11_bind_flags;
			D3D11_SUBRESOURCE_DATA initial_data_dx = { initial_data, 0, 0};
			dx::throw_if_failed<d3d11::create_buffer_exception> (device->CreateBuffer(&desc, &initial_data_dx, dx::get_pointer(result)));
			return result;
		}
	}

	d3d11::ibuffer_ptr create_constant_buffer(ID3D11Device* device, size_t size)
	{
		D3D11_BUFFER_DESC desc = {};
		d3d11::ibuffer_ptr result;

		desc.ByteWidth = static_cast<UINT> (size);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		dx::throw_if_failed<d3d11::create_buffer_exception> (device->CreateBuffer(&desc, nullptr, dx::get_pointer(result) ));
		return result;
	}

	d3d11::ibuffer_ptr create_default_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size )
	{
		return create_vertex_buffer(device, initial_data, size, D3D11_USAGE_DEFAULT, 0);
	}

	d3d11::ibuffer_ptr create_default_index_buffer(ID3D11Device* device, const void* initial_data, size_t size )
	{
		return create_index_buffer(device, initial_data, size, D3D11_USAGE_DEFAULT, 0);
	}

	d3d11::ibuffer_ptr create_dynamic_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size )
	{
		return create_vertex_buffer(device, initial_data, size, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE );
	}

	d3d11::ibuffer_ptr create_dynamic_index_buffer(ID3D11Device* device, const void* initial_data, size_t size )
	{
		return create_index_buffer(device, initial_data, size, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE );
	}

	d3d11::ibuffer_ptr create_immutable_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size )
	{
		return create_vertex_buffer(device, initial_data, size, D3D11_USAGE_IMMUTABLE, 0 );
	}

	d3d11::ibuffer_ptr create_immutable_index_buffer(ID3D11Device* device, const void* initial_data, size_t size )
	{
		return create_index_buffer(device, initial_data, size, D3D11_USAGE_IMMUTABLE, 0);
	}

    d3d11::ibuffer_ptr create_stream_out_vertex_buffer(ID3D11Device* device, const void* initial_data, size_t size )
	{
        D3D11_BUFFER_DESC desc = {};
		d3d11::ibuffer_ptr result;
		desc.ByteWidth = static_cast<UINT> (size);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT ;
		desc.CPUAccessFlags = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		D3D11_SUBRESOURCE_DATA initial_data_dx = { initial_data, 0, 0};
		dx::throw_if_failed<d3d11::create_buffer_exception> (device->CreateBuffer(&desc, &initial_data_dx, dx::get_pointer(result)));
		return result;
	}

    
}
