#include "EventRegistry.h"
#include <vector>

EventDefinition EventRegistry::Create(EventSelection selection)
{
    switch (selection)
    {
    case EventSelection::SundayCupTsukuba:
        return CreateSundayCup_Tsukuba();

    case EventSelection::SundayCupSpa:
        return CreateSundayCup_Spa();
    }

    throw std::runtime_error("Unknown event.");
}   

EventSession EventRegistry::CreateSession(
    EventLaunchType launchType,
    EventSelection selectedEvent)
{
    EventSession session;

    if (launchType == EventLaunchType::SingleEvent)
    {
        session.StartSingleEvent(selectedEvent);
        return session;
    }

    if (launchType == EventLaunchType::Championship)
    {
        // For now, only one championship exists.
        // Later this becomes a table.
        session.StartChampionship(
            {
               EventSelection::SundayCupSpa,
               EventSelection::SundayCupTsukuba
            });

        return session;
    }

    session.StartSingleEvent(selectedEvent);
    return session;
}