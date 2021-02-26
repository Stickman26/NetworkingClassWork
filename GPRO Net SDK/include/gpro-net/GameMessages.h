#pragma once

#include <string>

#include "RakNet/RakNetTypes.h"  // MessageID
#include "RakNet/MessageIdentifiers.h"
#include "RakNet/BitStream.h"
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

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	ID_GAME_MESSAGE_2,
	ID_SEND_IDENTIFICATION,
	ID_SEND_LIST,
	ID_CREATE_MINE,
	ID_CREATE_ROOM,
	ID_JOIN_ROOM,
	ID_MESSAGE_SPECTATORS,
	ID_MESSAGE_PLAYERS,
	ID_STRUCT_TEST
};

class Message : public RakNet::NetworkIDObject
{
public:
	Message();
	//~Message();

	RakNet::NetworkID networkID;
	RakNet::SystemAddress systemAddress;
	unsigned char typeID;

	//virtual void Read(RakNet::BitStream* bs) = 0;
	//virtual void Write(RakNet::BitStream* bs) = 0;

	RakNet::NetworkID GetNetworkID() { return networkID; }
};

class TextMessage : public Message 
{
public:
	struct StringMessage
	{
		std::string UserName;
		std::string Recipent;
		std::string Message;
	} myMessage;

	TextMessage(std::string name, std::string msg, std::string reciever = "ALL");
	//~TextMessage();

};

enum BlackJackMoves
{
	Hit,
	Stay
};

class BlackJackMove : public Message
{

};