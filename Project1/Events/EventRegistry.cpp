#include "EventRegistry.h"
#include "ChampionshipDefinition.h"
#include <vector>

EventDefinition EventRegistry::Create(EventSelection selection)
{
    switch (selection)
    {
    case EventSelection::SundayCupTsukuba:
        return CreateSundayCup_Tsukuba();

    case EventSelection::SundayCupSpa:
        return CreateSundayCup_Spa();

    case EventSelection::SundayCupTrialMountain:
        return CreateSundayCup_TrialMountain();
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
        ChampionshipDefinition championship = CreateSundayCup();

        session.StartChampionship(championship.events);

        return session;
    }

    session.StartSingleEvent(selectedEvent);
    return session;
}