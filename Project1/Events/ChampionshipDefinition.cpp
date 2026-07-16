#include "ChampionshipDefinition.h"

ChampionshipDefinition CreateSundayCup()
{
    ChampionshipDefinition cup;

    cup.events =
    {
        EventSelection::SundayCupTsukuba,
        EventSelection::SundayCupSpa,
        EventSelection::SundayCupTrialMountain
    };

    return cup;
}