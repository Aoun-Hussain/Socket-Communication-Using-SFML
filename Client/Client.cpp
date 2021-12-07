/*
Author: Aoun Hussain
Class: ECE 6122-A
Last Date Modified: Sat, Nov 6, 2021
Description: This is the cpp client source file for the solution of problem lab 3.
             According to Professor Jeff Hurley's instruction, assuming a maximum
             message length of 1000 bytes. The client source file continuously
             prompts for new messages. Press the terminate button to end the
             program. Only connects to the mentioned ip address and port.
             Checks for all inavlid argument scenarios.

*/

/*
 References:

 https://www.sfml-dev.org/documentation/2.0/classsf_1_1TcpSocket.php#a7904ca6ab9e018021e305a3aeb7a1b9a
 https://en.sfml-dev.org/forums/index.php?topic=15382.0

*/

#include <iostream>
#include <cstdlib>
#include <SFML/Network.hpp>
using namespace std;

bool is_numeric(const string &strIn, unsigned long &nInputNumber)
{
    /*
    Checks if the argument is numeric and returns true/false accordingly
    checks for the port numbers
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

void runTcpClient(unsigned long port, const string &server)
{
    /*
     function to prompt the user for messages and connects to
     the server and sends messages to the server
    */

    while (server == sf::IpAddress::None);

    // Create a socket for communicating with the server
    sf::TcpSocket socket;
    // Connect to the server
    if (socket.connect(server, port) != sf::Socket::Done)
    {
        if (socket.connect(server, port) == 4)
        {
            cout << "Failed to connect to the server at " << server << " on " << port << endl;
            cout << "Please check your values and press Enter to end program!" << endl;
            return;
        }
        else
        {
            return;
        }
    }

    // Send a message to the server
    while (true)
    {
        string msg;
        cout << "Please enter a message: "; // Type a message and press enter
        std::getline(std::cin, msg); // Get user input from the keyboard
        if (socket.send(&msg, 1000) != sf::Socket::Done) //sending the message 1000 bytes long
            return;
    }
}

int main(int argc, char* argv[])
{
    /*
    main function to check for all invalid combination of arguments
     and connects the client to the given port number and ip address
    */

    if (argc == 3)
    {
        unsigned long port{ 0 };
        string strInput1(argv[1]);
        string strInput2(argv[2]);
        bool bIsValid = is_numeric(strInput2, port);
        if ((bIsValid) & (!strInput1.empty()) & (port <= 65535) & (port >= 61000))
        {
            runTcpClient(port, strInput1);
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
