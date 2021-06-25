#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include "../GameLib/PlayerInfo.h"
#include <string>
#include <cstring>
#include <iostream>
#include <thread>
#include <list>
using namespace sf;
using namespace std;

#define SERVER_PORT 50000
#define NUM_PLAYERS 6

struct Directions
{
	string IP;
	unsigned short PORT;
	int torn;
	bool myturn = false;

};

int main()
{

	cout << "-------------- SERVER CHAT -------------" << endl;
	cout << "Waiting for users to connect the chat..." << endl;


	int seed = std::chrono::system_clock::now().time_since_epoch().count();

	TcpListener listener;
	listener.listen(SERVER_PORT);
	vector<Directions> aPeers;

	for (int i = 0; i <= NUM_PLAYERS; i++)
	{
		TcpSocket socket;
		if (listener.accept(socket) != Socket::Done)
		{
			cout << "Cannot conect the peer" << endl;
		}
		else
		{
			Packet pack;

			
			int size = aPeers.size();

			pack << seed << size;
			socket.send(pack);

			

			if (i != 0)
			{
				aPeers[i].torn = i;

				for (int j = 0; j < aPeers.size(); j++)
				{
					pack.clear();
					pack << aPeers[j].IP << aPeers[j].PORT << aPeers[i].torn;
					socket.send(pack);
				}

			}

		}

		Directions risis;
		risis.IP = socket.getRemoteAddress().toString();
		risis.PORT = socket.getRemotePort();
		aPeers.push_back(risis);
				
		cout << "Number of connections: " << aPeers.size() << endl;
		
		socket.disconnect();

	}

	listener.close();

	return 0;
}







