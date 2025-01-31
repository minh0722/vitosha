#include "precompiled.h"

#include <algorithm>
#include <future>
#include <fstream>
#include <iostream>
#include <thread>

#include <d3d11/d3d11_system.h>
#include <math/math_graphics.h>

#include <fnd/fnd_universe.h>
#include <fnd/fnd_updater.h>

#include <gx/gx_color.h>
#include <gx/gx_blinn_phong_shift_invariant_material.h>
#include <gx/gx_entity.h>
#include <gx/gx_entity_world.h>
#include <gx/gx_lambert_shift_invariant_material.h>
#include <gx/gx_lighting.h>
#include <gx/gx_scene.h>

#include <gxu/gxu_entity_factory.h>

#include "win32/floor.h"
#include "win32/point_lights.h"
#include "win32/directional_lights.h"
#include "win32/room.h"


std::shared_ptr<gx::scene> universe_bootstrap( gx::render_context* render_context, d3d11::system_context context, std::shared_ptr<fnd::universe> universe )
{

    auto scene = std::make_shared<gx::scene>();
    auto entities = std::make_shared<gx::entity_world> ();
    auto dynamic_entities = std::make_shared<fnd::typed_world> ();

    //add two worlds
    //1. with entities, created or loaded
    //2. dynamic entities which change depending on time
    //3. add a spatial structure for representing these entities
    universe->add_world(entities);
    universe->add_world(dynamic_entities);
    universe->add_world(scene);

    //room
    std::ifstream f("giroom.am", std::ios_base::in | std::ios_base::binary);

    std::future< std::shared_ptr<room_entity> > future_room = std::async( std::launch::async, [&]()
    {
        return create_room_entity(context.m_device.get(), render_context->get_shader_database(), f);
    });

    //auto room_entity = create_room_entity(context.m_device.get(), render_context->get_shader_database(), f);


    //floor
    auto floor_entity = create_floor_entity( context.m_device.get(), render_context->get_shader_database(), 20, 20, 30 );

    //directional light
    std::vector<directional_light> directional_lights;
    directional_lights.reserve(8);

    math::float4 light_position = math::set( 5.0f, 1.0f, 5.0f, 1.0f );

    directional_lights.push_back(  directional_light( gx::color::white(), math::sub(light_position, math::set(0.0f, 0.0f, 0.0f, 1.0f) ) ));

    //should be quad
    auto directional_entity = create_directional_lights_entity( context.m_device.get(), render_context->m_screen_space_render_data.m_screen_space_vertex_buffer, &directional_lights[0], &directional_lights[0] + directional_lights.size()  );

    //create point light entity and insert it into two worlds (for update and render)
    auto point_lights = create_point_lights_entity( context.m_device.get() );
    auto light_updater = create_light_entity_updater();
    dynamic_entities->add_type( 0, light_updater );


    point_light light1 ( math::set(0.3f, 0.2f, 0.0f, 0.0f), light_position, 1.0f, 0.5f);

    point_lights->add_light(light1);

    auto it = scene->begin();
    auto it_end = scene->end();

    auto pi_div_two		= 1.570796327f;

    //auto m_1 = math::rotation_x(-pi_div_two);
    //auto m_2 = math::translation(-1.0f, 0.0f, 0.0f);
    auto m_4 = math::translation(0.0f, 0.0f, 0.0f);
    
    auto root = scene->get_root();

    //auto entity_1 = gxu::create_lat_lon_sphere_entity<gx::lambert_shift_invairant_material_factory>( render_context, 1.0f, 20, gx::color::blue() ); 

    auto sphere_1 = gxu::create_lat_lon_sphere_entity<gx::blinn_phong_shift_invairant_material_factory>( render_context->get_device(), render_context->get_shader_database(), 1.0f, 20 , gx::color::green() , math::set(0.05f, 0.05f, 0.05f, gx::encode_specular_power(2 * 25.0f) )  ); 
    auto sphere_2 = gxu::create_lat_lon_sphere_entity<gx::blinn_phong_shift_invairant_material_factory>( render_context->get_device(), render_context->get_shader_database(), 1.0f, 20 , gx::color::green() , math::set(0.05f, 0.05f, 0.05f, gx::encode_specular_power(2 * 50.0f) )  ); 
    auto sphere_3 = gxu::create_lat_lon_sphere_entity<gx::blinn_phong_shift_invairant_material_factory>( render_context->get_device(), render_context->get_shader_database(), 1.0f, 20 , gx::color::green() , math::set(0.05f, 0.05f, 0.05f, gx::encode_specular_power(2 * 75.0f) )  ); 
    auto sphere_4 = gxu::create_lat_lon_sphere_entity<gx::blinn_phong_shift_invairant_material_factory>( render_context->get_device(), render_context->get_shader_database(), 1.0f, 20 , gx::color::green() , math::set(0.05f, 0.05f, 0.05f, gx::encode_specular_power(2 * 100.0f) )  ); 

    auto node_1 = std::make_shared<gx::scene::node> ( m_4, sphere_1.get() );
    auto node_2 = std::make_shared<gx::scene::node> ( m_4, point_lights.get() );
    auto node_3 = std::make_shared<gx::scene::node> ( m_4, directional_entity.get() );
    auto node_4 = std::make_shared<gx::scene::node> ( math::translation(0.0f, -2.0f, 0.0f) , floor_entity.get() );

    auto room_entity  = future_room.get();
    auto node_5 = std::make_shared<gx::scene::node> ( math::translation(0.0f, -2.0f, 0.0f), room_entity.get() );

    auto node_6 = std::make_shared<gx::scene::node> ( math::translation(2.0f, 0.0f, 0.0f), sphere_2.get() );
    auto node_7 = std::make_shared<gx::scene::node> ( math::translation(4.0f, 0.0f, 0.0f), sphere_3.get() );
    auto node_8 = std::make_shared<gx::scene::node> ( math::translation(6.0f, 0.0f, 0.0f), sphere_4.get() );


    //1. add to graphic world
    gx::add_node(root, node_1);
    gx::add_node(root, node_2);
    gx::add_node(root, node_3);
    gx::add_node(root, node_4);
    gx::add_node(root, node_5);
    gx::add_node(root, node_6);
    gx::add_node(root, node_7);
    gx::add_node(root, node_8);
    
    //2. add to entities world
    entities->add_entity( point_lights);
    entities->add_entity( directional_entity );
    entities->add_entity( floor_entity );
    entities->add_entity( room_entity );

    entities->add_entity( sphere_1);
    entities->add_entity( sphere_2);
    entities->add_entity( sphere_3);
    entities->add_entity( sphere_4);

    scene->rebuild();

    //3. add lights for updating
    light_updater->add_item(point_lights);
    
    return scene;
}

