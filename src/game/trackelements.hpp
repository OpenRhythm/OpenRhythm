// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "renderer/shader.hpp"
#include "renderer/renderer.hpp"
#include "renderer/texture.hpp"

#include "song.hpp"
#include "game.hpp"

#include <vector>

namespace ORGame
{
	class TrackElements
	{
	public:
		TrackElements(CoreRenderIDs renderIDs);
        void init_bars(std::vector<BarEvent> &bars);
        void init_neck();
        void init_solo_sections();
        void init_power_sections();
        void update(double songTime);
	private:
        ORCore::TextureID m_neckTexture;
        ORCore::TextureID m_texture;
        ORCore::TextureID m_soloNeckTexture;

        ORCore::ProgramID m_neckProgram;

        ORCore::ObjectID m_neckObj;
        CoreRenderIDs m_renderIDs;
	};
}