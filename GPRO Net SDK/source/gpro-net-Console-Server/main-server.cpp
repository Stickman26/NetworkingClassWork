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

	main-server.c/.cpp
	Main source for console server application.
*/

#include "gpro-net/gpro-net.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <sstream>


#include "RakNet/RakPeerInterface.h"
#include "RakNet/MessageIdentifiers.h"
#include "RakNet/BitStream.h"
#include "RakNet/RakNetTypes.h"  // MessageID
#include "RakNet/GetTime.h"

#define MAX_CLIENTS 10
#define SERVER_PORT 4024

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1,
	ID_GAME_MESSAGE_2,
	ID_SEND_IDENTIFICATION,
	ID_SEND_LIST
};

std::string ConvertTime(RakNet::Time ts) {
	int timelump = (int)ts;

	timelump /= 1000;
	int seconds = timelump % 60;
	timelump /= 60;
	int minutes = timelump % 60;
	timelump /= 60;
	int hours = timelump % 24;
	timelump /= 24;
	int days = timelump;

	std::stringstream timestamp;
	timestamp << std::setw(2) << std::setfill('0') << "[" << hours << ":" << minutes << ":" << seconds << "]";
	return timestamp.str();
}

int main(int const argc, char const* const argv[])
{

	std::ofstream messages;

	//Add a map for users and their IPs
	std::map<std::string, RakNet::SystemAddress> userList;

	RakNet::RakPeerInterface* peer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet* packet;

	RakNet::SocketDescriptor sd(SERVER_PORT, 0);
	peer->Startup(MAX_CLIENTS, &sd, 1);
	messages.open("logs.txt", std::fstream::out | std::fstream::trunc);
	messages.close();

	printf("Starting the server.\n");
	// We need to let the server accept incoming connections from the clients
	peer->SetMaximumIncomingConnections(MAX_CLIENTS);

	while(1)
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			switch (packet->data[0])
			{
				case ID_NEW_INCOMING_CONNECTION:
					{
						printf("A connection is incoming.\n");
					}
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					{
						std::string rs;

						for (std::map<std::string, RakNet::SystemAddress>::const_iterator it = userList.begin(); it != userList.end(); ++it)
						{
							if (it->second == packet->systemAddress) 
							{
								rs = it->first;
								userList.erase(it);
								break;
							}
						}

						printf("%s has disconnected.\n", rs.c_str());

						RakNet::BitStream bsOut;
						bsOut.Write((RakNet::MessageID)ID_REMOTE_DISCONNECTION_NOTIFICATION);
						bsOut.Write(rs.c_str());
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);	
					}
					break;
				case ID_CONNECTION_LOST:
					{
						std::string rs;

						for (std::map<std::string, RakNet::SystemAddress>::const_iterator it = userList.begin(); it != userList.end(); ++it)
						{
							if (it->second == packet->systemAddress)
							{
								rs = it->first;
								userList.erase(it);
								break;
							}
						}

						printf("%s lost the connection.\n", rs.c_str());

						RakNet::BitStream bsOut;
						bsOut.Write((RakNet::MessageID)ID_REMOTE_DISCONNECTION_NOTIFICATION);
						bsOut.Write(rs.c_str());
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
					}
					break;
				case ID_SEND_IDENTIFICATION:
					{
						RakNet::RakString rs;
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rs);
						userList.insert(std::pair<std::string, RakNet::SystemAddress>(rs, packet->systemAddress));
						printf("%s has connected\n", rs.C_String());

						RakNet::BitStream bsOut;
						bsOut.Write((RakNet::MessageID)ID_REMOTE_NEW_INCOMING_CONNECTION);
						bsOut.Write(rs.C_String());
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
					}
					break;
				case ID_SEND_LIST:
				{
					std::stringstream formatMessage;
					for (std::map<std::string, RakNet::SystemAddress>::const_iterator it = userList.begin(); it != userList.end(); ++it)
					{
						formatMessage << it->first << "\n";
					}
					std::string sendMessage = formatMessage.str();
					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
					bsOut.Write(sendMessage.c_str());
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
				}
					break;
				case ID_GAME_MESSAGE_1:
					{
						RakNet::RakString id;
						RakNet::RakString rs;
						RakNet::Time rt;
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rt);

						messages.open("logs.txt", std::fstream::app);
						messages << ConvertTime(rt) << ": ";

						bsIn.Read(id);
						bsIn.Read(rs);
						std::stringstream formatMessage;
						formatMessage << "[" << id.C_String() << " to ALL]: " << rs.C_String();
						std::string sendMessage = formatMessage.str();

						printf(sendMessage.c_str());

						messages << sendMessage.c_str();
						messages << "\n";
						messages.close();

						RakNet::BitStream bsOut;
						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
						bsOut.Write(sendMessage.c_str());
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
					}
					break;
				case ID_GAME_MESSAGE_2:
					{
						RakNet::RakString uid; //user id
						RakNet::RakString sid; //send id
						RakNet::RakString rs;
						RakNet::Time rt;
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						
						bsIn.Read(rt);

						messages.open("logs.txt", std::fstream::app);
						messages << rt << ": ";

						bsIn.Read(uid);
						bsIn.Read(sid);
						bsIn.Read(rs);
						std::stringstream formatMessage;
						formatMessage << "[" << uid.C_String() << " to " << sid.C_String() << "]: " << rs.C_String();
						std::string sendMessage = formatMessage.str();

						printf(sendMessage.c_str());

						messages << sendMessage.c_str();
						
						if (userList.find(sid.C_String()) != userList.end()) 
						{
							RakNet::BitStream bsOut;
							bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
							bsOut.Write(sendMessage.c_str());
							peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, userList.find(sid.C_String())->second, false);
						}
						else
						{
							RakNet::BitStream bsOut;
							bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
							bsOut.Write("User not found...");
							peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
							messages << " <- Failed to send.";
						}

						messages << "\n";
						messages.close();
					}
					break;
				default:
					printf("Message with identifier %i has arrived.\n", packet->data[0]);
					break;
			}
		}
	}
}
