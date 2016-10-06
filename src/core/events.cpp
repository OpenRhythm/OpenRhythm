#include "events.hpp"

#include <algorithm>
#include <iostream>

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
        SDL_Event sdlevent;
        bool eventProcessed = false;
        Event eventContainer;

        while (SDL_PollEvent(&sdlevent)) {
            if (sdlevent.type == SDL_QUIT) {
                eventContainer = Event{Quit, 0.0, QuitEvent{true}};
                eventProcessed = true;
            } else if (sdlevent.type == SDL_MOUSEMOTION) {
                eventContainer = Event{MouseMove, 0.0, MouseMoveEvent{sdlevent.motion.x, sdlevent.motion.y}};
                eventProcessed = true;
            } else if (sdlevent.type == SDL_WINDOWEVENT) {
                const SDL_WindowEvent winEvent = sdlevent.window;
                if (winEvent.event == SDL_WINDOWEVENT_CLOSE) {
                    eventContainer = Event{WindowClose, 0.0, WindowCloseEvent{winEvent.windowID}};
                    eventProcessed = true;
                }
                else if (winEvent.event == SDL_WINDOWEVENT_RESIZED) {
                    eventContainer = Event{WindowSize, 0.0, WindowSizeEvent{winEvent.windowID, winEvent.data1, winEvent.data2}};
                    eventProcessed = true;
                }
            }

            if (eventProcessed) {
                m_events->broadcast_event(eventContainer);
            }

        }

    }

}