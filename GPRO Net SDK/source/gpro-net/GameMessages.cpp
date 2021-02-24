#include "gpro-net/GameMessages.h"

Message::Message() 
{

	RakNet::NetworkIDManager networkIDManager;

	this->SetNetworkIDManager(&networkIDManager);
	networkID = GetNetworkID();
	typeID = ID_GAME_MESSAGE_1;
}

TextMessage::TextMessage(std::string name, std::string msg, std::string reciever = "ALL") : Message()
{
	myMessage.UserName = name;
	myMessage.Recipent = reciever;
	myMessage.Message = msg;
}