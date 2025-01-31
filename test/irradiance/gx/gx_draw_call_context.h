#ifndef __GX_DRAW_CALL_CONTEXT_H__
#define __GX_DRAW_CALL_CONTEXT_H__

#include <cstdint>
#include <math/math_matrix.h>

struct ID3D11DeviceContext;

namespace gx
{
    struct draw_call_context
    {
        ID3D11DeviceContext*	  m_device_context;

        math::float4x4*           m_view_matrix;
        math::float4x4*           m_projection_matrix;
        math::float4x4*           m_inverse_view_matrix;
        math::float4x4*           m_inverse_projection_matrix;
        math::float4x4*           m_wvp_matrix;
        math::float4x4*           m_world_matrix;

    };
}

#endif