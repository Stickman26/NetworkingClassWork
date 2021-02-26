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
#include "gpro-net/MineLab.h"
#include "gpro-net/GameMessages.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <sstream>

#include "RakNet/RakPeerInterface.h"
#include "RakNet/GetTime.h"

#define MAX_CLIENTS 10
#define SERVER_PORT 4024

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
	timestamp << "[" << hours << ":" << minutes << ":" << seconds << "]";
	return timestamp.str();
}

struct GameRoom
{
	std::string RoomName;
	int MaxPlayers;
	std::map<std::string, RakNet::SystemAddress> Players;
	std::map<std::string, RakNet::SystemAddress> Spectators;
};

int main(int const argc, char const* const argv[])
{

	std::ofstream messages;

	//Add a map for users and their IPs
	std::map<std::string, RakNet::SystemAddress> userList;
	
	//room list for storing room ID
	std::vector<GameRoom> roomList;

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
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, true);
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
						//RakNet::RakString id;
						//RakNet::RakString rs;
						std::string id;
						std::string rs;
						TextMessage* txtObj = new TextMessage(std::string("a"), std::string("a"), std::string("a"));
						RakNet::Time rt;
						RakNet::BitStream bsIn(packet->data, packet->length, false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						bsIn.Read(rt);
						//bsIn.Read(id);
						//bsIn.Read(rs);
						bsIn >> txtObj->myMessage;
						id = txtObj->myMessage.UserName;
						rs = txtObj->myMessage.Message;

						std::map<std::string, RakNet::SystemAddress>::const_iterator userToFind;
						std::vector<GameRoom>::const_iterator roomUserIsIn;
						bool isPlayer = false;
						bool isInRoom = false;

						//find the user in the userlist
						for (std::map<std::string, RakNet::SystemAddress>::const_iterator it = userList.begin(); it != userList.end(); ++it)
						{
							if(it->first == id)
							{
								userToFind = it;
								break;
							}
						}

						//find the room the specified user is in by checking every room
						for (std::vector<GameRoom>::const_iterator roomBeingChecked = roomList.begin(); roomBeingChecked != roomList.end(); ++roomBeingChecked)
						{
							for (std::map<std::string, RakNet::SystemAddress>::const_iterator it2 = roomBeingChecked->Players.begin(); it2 != roomBeingChecked->Players.end(); ++it2)
							{
								if (it2->first == id)
								{
									roomUserIsIn = roomBeingChecked;
									isPlayer = true;
									isInRoom = true;
									break;
								}
							}

							for (std::map<std::string, RakNet::SystemAddress>::const_iterator it2 = roomBeingChecked->Spectators.begin(); it2 != roomBeingChecked->Spectators.end(); ++it2)
							{
								if (it2->first == id)
								{
									roomUserIsIn = roomBeingChecked;
									isPlayer = false;
									isInRoom = true;
									break;
								}
							}
						}

						if (isInRoom)
						{
							if (isPlayer)
							{
								//format user message
								std::stringstream formatMessage;
								formatMessage << "\n[" << id << " to ALL]: " << rs;
								std::string sendMessage = formatMessage.str();

								printf(sendMessage.c_str());

								std::map<std::string, RakNet::SystemAddress>::const_iterator it;
								RakNet::BitStream bsOut;
								bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
								bsOut.Write(sendMessage);
								//Send to Players
								for (it = roomUserIsIn->Players.begin(); it != roomUserIsIn->Players.end(); ++it)
								{
									peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, it->second, true);
								}
								//Send to Spectators
								for (it = roomUserIsIn->Spectators.begin(); it != roomUserIsIn->Spectators.end(); ++it)
								{
									peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, it->second, true);
								}
							}
							else
							{
								//format user message
								std::stringstream formatMessage;
								formatMessage << "\n[" << id << " to Spectators]: " << rs;
								std::string sendMessage = formatMessage.str();

								printf(sendMessage.c_str());

								std::map<std::string, RakNet::SystemAddress>::const_iterator it;
								//send message to spectators
								RakNet::BitStream bsOut;
								bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
								bsOut.Write(sendMessage);
								for (it = roomUserIsIn->Spectators.begin(); it != roomUserIsIn->Spectators.end(); ++it)
								{
									peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, it->second, true);
								}
							}
						}
						else 
						{
							//format user message
							std::stringstream formatMessage;
							formatMessage << "\n[" << id << " to ALL]: " << rs;
							std::string sendMessage = formatMessage.str();

							printf(sendMessage.c_str());
							printf("\n");

							RakNet::BitStream bsOut;
							bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
							bsOut.Write(sendMessage.c_str());
							peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
						}

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
						messages << ConvertTime(rt) << ": ";

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
				case ID_CREATE_ROOM:
				{
					RakNet::RakString id;
					RakNet::RakString rs;
					RakNet::Time rt;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					//read in time stamp
					bsIn.Read(rt);
					//read in user ID
					bsIn.Read(id);
					//read in room ID that user is trying to create
					bsIn.Read(rs);

					std::stringstream formatMessage;
					std::string sendMessage;
					RakNet::BitStream bsOut;

					std::string roomName = rs.C_String();
					std::vector<GameRoom>::iterator it = find_if(roomList.begin(), roomList.end(), [roomName](const GameRoom& i) { return i.RoomName == roomName; });

					//see if a room w/ specified ID exists
					if (it != roomList.end()) 
					{
						//if it does
						formatMessage << "You have attempted to create a room w/ ID " << rs.C_String() << " but that room already exists!";
						sendMessage = formatMessage.str();
						printf(sendMessage.c_str());

						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
						bsOut.Write(sendMessage.c_str());
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
					}
					else 
					{
						//if it doesn't exist add to list of rooms
						GameRoom newRoom;
						newRoom.MaxPlayers = 8;
						newRoom.RoomName = rs.C_String();

						roomList.push_back(newRoom);
						printf("%s has created room w/ ID: %s", id.C_String(), rs.C_String());

						formatMessage << "[" << id.C_String() << " has created Room " << rs.C_String() << "]";
						sendMessage = formatMessage.str();

						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
						bsOut.Write(sendMessage.c_str());
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
					}
				}
				break;
				case ID_JOIN_ROOM:
				{
					RakNet::RakString id;
					RakNet::RakString rs;
					RakNet::RakString playerOrSpectator;
					RakNet::Time rt;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					//read in time stamp
					bsIn.Read(rt);
					//read in user ID
					bsIn.Read(id);
					//read in room ID that user is trying to join
					bsIn.Read(rs);
					//read in if the person should be a player or spectator
					bsIn.Read(playerOrSpectator);

					std::stringstream formatMessage;
					std::string sendMessage;
					RakNet::BitStream bsOut;

					//check if specified room exists and if it does, join it
					std::string roomName = rs.C_String();
					std::vector<GameRoom>::iterator it = find_if(roomList.begin(), roomList.end(), [roomName](const GameRoom& i) { return i.RoomName == roomName; });

					if (it != roomList.end())
					{
						printf("\n%s has joined room w/ ID: %s", id.C_String(), rs.C_String());

						if(playerOrSpectator == "p")
						{
							formatMessage << "Player " << id.C_String() << " joined room " << rs.C_String() << "!\n";
							sendMessage = formatMessage.str();
							printf(sendMessage.c_str());

							//actually join the room as a player
							it->Players.insert(std::pair<std::string, RakNet::SystemAddress>(id, packet->systemAddress));

							//this message should be sent to everyone to let players and spectators know that a player has joined
							bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
							bsOut.Write(sendMessage.c_str());
							peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, true);
						}
						else if (playerOrSpectator == "s")
						{
							formatMessage << "Spectator " << id.C_String() << " joined room " << rs.C_String() << "!\n";
							sendMessage = formatMessage.str();
							printf(sendMessage.c_str());

							//actually join the room as a spectator
							it->Spectators.insert(std::pair<std::string, RakNet::SystemAddress>(id, packet->systemAddress));

							//this message should be sent to everyone to let players and spectators know that a spectator has joined
							bsOut.Write((RakNet::MessageID)ID_MESSAGE_SPECTATORS);
							bsOut.Write(sendMessage.c_str());
							bsOut.Write(it);
							peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, true);
						}
						
					}
					else 
					{
						printf("%s attempted to join room %s\n", id.C_String(), rs.C_String());
						formatMessage << "Room " << rs.C_String() << " does not exist!\n";
						sendMessage = formatMessage.str();

						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
						bsOut.Write(sendMessage.c_str());
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
					}
				}
				break;
				case ID_MESSAGE_SPECTATORS:
				{
					RakNet::RakString messageToSend;
					GameRoom roomToSend;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(messageToSend);
					bsIn.Read(roomToSend);

					std::map<std::string, RakNet::SystemAddress>::const_iterator it;
					//send message to spectators
					for(it = roomToSend.Spectators.begin(); it != roomToSend.Spectators.end(); ++it)
					{
						RakNet::BitStream bsOut;
						bsOut.Write((RakNet::MessageID)ID_MESSAGE_SPECTATORS);
						bsOut.Write(messageToSend);
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, it->second, true);
					}
				}
				break;
				case ID_STRUCT_TEST:
				{
					TextMessage* testing;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(testing);

					printf("Testing our struct: %s", testing->myMessage.Message.c_str());
				}
				break;
				default:
					printf("Message with identifier %i has arrived.\n", packet->data[0]);
					break;
			}
		}
	}
}
