#pragma once
#include "RakNet/NetworkIDObject.h"
#include "RakNet/NetworkIDManager.h"

/*
objectives are to create a base Message class, 
design this to have some of the common elements 
that every message should contain such as the 
networkID of the game object that would be sending
this message and who the owner of the message/game 
object is either GUID or system address

inherit from this class to include custom data
that may be unique to the game object 
(soldier/mine/enemy etc) store the data sets as
structs i.e if position is the only necessary 
information then you can have a vec3 struct 
for this derived class

Finally overload the >> and << bitstream operators 
to be able to write this data to and read from bitstreams
*/

struct MinePosition
{
	float x, y, z;
};

//Make a message base class

class Mine : public RakNet::NetworkIDObject
{
private:
	MinePosition minePos;

public:
	Mine(float x, float y, float z, RakNet::NetworkIDManager* networkIDManager);

	MinePosition GetPosition() { return minePos; }
};