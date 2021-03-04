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

/*
Code implemented by:
Lansingh Freeman
Jason Gold
*/

#include "gpro-net/gpro-net.h"
#include "gpro-net/GameMessages.h"
#include "gpro-net/blackjack/BlackJack.h"

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
	BlackJack RoomSession;
	std::string RoomName;
	bool GameInProgess = false;
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
					//user connecting
						printf("A connection is incoming.\n");
					}
					break;
				case ID_DISCONNECTION_NOTIFICATION:
					{
					//checks for when a user disconnects and removes them from any room they were in
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

						//find the room the specified user is in by checking every room
						for (std::vector<GameRoom>::iterator roomBeingChecked = roomList.begin(); roomBeingChecked != roomList.end(); ++roomBeingChecked)
						{
							for (std::map<std::string, RakNet::SystemAddress>::iterator it2 = roomBeingChecked->Players.begin(); it2 != roomBeingChecked->Players.end(); ++it2)
							{
								if (it2->first == rs)
								{
									printf("Player %s removed from room %s\n", rs.c_str(), roomBeingChecked->RoomName.c_str());
									roomBeingChecked->Players.erase(it2);
									break;
								}
							}

							for (std::map<std::string, RakNet::SystemAddress>::iterator it2 = roomBeingChecked->Spectators.begin(); it2 != roomBeingChecked->Spectators.end(); ++it2)
							{
								if (it2->first == rs)
								{
									printf("Spectator %s removed from room %s\n", rs.c_str(), roomBeingChecked->RoomName.c_str());
									roomBeingChecked->Spectators.erase(it2);
									break;
								}
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
					//checks for when a user loses connection and removes them from the room they were in
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

						//find the room the specified user is in by checking every room
						for (std::vector<GameRoom>::iterator roomBeingChecked = roomList.begin(); roomBeingChecked != roomList.end(); ++roomBeingChecked)
						{
							for (std::map<std::string, RakNet::SystemAddress>::iterator it2 = roomBeingChecked->Players.begin(); it2 != roomBeingChecked->Players.end(); ++it2)
							{
								if (it2->first == rs)
								{
									printf("Player %s removed from room %s\n", rs.c_str(), roomBeingChecked->RoomName.c_str());
									roomBeingChecked->Players.erase(it2);
									break;
								}
							}

							for (std::map<std::string, RakNet::SystemAddress>::iterator it2 = roomBeingChecked->Spectators.begin(); it2 != roomBeingChecked->Spectators.end(); ++it2)
							{
								if (it2->first == rs)
								{
									printf("Spectator %s removed from room %s\n", rs.c_str(), roomBeingChecked->RoomName.c_str());
									roomBeingChecked->Spectators.erase(it2);
									break;
								}
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
					//lets everyone know a user has connected
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
					//sends a list of all connected users back to the sender
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
						TextMessage* txtObj = new TextMessage(std::string("a"), std::string("a"));
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

						//make sure user is in a room
						if (isInRoom)
						{
							//check if the user is a player or spectator
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
								bsOut.Write(sendMessage.c_str());
								//Send to Players
								for (it = roomUserIsIn->Players.begin(); it != roomUserIsIn->Players.end(); ++it)
								{
									peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, it->second, false);
								}
								//Send to Spectators
								for (it = roomUserIsIn->Spectators.begin(); it != roomUserIsIn->Spectators.end(); ++it)
								{
									peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, it->second, false);
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
								bsOut.Write(sendMessage.c_str());
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
					//old message system, still used for DMs rather than our new system (if it ain't broke don't fix it)
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
						//printf("\n%s has joined room w/ ID: %s\n", id.C_String(), rs.C_String());
						//check is user is player or spectator and send the message accordingly
						if(playerOrSpectator == "p")
						{
							formatMessage << "Player " << id.C_String() << " joined room " << rs.C_String() << "!\n";
							sendMessage = formatMessage.str();
							printf(sendMessage.c_str());

							//actually join the room as a player
							it->Players.insert(std::pair<std::string, RakNet::SystemAddress>(id, packet->systemAddress));

							//this message should be sent to everyone to let players and spectators know that a player has joined
							bsOut.Reset();
							bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
							bsOut.Write(sendMessage.c_str());
							peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
						}
						else if (playerOrSpectator == "s")
						{
							formatMessage << "Spectator " << id.C_String() << " joined room " << rs.C_String() << "!\n";
							sendMessage = formatMessage.str();
							printf(sendMessage.c_str());

							//actually join the room as a spectator
							it->Spectators.insert(std::pair<std::string, RakNet::SystemAddress>(id, packet->systemAddress));

							//this message should be sent to everyone to let players and spectators know that a spectator has joined
							bsOut.Reset();
							bsOut.Write((RakNet::MessageID)ID_MESSAGE_SPECTATORS);
							bsOut.Write(sendMessage.c_str());
							peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
						}

						//message the user that just joined to let them know the room information
						bsOut.Reset();
						bsOut.Write((RakNet::MessageID)ID_MESSAGE_JOINER);
						bsOut.Write(1);
						bsOut.Write(it->RoomName.c_str());
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
					}
					else 
					{
						printf("%s attempted to join room %s\n", id.C_String(), rs.C_String());
						formatMessage << "Room " << rs.C_String() << " does not exist!\n";
						sendMessage = formatMessage.str();

						bsOut.Reset();
						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
						bsOut.Write(sendMessage.c_str());
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
					}
				}
				break;
				case ID_MESSAGE_SPECTATORS:
				{
					//send a message to the spectators
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
				case ID_PLAYER_MOVE:
				{
					//read in the player's move via custom struct
					BlackJackMoveMessage* playerMove;
					RakNet::RakString rs;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					playerMove = new BlackJackMoveMessage("", BlackJackMoves::Hit);
					bsIn >> playerMove->myMove;
					bsIn.Read(rs);
					std::string roomString = rs.C_String();

					//find player and do the thing
					std::vector<GameRoom>::iterator it = std::find_if(roomList.begin(), roomList.end(), [roomString](const GameRoom& myRoom) {return myRoom.RoomName == roomString; });

					//make sure the player's room exists
					if (it != roomList.end())
					{
						std::string outputMessage = "";
						RakNet::BitStream bsOut;

						if(!it->RoomSession.gameEnded() && playerMove->myMove.userName == it->RoomSession.getCurrentPlayerName())
						{
							//actually do the player's move
							switch (playerMove->myMove.move)
							{
							case BlackJackMoves::Hit:
								it->RoomSession.currentPlayerHit();
								outputMessage += it->RoomSession.getCurrentPlayerName() + " Hit\n Current Standing: \n";
								outputMessage += it->RoomSession.displayGameState().c_str();

								//check if the player busted
								if(it->RoomSession.currentHandCheck())
								{
									//check if the player has 21
									if(it->RoomSession.currentHandBlackJackCheck())
									{
										outputMessage += it->RoomSession.getCurrentPlayerName() + " has 21! Moving to next turn\n\n";
										it->RoomSession.currentPlayerStand();
										outputMessage += it->RoomSession.displayGameState();
									}
								}
								else
								{
									outputMessage += it->RoomSession.getCurrentPlayerName() + " has busted! Moving to next turn\n\n";
									//ends the player's turn
									it->RoomSession.currentPlayerStand();
									outputMessage += it->RoomSession.displayGameState();
								}

								break;
							case BlackJackMoves::Stand:
								outputMessage += it->RoomSession.getCurrentPlayerName() + " stands! Moving to next turn\n\n";
								//ends the player's turn
								it->RoomSession.currentPlayerStand();
								outputMessage += it->RoomSession.displayGameState();
								break;
							}

							printf(outputMessage.c_str());

							if (it->RoomSession.gameEnded())
							{
								it->GameInProgess = false;
							}

							//send the current gamestate to all members of the lobby
							bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
							bsOut.Write(outputMessage.c_str());
							peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
						}
						else
						{
							//Yell at the player for being impatient
							RakNet::RakString messageToSend = "Please wait until your turn!";
							bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
							bsOut.Write(messageToSend.C_String());
							peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
						}
					}
				}
				break;
				case ID_START_GAME:
				{
					//starts the game and sends the current player's hand and the dealer's hand to all players
					RakNet::RakString rs;
					RakNet::BitStream bsIn(packet->data, packet->length, false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read(rs);
					std::string roomString = rs.C_String();
					std::vector<GameRoom>::iterator it = std::find_if(roomList.begin(), roomList.end(), [roomString](const GameRoom& myRoom) {return myRoom.RoomName == roomString; });

					if(it != roomList.end())
					{
						if (it->GameInProgess)
						{
							break;
						}
						it->GameInProgess = true;

						RakNet::BitStream bsOut;
						std::vector<std::string> names;

						for(std::map<std::string, RakNet::SystemAddress>::iterator it2 = it->Players.begin(); it2 != it->Players.end(); ++it2)
						{
							names.push_back(it2->first);
						}

						it->RoomSession.resetBlackJackGame(names);

						//bsOut.Reset();
						bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
						RakNet::RakString sendMe = it->RoomSession.displayGameState().c_str();
						printf(sendMe.C_String());
						bsOut.Write(sendMe.C_String());
						peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
					}
				}
				break;
				case ID_ROOM_LIST:
				{
					//returns a list of all the available game rooms a player can join
					std::stringstream formatMessage;
					formatMessage << "---Open Rooms---\n";
					for (std::vector<GameRoom>::iterator it = roomList.begin(); it != roomList.end(); ++it)
					{
						formatMessage << "Room ID: " << it->RoomName << "\n";
					}
					std::string sendMessage = formatMessage.str();
					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
					bsOut.Write(sendMessage.c_str());
					peer->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 1, packet->systemAddress, false);
				}
				break;
				default:
					printf("Message with identifier %i has arrived.\n", packet->data[0]);
					break;
			}
		}
	}
}
