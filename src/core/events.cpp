// Copyright (c) 2015-2017 Matthew Sitton <matthewsitton@gmail.com>
// See LICENSE in the project root for license information.

#include "events.hpp"

#include <algorithm>
#include <thread>
#include <iostream>
#include <stdexcept>

namespace ORCore
{

    // Event Manager methods

    void EventManager::add_listener(Listener &listener)
    {
        static int id {0};

        id++;
        listener.id = id;

        m_listeners.push_back(&listener);
    }

    void EventManager::remove_listener(Listener &listener)
    {
        auto vecEnd = std::end(m_listeners);
        auto vecLoc = std::find(std::begin(m_listeners), vecEnd, &listener);

        if (vecLoc != vecEnd)
        {
            m_listeners.erase(vecLoc);
        }
    }

    void EventManager::broadcast_event(const Event &event)
    {
        for (Listener *listener : m_listeners)
        {
            if ((listener->mask & event.type) != EventNone)
            {
                listener->handler(event);
            }
        }
    }

    // SDL Event Pump methods

    EventPumpSDL2::EventPumpSDL2(EventManager *events)
    : m_events(events)
    {
        SDL_InitSubSystem(SDL_INIT_EVENTS);
        SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER);

        SDL_GameController *controller = nullptr;
        SDL_Joystick *joy = nullptr;

        std::cout << SDL_NumJoysticks() << std::endl;
        for (int i = 0; i < SDL_NumJoysticks(); ++i) {
            if (SDL_IsGameController(i)) {
                std::cout << SDL_GameControllerNameForIndex(i) << std::endl;
                controller = SDL_GameControllerOpen(i);
                if (controller) {
                    break;
                } else {
                    throw std::runtime_error("Error loading joystick");
                }
            }
            else
            {
                joy = SDL_JoystickOpen(i);

                std::cout << "Not game controller, falling back to sdl joystick" << std::endl;
                std::cout << "Name: " << SDL_JoystickNameForIndex(i) << std::endl;
            }
        }
    }

    void EventPumpSDL2::process()
    {
        SDL_Event sdlEvent;

        while (SDL_PollEvent(&sdlEvent))
        {
            bool eventProcessed = false;
            Event eventContainer;
            if (sdlEvent.type == SDL_QUIT)
            {
                eventContainer = Event{Quit, 0.0, QuitEvent{true}};
                eventProcessed = true;
            }
            else if (sdlEvent.type == SDL_CONTROLLERBUTTONDOWN)
            {
                if (sdlEvent.cbutton.state == SDL_PRESSED)
                {
                    eventContainer = Event{GuitarControllerDown, 0.0, GuitarControllerDownEvent{xplorerMap[sdlEvent.cbutton.button]}};
                }
                eventProcessed = true;
            }
            else if (sdlEvent.type == SDL_CONTROLLERBUTTONUP)
            {
                if (sdlEvent.cbutton.state == SDL_RELEASED)
                {
                    eventContainer = Event{GuitarControllerUp, 0.0, GuitarControllerUpEvent{xplorerMap[sdlEvent.cbutton.button]}};
                }
                eventProcessed = true;
            }
            else if (sdlEvent.type == SDL_JOYBUTTONDOWN)
            {
                if (sdlEvent.jbutton.state == SDL_PRESSED)
                {
                    eventContainer = Event{GuitarControllerDown, 0.0, GuitarControllerDownEvent{xplorerJoyMap[sdlEvent.jbutton.button]}};
                }
                eventProcessed = true;
            }
            else if (sdlEvent.type == SDL_JOYBUTTONUP)
            {
                if (sdlEvent.jbutton.state == SDL_RELEASED)
                {
                    eventContainer = Event{GuitarControllerUp, 0.0, GuitarControllerDownEvent{xplorerJoyMap[sdlEvent.jbutton.button]}};
                }
                eventProcessed = true;
            }
            else if (sdlEvent.jhat.type == SDL_JOYHATMOTION)
            {
                if (sdlEvent.jhat.value == SDL_HAT_UP)
                {
                    eventContainer = Event{GuitarControllerUp, 0.0, GuitarControllerUpEvent{GuitarController::STRUM_D}};
                }
                else if (sdlEvent.jhat.value == SDL_HAT_DOWN)
                {
                    eventContainer = Event{GuitarControllerUp, 0.0, GuitarControllerUpEvent{GuitarController::STRUM_D}};
                }
                if (sdlEvent.jhat.value == SDL_HAT_CENTERED)
                {
                    eventContainer = Event{GuitarControllerDown, 0.0, GuitarControllerUpEvent{GuitarController::STRUM_D}};
                }
                eventProcessed = true;
            }
            else if (sdlEvent.type == SDL_MOUSEMOTION)
            {
                eventContainer = Event{MouseMove, 0.0, MouseMoveEvent{sdlEvent.motion.x, sdlEvent.motion.y}};
                eventProcessed = true;
            }
            else if (sdlEvent.type == SDL_KEYDOWN)
            {
                eventContainer = Event{KeyDown, 0.0, KeyDownEvent{keyMap[sdlEvent.key.keysym.scancode], static_cast<ModFlag>(sdlEvent.key.keysym.mod)}};
                eventProcessed = true;
            }
            else if (sdlEvent.type == SDL_KEYUP)
            {
                eventContainer = Event{KeyUp, 0.0, KeyUpEvent{keyMap[sdlEvent.key.keysym.scancode], static_cast<ModFlag>(sdlEvent.key.keysym.mod)}};
                eventProcessed = true;
            }
            else if (sdlEvent.type == SDL_WINDOWEVENT)
            {
                const SDL_WindowEvent winEvent = sdlEvent.window;
                if (winEvent.event == SDL_WINDOWEVENT_CLOSE)
                {
                    eventContainer = Event{WindowClose, 0.0, WindowCloseEvent{winEvent.windowID}};
                    eventProcessed = true;
                }
                else if (winEvent.event == SDL_WINDOWEVENT_RESIZED)
                {
                    eventContainer = Event{WindowSize, 0.0, WindowSizeEvent{winEvent.windowID, winEvent.data1, winEvent.data2}};
                    eventProcessed = true;
                }
            }

            if (eventProcessed)
            {
                m_events->broadcast_event(eventContainer);
            }

        }

    }

} // namespace ORCore
