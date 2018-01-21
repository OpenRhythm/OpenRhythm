// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.
#pragma once

#include "renderer/shader.hpp"
#include "renderer/renderer.hpp"
#include "renderer/texture.hpp"

#include "song.hpp"
#include "commonobjects.hpp"

#include <vector>
#include <cstdint>

namespace ORGame
{
    class Guitar
    {
    public:
        void set_renderInfo(CoreRenderIDs renderIDs);

        void init_frets();
        void init_notes(std::vector<TrackNote> &notes);
        void do_hit_detection(std::vector<TrackNote*> &notesInWindow, double songTime);
        void update(double songTime);

    private:
        ORCore::TextureID m_tailTexture;
        ORCore::TextureID m_fretsTexture;
        ORCore::ProgramID m_tailProgram;
        
        ORCore::ObjectID m_fretObj;
        std::vector<TrackNote*> m_heldNotes;

        CoreRenderIDs m_renderIDs;
    };
}