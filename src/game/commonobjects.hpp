// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.
#pragma once

#include "renderer/shader.hpp"
#include "renderer/renderer.hpp"
#include "renderer/texture.hpp"

namespace ORGame
{

    static const float neck_speed_divisor = 0.5f;
    static const float neck_board_length = 2.0f;

    static const float hit_window_front = 0.085f;
    static const float hit_window_back = 0.085f;

    static const float videoOffset = 0.015f;
    static const float audioOffset = 0.0f;

    struct CoreRenderIDs
    {
        ORCore::CameraID cameraStatic;
        ORCore::CameraID cameraDynamic;
        ORCore::ProgramID program;
    };

}