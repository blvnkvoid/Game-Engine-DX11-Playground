#pragma once
#include <vector>
#include "EventList.h"

class EventSession
{
public:
    void StartSingleEvent(EventSelection event)
    {
        m_events = { event };
        m_currentIndex = 0;
        m_active = true;
    }

    void StartChampionship(const std::vector<EventSelection>& events)
    {
        m_events = events;
        m_currentIndex = 0;
        m_active = true;
    }

    EventSelection GetCurrentEvent() const
    {
        return m_events[m_currentIndex];
    }

    bool Advance()
    {
        m_currentIndex++;

        if (m_currentIndex >= m_events.size())
        {
            m_active = false;
            return false;
        }

        return true;
    }

    bool IsActive() const
    {
        return m_active;
    }

private:
    std::vector<EventSelection> m_events;
    int m_currentIndex = 0;
    bool m_active = false;
};