#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <iostream>
int main()
{
    sf::TcpListener listener;
    listener.listen(25565);
    while (1)
    {
        sf::TcpSocket client;
        if (listener.accept(client) == sf::Socket::Done)
        {
            // A new client just connected!
            std::cout << "New connection received from " << client.getRemoteAddress() << std::endl;
        }
    }
}
