#include "gpro-net/GameMessages.h"

Message::Message() 
{

	RakNet::NetworkIDManager networkIDManager;

	this->SetNetworkIDManager(&networkIDManager);
	networkID = this->GetNetworkID();
	typeID = ID_GAME_MESSAGE_1;
}

TextMessage::TextMessage(std::string name, std::string msg, std::string reciever) : Message()
{
	myMessage.UserName = name;
	myMessage.Recipent = reciever;
	myMessage.Message = msg;
}

//write
RakNet::BitStream& operator << (RakNet::BitStream& out, TextMessage::StringMessage& in)
{
	out.Write(in.UserName);
	out.Write(in.Recipent);
	out.Write(in.Message);
	return out;
}

//read
RakNet::BitStream& operator >> (RakNet::BitStream& in, TextMessage::StringMessage& out)
{
	bool sucess = in.Read(out.UserName);
	assert(sucess);
	sucess = in.Read(out.Recipent);
	assert(sucess);
	sucess = in.Read(out.Message);
	assert(sucess);
	return in;
}