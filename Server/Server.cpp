/*
 References:

 https://www.sfml-dev.org/documentation/2.0/classsf_1_1TcpSocket.php#a7904ca6ab9e018021e305a3aeb7a1b9a
 https://en.sfml-dev.org/forums/index.php?topic=15382.0

*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <SFML/Network.hpp>
#include "unistd.h"
#include <list>
using namespace std;

bool is_numeric(const string &strIn, unsigned long &nInputNumber)
{
    /*
    Checks if the argument is numeric and returns true/false accordingly.
     Used for checking if port number is a valid integer
    */

    bool bRC = all_of(strIn.begin(), strIn.end(), [](unsigned char c)
    {
        return ::isdigit(c);                      // http://www.cplusplus.com/reference/algorithm/all_of/
    }                                             // https://www.geeksforgeeks.org/lambda-expression-in-c/
    );                                            // http://www.cplusplus.com/reference/cctype/isdigit/

    if (bRC)
    {
        nInputNumber = stoul(strIn);              // https://www.cplusplus.com/reference/string/stoul/
        return true;
    }
    else
    {
        return false;
    }

}

void writeFile(const string& arg)
{
    /*
    function to check the existence of the file
     and decide whether to append or create a new
     one. Writes the given argument in the file
     separated by a new line character.
    */

    if(access("server.log", F_OK ) == 0)
    {
        // file exists
        std::ofstream myfile;

        myfile.open("server.log", std::ios_base::app); // append instead of overwrite
        if (myfile.is_open())
        {
            myfile << arg << "\n";
            myfile.close();
        }
        else
        {
            cout << "Unable to open file";
        }
    }
    else
    {
        // file doesn't exist
        ofstream myfile ("server.log");
        if (myfile.is_open())
        {
            myfile << arg << "\n";
            myfile.close();
        }
        else
        {
            cout << "Unable to open file";
        }
    }
}

void runTcpServer(unsigned short port)
{
    /*
    function that listens to unlimited clients on the given port
    and logs in their messages as well their connects
    and disconnects in the server.log file
    */

    // Create a list to store the future clients
    std::list<sf::TcpSocket*> clients;

    // Create a socket to listen to new connections
    sf::TcpListener listener;
    listener.listen(port);

    // Create a selector
    sf::SocketSelector selector;

    // Add the listener to the selector
    selector.add(listener);

    // Endless loop that waits for new connections
    while (true)
    {
        // Make the selector wait for data on any socket
        if (selector.wait())
        {
            // Test the listener
            if (selector.isReady(listener))
            {
                // The listener is ready: a pending connection
                sf::TcpSocket* client = new sf::TcpSocket;
                client->setBlocking(false);

                if (listener.accept(*client) == sf::Socket::Done)
                {
                    // Add the new client to the clients list
                    clients.push_back(client);
                    // Add the new client to the selector so that we will
                    // be notified when he sends something
                    selector.add(*client);

                    //writes output to the log file
                    writeFile("Client connected");
                }
                else
                {
                    // Error, we won't get a new connection, delete the socket
                    delete client;
                }
            }
            else
            {
                // The listener socket is not ready, test all other sockets (the clients)
                for (std::list<sf::TcpSocket*>::iterator it = clients.begin(); it != clients.end(); ++it)
                {
                    sf::TcpSocket& client = **it;
                    char in[1000];      //receiving the message 1000 bytes long
                    unsigned long received;
                    if (selector.isReady(client))
                    {
                        // Receive a message from the client
                        if (client.receive(in, sizeof(in), received) == sf::Socket::Disconnected)
                        {
                            //check if the client disconnected
                            clients.remove(*it);
                            selector.remove(client);
                            delete *it;
                            //writes output to the file
                            writeFile("Client disconnected");
                            break;
                        }

                        //writes output to the file
                        writeFile(in);
                    }
                }
            }
        }
    }
}

int main(int argc, char* argv[])
{
    /*
    main function to call all sub functions and check for invalid arguments
    */

    if (argc == 2)      //only takes a valid port number as a command line argument
    {
        unsigned long port{ 0 };
        string strInput(argv[1]);
        bool bIsValid = is_numeric(strInput, port);
        if ((bIsValid) & (port <= 65535) & (port >= 61000))
        {
            //calls this endless function
            runTcpServer(port);
        }
        else
        {
            for(int i = 1; i < argc; i++)
            {
                cout << "Invalid command line argument detected: " << argv[i] << endl;
            }
            cout << "Please check your values and press Enter to end the program!" << endl;
        }
    }
    else
    {
        for(int i = 1; i < argc; i++)
        {
            cout << "Invalid command line argument detected: " << argv[i] << endl;
        }
        cout << "Please check your values and press Enter to end the program!" << endl;
    }

    // Wait until the user presses 'enter' key
    std::cin.ignore(10000, '\n');
    return EXIT_SUCCESS;
}
