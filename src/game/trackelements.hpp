// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "renderer.hpp"

#include "song.hpp"

#include <vector>

namespace ORGame
{
	class TrackElements
	{
	public:
        void init_bars(std::vector<BarEvent> &bars);
	private:
        ORCore::TextureID m_neckTexture;
        ORCore::TextureID m_soloNeckTexture;

        ORCore::ObjectID m_neckObj;
	};
}