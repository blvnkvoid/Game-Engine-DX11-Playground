#pragma once

class btTransform;

class Spawner
{
public:
	Spawner();
	~Spawner();
	

	btTransform PutCar();
	btTransform ResetPos();
};