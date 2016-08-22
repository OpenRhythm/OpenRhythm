#include "events.hpp"

#include <algorithm>
#include <iostream>

namespace ORCore
{

    Events::Events()
    {
        SDL_InitSubSystem(SDL_INIT_EVENTS);
    }

    void Events::add_listener(Listener &listener)
    {
        static int id {0};

        id++;
        listener.id = id;

        m_listeners.push_back(&listener);

    }

    void Events::remove_listener(Listener &listener)
    {
        auto vecEnd = std::end(m_listeners);
        auto vecLoc = std::find(std::begin(m_listeners), vecEnd, &listener);

        if (vecLoc != vecEnd) {
            m_listeners.erase(vecLoc);
        }

    }

    void Events::broadcast_event(Event &event)
    {
        for (Listener *listener : m_listeners) {
            if ((listener->mask & event.type) != EventType::EventNone) {
                listener->handler(event);
            }
        }

    }

    void Events::process()
    {
        SDL_Event sdlevent;
        bool eventProcessed = false;

        while (SDL_PollEvent(&sdlevent)) {
            if (sdlevent.type == SDL_QUIT) {
                QuitEvent event {true};

                m_event.type = EventType::Quit;
                m_event.time = 0.0;                    // not used currently
                m_event.event.quit = event;

                eventProcessed = true;
            } else if (sdlevent.type == SDL_MOUSEMOTION) {
                MouseMoveEvent event {sdlevent.motion.x, sdlevent.motion.y};
                m_event.type = EventType::MouseMove;
                m_event.time = 0.0;                    // not used currently
                m_event.event.mouseMove = event;
                eventProcessed = true;
            } else if (sdlevent.type == SDL_WINDOWEVENT) {
                const SDL_WindowEvent winEvent = sdlevent.window;
                if (winEvent.event == SDL_WINDOWEVENT_CLOSE) {
                    WindowCloseEvent event {winEvent.windowID};
                    m_event.type = EventType::WindowClose;
                    m_event.time = 0.0;                    // not used currently
                    m_event.event.windowClose = event;
                    eventProcessed = true;
                }
                else if (winEvent.event == SDL_WINDOWEVENT_RESIZED) {
                    WindowSizeEvent event {winEvent.windowID, winEvent.data1, winEvent.data2};
                    m_event.type = EventType::WindowSized;
                    m_event.time = 0.0;                    // not used currently
                    m_event.event.windowSized = event;
                    eventProcessed = true;
                }

            }

            if (eventProcessed == true) {
                broadcast_event(m_event);
            }

        }

    }

}