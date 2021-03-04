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

BlackJackMoveMessage::BlackJackMoveMessage(std::string name, BlackJackMoves move) : Message()
{
	myMove.userName = name;
	myMove.move = move;
}

//write overloads
RakNet::BitStream& operator << (RakNet::BitStream& out, TextMessage::StringMessage& in)
{
	out.Write(in.UserName);
	out.Write(in.Recipent);
	out.Write(in.Message);
	return out;
}

RakNet::BitStream& operator << (RakNet::BitStream& out, BlackJackMoveMessage::MoveStruct& in)
{
	out.Write(in.userName);
	out.Write(in.move);
	return out;
}

//read overloads
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

RakNet::BitStream& operator >> (RakNet::BitStream& in, BlackJackMoveMessage::MoveStruct& out)
{
	bool sucess = in.Read(out.userName);
	assert(sucess);
	sucess = in.Read(out.move);
	assert(sucess);
	return in;
}