#include "gpro-net/MineLab.h"

Mine::Mine(float x, float y, float z, RakNet::NetworkIDManager* networkIDManager) {
	SetNetworkIDManager(networkIDManager);
	minePos.x = x;
	minePos.y = y;
	minePos.z = z;
}