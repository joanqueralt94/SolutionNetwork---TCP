#pragma once
#include <SFML\Graphics.hpp>
#include <SFML/Network.hpp>

enum Packet_TYPE
{
	CHAT,
	GAME,
	EMPTY
};



class PlayerInfo
{
	std::string name;
	sf::Vector2i position;
	int lives;
public:
	PlayerInfo();
	~PlayerInfo();
};


//Overload send and recieve function for packet to recieve an enumerator
inline
sf::Packet& operator << (sf::Packet& packet, Packet_TYPE& CMD) { return packet << (int)CMD; }

inline
sf::Packet& operator >> (sf::Packet& packet, Packet_TYPE& CMD) {
	int id;
	auto info = packet >> id;
	CMD = (Packet_TYPE)id;
	return info;
}