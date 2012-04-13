#ifndef __GXU_ENTITY_FACTORY_H__
#define __GXU_ENTITY_FACTORY_H__

#include <cstdint>
#include <memory>

#include <math/math_vector.h>

#include <gx/gx_render_context.h>

#include <gxu/gxu_static_mesh_entity.h>


namespace gx
{
	class entity;
    class indexed_draw_call;
	class render_context;
}

namespace gxu
{
	//fills 2 buffers (positions) , (normals + uvs)
    gx::indexed_draw_call create_lat_lon_sphere( ID3D11Device* device, float radius, uint32_t subdivision_count );

	//fills 1 buffer (positions)
	std::tuple< dx11::id3d11buffer_ptr, dx11::id3d11buffer_ptr > create_lat_lon_sphere_2( ID3D11Device* device, float radius, uint32_t subdivision_count );

	template <typename factory> inline std::shared_ptr<gx::entity> create_lat_lon_sphere_entity( gx::render_context* context, gx::indexed_draw_call draw_call, math::vector_float4 color )
	{
		return std::make_shared< gxu::static_mesh_entity<factory::type> > ( draw_call , factory::create(context, color) );
	}

	template <typename factory> inline std::shared_ptr<gx::entity> create_lat_lon_sphere_entity( gx::render_context* context, float radius, uint32_t subdivision_count, math::vector_float4 color )
    {
		return create_lat_lon_sphere_entity<factory> ( context, create_lat_lon_sphere(context->get_device(), radius, subdivision_count) , color );
    }
}

#endif