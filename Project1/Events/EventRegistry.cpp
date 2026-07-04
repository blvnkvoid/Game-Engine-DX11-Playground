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

