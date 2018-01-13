// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "config.hpp"
#include "resolver.hpp"


namespace ORGame
{

    void TrackElements::init_bars(std::vector<BarEvent> &bars)
    {

        std::vector<BarEvent> &bars = m_tempoTrack->get_bars();
        std::cout << "Bar Count: " << bars.size() << " Song Length: " << m_song.length() << std::endl;

        // reuse the same container when creating bars as add_obj wont modify the original.
        ORCore::RenderObject obj;
        obj.set_camera(m_cameraDynamic);
        obj.set_texture(m_texture);
        obj.set_program(m_program);

        auto& renderer = ORCore::Resolver::get<ORCore::Renderer>();

        for (size_t i = 0; i < bars.size(); i++) {
            float z = (bars[i].time / neck_speed_divisor);

            if (bars[i].type == BarType::measure)
            {
                obj.set_scale(glm::vec3{1.0f, 1.0f, 0.021});
            }
            else if (bars[i].type == BarType::upbeat)
            {
                obj.set_scale(glm::vec3{1.0f, 1.0f, 0.003});
            }
            else 
            {
                obj.set_scale(glm::vec3{1.0f, 1.0f, 0.01});
            }

            obj.set_translation(glm::vec3{0.0, 0.0f, z}); // center the line on the screen
            obj.set_primitive_type(ORCore::Primitive::triangle);
            obj.set_geometry(ORCore::create_rect_z_center_mesh(glm::vec4{1.0f,1.0f,1.0f,1.0f}));

            renderer.add_object(obj);
        }
    }
}