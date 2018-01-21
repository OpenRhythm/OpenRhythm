// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "guitar.hpp"

#include "resolver.hpp"

#include <iostream>



namespace ORGame
{
    static const std::map<NoteType, glm::vec4> noteColorMap = {
        {NoteType::Green, {0.0f,1.0f,0.0f,1.0f}},
        {NoteType::Red, {1.0f,0.0f,0.0f,1.0f}},
        {NoteType::Yellow, {1.0f,1.0f,0.0f,1.0f}},
        {NoteType::Blue, {0.0f,0.0f,1.0f,1.0f}},
        {NoteType::Orange, {1.0f,0.5f,0.0f,1.0f}},
    };
    
    static const std::map<NoteType, glm::vec4> noteColorMapActive = {
        {NoteType::Green, {0.35f,1.5f,0.35f,1.0f}},
        {NoteType::Red, {1.5f,0.35f,0.35f,1.0f}},
        {NoteType::Yellow, {1.5f,1.5f,0.35f,1.0f}},
        {NoteType::Blue, {0.25f,1.5f,4.5f,1.0f}},
        {NoteType::Orange, {3.0f,1.5f,0.5f,1.0f}},
    };

    void Guitar::set_renderInfo(CoreRenderIDs renderIDs)
    {
        m_renderIDs = renderIDs;
    }

    void Guitar::init_frets()
    {
        auto& renderer = ORCore::Resolver::get<ORCore::Renderer>();

        m_fretsTexture = renderer.add_texture(ORCore::loadSTB("data/frets.png"));

        ORCore::RenderObject obj;

        // Frets
        obj.set_program(m_renderIDs.program);
        obj.set_camera(m_renderIDs.cameraStatic);
        obj.set_texture(m_fretsTexture);
        obj.set_scale(glm::vec3{1.0f, 1.0f, 0.05f});
        obj.set_translation(glm::vec3{0.0f, 0.0f, 0.0f}); // center the line on the screen
        obj.set_primitive_type(ORCore::Primitive::triangle);
        obj.set_geometry(ORCore::create_rect_z_center_mesh(glm::vec4{1.0f,1.0f,1.0f,1.0f}));
        m_fretObj = renderer.add_object(obj);


        // // Top of frets
        // obj.set_texture(-1);

        // float laneWidth = 1.0f/5.0f;
        // float objheight = laneWidth/3.0f;

        // for (int i = 0; i < 5; i++)
        // {
        //     obj.set_scale(glm::vec3{laneWidth, objheight/7.0f, objheight/2.5f});
        //     obj.set_translation(glm::vec3{i*laneWidth, 0.0f, 0.0f});
        //     obj.set_geometry(ORCore::create_cube_mesh(glm::vec4{1.0f,1.0f,1.0f,0.0f}));
        //     auto objID = renderer.add_object(obj);
        //     m_buttonRender.push_back(objID);
        // }

    }

