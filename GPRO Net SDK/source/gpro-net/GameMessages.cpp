#include "gpro-net/GameMessages.h"

Message::Message() 
{

	RakNet::NetworkIDManager networkIDManager;

	this->SetNetworkIDManager(&networkIDManager);
	networkID = GetNetworkID();
	typeID = ID_GAME_MESSAGE_1;
}

TextMessage::TextMessage() : Message()
{

}