#include "events.hpp"

#include <algorithm>
#include <thread>

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

        if (vecLoc != vecEnd) {
            m_listeners.erase(vecLoc);
        }
    }

    void EventManager::broadcast_event(const Event &event)
    {
        for (Listener *listener : m_listeners) {
            if ((listener->mask & event.type) != EventNone) {
                listener->handler(event);
            }
        }
    }

    // SDL Event Pump methods

    EventPumpSDL2::EventPumpSDL2(EventManager *events)
    : m_events(events)
    {
        SDL_InitSubSystem(SDL_INIT_EVENTS);
    }

    void EventPumpSDL2::process()
    {
        SDL_Event sdlEvent;

        while (SDL_PollEvent(&sdlEvent)) {
            bool eventProcessed = false;
            Event eventContainer;
            if (sdlEvent.type == SDL_QUIT) {
                eventContainer = Event{Quit, 0.0, QuitEvent{true}};
                eventProcessed = true;
            } else if (sdlEvent.type == SDL_MOUSEMOTION) {
                eventContainer = Event{MouseMove, 0.0, MouseMoveEvent{sdlEvent.motion.x, sdlEvent.motion.y}};
                eventProcessed = true;
            } else if (sdlEvent.type == SDL_KEYDOWN) {
                eventContainer = Event{KeyDown, 0.0, KeyDownEvent{keyMap[sdlEvent.key.keysym.scancode], static_cast<ModFlag>(sdlEvent.key.keysym.mod)}};
                eventProcessed = true;
            } else if (sdlEvent.type == SDL_KEYUP) {
                eventContainer = Event{KeyUp, 0.0, KeyUpEvent{keyMap[sdlEvent.key.keysym.scancode], static_cast<ModFlag>(sdlEvent.key.keysym.mod)}};
                eventProcessed = true;
            } else if (sdlEvent.type == SDL_WINDOWEVENT) {
                const SDL_WindowEvent winEvent = sdlEvent.window;
                if (winEvent.event == SDL_WINDOWEVENT_CLOSE) {
                    eventContainer = Event{WindowClose, 0.0, WindowCloseEvent{winEvent.windowID}};
                    eventProcessed = true;
                } else if (winEvent.event == SDL_WINDOWEVENT_RESIZED) {
                    eventContainer = Event{WindowSize, 0.0, WindowSizeEvent{winEvent.windowID, winEvent.data1, winEvent.data2}};
                    eventProcessed = true;
                }
            }

            if (eventProcessed) {
                m_events->broadcast_event(eventContainer);
            }

        }

    }

} // namespace ORCore
