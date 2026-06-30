#include "Spawner.h"
#include "../SharedTypes.h"
#include <bullet/btBulletDynamicsCommon.h>

TrackSelection GameConfig::activeTrack = TrackSelection::TestCourse;
Spawner::Spawner()
{

}

Spawner::~Spawner()
{
}

btTransform Spawner::PutCar()
{

	btTransform startPos;
	startPos.setIdentity();


	switch (GameConfig::activeTrack)
	{
	case TrackSelection::AutumnRing:
		startPos.setOrigin(btVector3(-23.0f, 7.0f, 176.0f));	
		break;	
	case TrackSelection::ElCapitan:
		startPos.setOrigin(btVector3(0.0f, 0.0f, 0.0f));
		break;
	case TrackSelection::Spa:
		startPos.setOrigin(btVector3(-314.0f, 24.0f, 730.0f)); // Spawamy w powietrzu!	
		break;	
	case TrackSelection::Nordschleife:
		startPos.setOrigin(btVector3(-449.0f, 140.0F, -2351.0f)); // Spawamy w powietrzu!	
		break;
	case TrackSelection::GrandValley:
		startPos.setOrigin(btVector3(-283.0f, -20.0f, 50.0f));
		break;
	case TrackSelection::TrialMountain:
		startPos.setOrigin(btVector3(-5.0f, 0.0f, 200.0f));
		break;
	case TrackSelection::HighSpeedRing:
		startPos.setOrigin(btVector3(0.0f, 0.0f, 5.0f));
		break;
	case TrackSelection::MidfieldRaceway:
		startPos.setOrigin(btVector3(22.0f, 8.0f, 50.0f));
		break;
	case TrackSelection::TestCourse:
		startPos.setOrigin(btVector3(-970.0f, 5.0f, 1710.0f));
		break;		
	case TrackSelection::RouteX:
		startPos.setOrigin(btVector3(-3205.0f, 10.0f, 4645.0f));
		break;	
	case TrackSelection::RoadAtlanta:
		startPos.setOrigin(btVector3(0.0f, 5.0f, 0.0f));
		break;		
	
	case TrackSelection::Motorland:
		startPos.setOrigin(btVector3(-85.0f, 25.0f, -18.0f));
		break;	
	
	case TrackSelection::BeginnerCourse:
		startPos.setOrigin(btVector3(-47.0f, 20.0f, 60.0f));
		break;	
	
	case TrackSelection::Tsukuba:
		startPos.setOrigin(btVector3(-204.0f, 15.0f, 170.0f));
		break;	
	
	case TrackSelection::LeMans:
		startPos.setOrigin(btVector3(-1069.0f, 55.0f, 930.0f));
		break;	
	
	case TrackSelection::DeepForest:
		startPos.setOrigin(btVector3(21.0f, 35.0f, 100.0f));
		break;	
	
	case TrackSelection::SSR5:
		startPos.setOrigin(btVector3(-2.0f, 1.0f, 0.0f));
		break;
	default:
		startPos.setOrigin(btVector3(0.0f, 0.0f, 0.0f)); // Spawamy w powietrzu!
		break;
	}
	return startPos;
}


btTransform Spawner::ResetPos()
{
	btTransform startPos;
	startPos.setIdentity();

	switch (GameConfig::activeTrack)
	{
	case TrackSelection::AutumnRing:
		startPos.setOrigin(btVector3(-23.0f, 7.0f, 176.0f));
		break;	
	case TrackSelection::ElCapitan:
		startPos.setOrigin(btVector3(0.0f, 0.0f, 0.0f));
		break;	
	case TrackSelection::Spa:
		startPos.setOrigin(btVector3(-314.0f, 24.0f, 730.0f)); // Spawamy w powietrzu!	
		break;	
	case TrackSelection::GrandValley:
		startPos.setOrigin(btVector3(-283.0f, -20.0f, 50.0f));
		break;
	case TrackSelection::TrialMountain:
		startPos.setOrigin(btVector3(-5.0f, 0.0f, 200.0f));
		break;
	case TrackSelection::HighSpeedRing:
		startPos.setOrigin(btVector3(0.0f, 0.0f, 5.0f));
		break;
	case TrackSelection::MidfieldRaceway:
		startPos.setOrigin(btVector3(22.0f, 8.0f, 50.0f));
		break;
	case TrackSelection::TestCourse:
		startPos.setOrigin(btVector3(-970.0f, 5.0f, 1710.0f));
		break;
	case TrackSelection::RoadAtlanta:
		startPos.setOrigin(btVector3(0.0f, 5.0f, 0.0f));
		break;
	case TrackSelection::Nordschleife:
		startPos.setOrigin(btVector3(-449.0f, 140.0F, -2351.0f)); // Spawamy w powietrzu!	
		break;
	case TrackSelection::BeginnerCourse:
		startPos.setOrigin(btVector3(-47.0f, 20.0f, 60.0f));
		break;
	case TrackSelection::Motorland:
		startPos.setOrigin(btVector3(-85.0f, 25.0f, -18.0f));
		break;
	case TrackSelection::Tsukuba:
		startPos.setOrigin(btVector3(-204.0f, 15.0f, 170.0f));
		break;

	case TrackSelection::LeMans:
		startPos.setOrigin(btVector3(-1069.0f, 55.0f, 930.0f));
		break;

	case TrackSelection::DeepForest:
		startPos.setOrigin(btVector3(21.0f, 35.0f, 100.0f));
		break;

	case TrackSelection::SSR5:
		startPos.setOrigin(btVector3(-2.0f, 1.0f, 0.0f));
		break;

	default:
		startPos.setOrigin(btVector3(0.0f, 0.0f, 0.0f));

		break;
	}


	return startPos;
}