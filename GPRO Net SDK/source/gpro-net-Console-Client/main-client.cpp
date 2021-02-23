/*
   Copyright 2021 Daniel S. Buckstein

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

/*
	GPRO Net SDK: Networking framework.
	By Daniel S. Buckstein

	main-client.c/.cpp
	Main source for console client application.
*/

#include "gpro-net/gpro-net.h"
#include "gpro-net/MineLab.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>

#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"  // MessageID
#include "RakNet/GetTime.h"

#define SERVER_PORT 4024

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	ID_GAME_MESSAGE_2,
	ID_SEND_IDENTIFICATION,
	ID_SEND_LIST,
	ID_CREATE_MINE
};


int main(int const argc, char const* const argv[])
{

	char str[512];

	bool isConnected = false;

	std::string thisUserID;

	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet* packet;

	RakNet::SocketDescriptor sd;
	peer->Startup(1, &sd, 1);

	strcpy(str, "65.183.134.40");

	printf("Starting the client.\n");
	printf("Enter Username: \n");
	std::getline(std::cin, thisUserID);
	//printf("%s", thisUserID.c_str());

	peer->Connect(str, SERVER_PORT, 0, 0);

	while (1)
	{

		if (isConnected) 
		{
			//do the thing
			RakNet::BitStream bsOut;
			std::string userID;
			std::string userMessage;
			std::string userSelection;

			printf("Press r to recieve messages \nPress d to dm someone \nPress a to send a message to everyone\nPress l to list all connected users\n");
			std::getline(std::cin, userSelection);

			switch (userSelection[0]) 
			{
				case 'r':
					break;
				case 'd':
					printf("Who do you wish to connect to: ");
					std::getline(std::cin, userID);

					printf("Message: ");
					std::getline(std::cin, userMessage);

					bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_2);
					bsOut.Write((RakNet::Time)RakNet::GetTime());
					bsOut.Write(thisUserID.c_str());
					bsOut.Write(userID.c_str());
					bsOut.Write(userMessage.c_str());
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
					continue;
					break;
				case 'a':
					printf("Message: ");
					std::getline(std::cin, userMessage);

					bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
					bsOut.Write((RakNet::Time)RakNet::GetTime());
					bsOut.Write(thisUserID.c_str());
					bsOut.Write(userMessage.c_str());
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
					continue;
					break;
				case 'l':
					bsOut.Write((RakNet::MessageID)ID_SEND_LIST);
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
					break;
				default:
					printf("Invalid Input");
					continue;
					break;
			}
		}

		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
				case ID_CONNECTION_REQUEST_ACCEPTED:
					{
						printf("Our connection request has been accepted.\n");
						// Use a BitStream to write a custom user message
						// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
						isConnected = true;
						RakNet::BitStream bsOut;
						bsOut.Write((RakNet::MessageID)ID_SEND_IDENTIFICATION);
						bsOut.Write(thisUserID.c_str());
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false); //Send User defined ID to server
						/*
						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
						bsOut.Write((RakNet::Time)RakNet::GetTime());
						bsOut.Write("Hello world");
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);

						bsOut.Reset();
						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_2);
						bsOut.Write((RakNet::Time)RakNet::GetTime());
						bsOut.Write("Hello squirel");
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
						*/

						Mine red(1f,2f,3f)
					}
					break;
				case ID_REMOTE_NEW_INCOMING_CONNECTION:
					{
						RakNet::RakString rs;
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs);
						printf("%s has connected.\n", rs.C_String());
					}
					break;
				case ID_REMOTE_DISCONNECTION_NOTIFICATION:
					{
						RakNet::RakString rs;
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs);
						printf("%s has disconnected.\n", rs.C_String());
					}
					break;
				case ID_REMOTE_CONNECTION_LOST:
					{
						RakNet::RakString rs;
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs);
						printf("%s has lost the connection.\n", rs.C_String());	
					}
					break;
				case ID_NO_FREE_INCOMING_CONNECTIONS:
					printf("The server is full.\n");
					isConnected = false;
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					printf("We have been disconnected.\n");
					isConnected = false;
					break;
				case ID_CONNECTION_LOST:
					printf("Connection lost.\n");
					isConnected = false;
					break;
				case ID_GAME_MESSAGE_1:
					{
						RakNet::RakString rs;
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs);
						printf("%s\n" , rs.C_String());
					}
					break;
				default:
					printf("Message with identifier %i has arrived.\n", packet->data[0]);
					break;
			}
		}
	}
}