    void Guitar::init_notes(std::vector<TrackNote> &notes)
    {
        auto& renderer = ORCore::Resolver::get<ORCore::Renderer>();

        m_tailTexture = renderer.add_texture(ORCore::loadSTB("data/tail.png"));

        std::cout << "Note Count: " << notes.size() << std::endl;

        // reuse the same container when creating notes as add_obj wont modify the original.
        ORCore::RenderObject obj;
        obj.set_camera(m_renderIDs.cameraDynamic);
        obj.set_program(m_renderIDs.program);
        obj.set_primitive_type(ORCore::Primitive::triangle);

        float noteWidth = 1.0f/5.0f;
        float tailWidth = noteWidth/3.0f;

        bool firstTailMarked = false;

        for (auto &note : notes)
        {
            float z = note.time / neck_speed_divisor;
            glm::vec4 color;
            try
            {
                color = noteColorMap.at(note.type);
            }
            catch (std::out_of_range &err) {
                color = glm::vec4{1.0f,1.0f,1.0f,1.0f};
            }

            float noteLength = note.length/neck_speed_divisor;

            obj.set_scale(glm::vec3{tailWidth, 1.0f, noteLength});
            obj.set_translation(glm::vec3{(static_cast<int>(note.type)*noteWidth) - noteWidth+tailWidth, 0.0f, z}); // center the line on the screen
            obj.set_geometry(ORCore::create_rect_z_mesh(color));
            obj.set_texture(m_tailTexture);

            note.objTailID = renderer.add_object(obj);
            firstTailMarked = true;

            obj.set_texture(-1); // -1 gets set to the default texture.

            if (note.isHopo)
            {
                obj.set_scale(glm::vec3{noteWidth*0.75, tailWidth/2.5f, tailWidth/2.5f});
                obj.set_translation(glm::vec3{(static_cast<int>(note.type)*noteWidth) - (noteWidth*0.875), 0.0f, z}); // center the line on the screen
                obj.set_geometry(ORCore::create_cube_mesh(glm::vec4{1.0f,1.0f,1.0f,1.0f}));
            }
            else
            {
                obj.set_scale(glm::vec3{noteWidth, tailWidth/2.0f, tailWidth/2.0f});
                obj.set_translation(glm::vec3{(static_cast<int>(note.type)*noteWidth) - noteWidth, 0.0f, z}); // center the line on the screen
                obj.set_geometry(ORCore::create_cube_mesh(color));
            }

            note.objNoteID = renderer.add_object(obj);
        }
    }

    void Guitar::do_hit_detection(std::vector<TrackNote*> &notesInWindow, double songTime)
    {
        auto& renderer = ORCore::Resolver::get<ORCore::Renderer>();

        glm::vec4 color;

        for (auto *note : notesInWindow)
        {
            if (!note->played && note->time + videoOffset <= songTime)
            {
                try
                {
                    color = noteColorMapActive.at(note->type);
                }
                catch (std::out_of_range &err)
                {
                    color = glm::vec4{1.0f,1.0f,1.0f,1.0f};
                }
                color[3] = 0.0f; // Dissapear notes

                auto *noteObj = renderer.get_object(note->objNoteID);
                noteObj->set_geometry(ORCore::create_cube_mesh(color));
                renderer.update_object(note->objNoteID);

                note->played = true;
                m_heldNotes.push_back(note);
            }
        }

        // Todo - do we need to think about video cal here if we are talking about scoring?
        m_heldNotes.erase(std::remove_if(m_heldNotes.begin(), m_heldNotes.end(),
                [&](TrackNote *note)
                {
                    // Hide notes
                    auto *tailObj = renderer.get_object(note->objTailID);
                    tailObj->set_geometry(ORCore::create_rect_z_mesh(glm::vec4{1.0f,1.0f,1.0f,0.0f}));
                    renderer.update_object(note->objTailID);
                    return (note->time + note->length) <= songTime;
                }),
                m_heldNotes.end());
    }

    void Guitar::update(double songTime)
    {
        auto& renderer = ORCore::Resolver::get<ORCore::Renderer>();

        glm::vec4 color;

        for (auto *note : m_heldNotes)
        {
            try
            {
                color = noteColorMapActive.at(note->type);
            }
            catch (std::out_of_range &err)
            {
                color = glm::vec4{1.0f,1.0f,1.0f,1.0f};
            }

            auto *tailObj = renderer.get_object(note->objTailID);

            float noteWidth = 1.0f/5.0f;
            float tailWidth = noteWidth/3.0f;

            float z = songTime / neck_speed_divisor;
            float noteLength = (note->length - (songTime-note->time)) / neck_speed_divisor;

            tailObj->set_scale(glm::vec3{tailWidth, 1.0f, noteLength});
            tailObj->set_translation(glm::vec3{(static_cast<int>(note->type)*noteWidth) - noteWidth+tailWidth, 0.0f, z}); // center the line on the screen

            tailObj->set_geometry(ORCore::create_rect_z_mesh(color));

            renderer.update_object(note->objTailID);
        }
    }
}