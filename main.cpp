#include <iostream>
#include <string>
#include <fstream>

#include "datagram.h"
#include "system_utilities.h"
#include "definitions.h"
#include "machines.h"


using namespace std;


ifstream* inp = new ifstream;
node* network[MAX_MACHINES];

int main() {
    string* cmd_line = new string;
    string* parsed_command = new string[MAX_TOKENS_ON_A_LINE];
    int  number_of_tokens;
    int  done;
    int cmd;
    int ec1, ec2, error_code;
    
    IPAddress ip1, ip2, ip3;
    string name = "";
    string type = "";
    int i, j, k, t1, t2, k1, k2;
    
    datagram* d2 = NULL;
    
    for (int i = 0; i < MAX_MACHINES; i++)
        network[i] = NULL;
    
    // Determine and set up source of input.
    if(COMMANDS_FROM_FILE) {
        (*inp).open("p8input.txt", ios::in);
        if(!(*inp)) {
            cout << "***Could not open file.";
            return 0;
        }
    }
    
    fillSystemCommandList();
    
    done = 0;
    
    do {
        error_code = 0;
        
        //Get command from input and parse into arg0 and argv
        if(COMMANDS_FROM_FILE)
            getline(*inp, *cmd_line);
        else {
            cout << ">";           // command line prompt
            getline(cin, *cmd_line);//, MAX_CMD_LINE_LENGTH);
        }
        number_of_tokens = parseCommandLine(*cmd_line, parsed_command);
        if(ECHO_COMMAND) print_token_list(number_of_tokens, parsed_command);
        
        cmd = getCommandNumber(parsed_command[0]);
        switch(cmd) {
            case HALT:
                cout << "** Command HALT recognized" << endl;
                done = 1;
                break;
            case SYSTEM_STATUS:
                cout << "** Command SYSTEM_STATUS recognized" << endl;
                if (number_of_tokens == 1)
                {
                    cout << "Nodes in the network:" << endl;
                    for(i=0; i<MAX_MACHINES; i++) {
                        if(network[i]!=NULL)
                        {
                            network[i]->display();
                            cout << endl;
                        }
                    }
                    
                }
                else
                {
                    cout << "Selected nodes in the network:" << endl;
                    j = 1;
                    for (i = 0; i < MAX_MACHINES && j < number_of_tokens; i++)
                    {
                        IPAddress ip_dis;
                        ip_dis.parse(parsed_command[j]);
                        if (network[i] != NULL)
                        {
                            if (network[i]->amIThisComputer(ip_dis))
                            {
                                network[i]->display();
                                j++;
                                cout << endl;
                            }
                        }
                        
                    }
                }
                cout << "End of network." << endl <<endl;
                break;
            case CREATE_MACHINE:
                cout << "** Command CREATE_MACHINE recognized" << endl;
                type = parsed_command[1];
                name = parsed_command[2];
                error_code = ip3.parse(parsed_command[3]);
                for (i = 0; i < MAX_MACHINES; i++)
                {
                    if (network[i] == NULL)
                        break;
                }
                
                if (i >= MAX_MACHINES) {
                    error_code = NETWORK_FULL;
                    break;
                }
                
                if (error_code == 0)
                {
                    if (parsed_command[1] == "laptop")
                        network[i] = new laptop(name, ip3);
                    else if (parsed_command[1] == "server")
                        network[i] = new server(name, ip3);
                    else if (parsed_command[1] == "wan")
                        network[i] = new WAN(name, ip3);
                    else
                        error_code = UNKNOWN_MACHINE_TYPE;
                    
                    if (error_code == 0)
                        network[i]->display();
                    
                    cout << endl;
                }
                break;
            case DESTROY_MACHINE:
                cout << "** Command DESTROY_MACHINE recognized" << endl;
                error_code = ip1.parse(parsed_command[1]);
                if(error_code!=0) break;
                for(i=0; i<MAX_MACHINES; i++) {
                    if(network[i]==NULL) continue;
                    if(network[i]->amIThisComputer(ip1)) {
                  
                        if (network[i]->myType() == LAPTOP)
                            delete (laptop*)network[i];
                        else if (network[i]->myType() == SERVER)
                            delete (server*)network[i];
                        else if (network[i]->myType() == WAN_MACHINE)
                            delete (WAN*)network[i];
                        
                        network[i] = NULL;
                        cout << "Computer removed from the network." << endl;
                        break;
                    }
                }
                if(i>=MAX_MACHINES) error_code = NO_SUCH_MACHINE;
                break;
            case CONNECT:
                cout << "** Command CONNECT recognized" << endl;
                ec1 = ip1.parse(parsed_command[1]);
                ec2 = ip2.parse(parsed_command[2]);
                if ((ec1 != 0) || (ec2 != 0))
                {
                    error_code = BAD_IP_ADDRESS;
                    break;
                }
                j = -1;
                k = -1;
                for(i = 0; i <MAX_MACHINES; i++)
                {
                    if(network[i]==NULL) continue;
                    if(network[i]->amIThisComputer(ip1)) {
                        j = i; //j is index of machine1
                    } else if(network[i]->amIThisComputer(ip2)){
                        k = i; //k is index of machine2
                    }
                }
                if ((j == -1) || (k == -1))
                {
                    error_code = NO_SUCH_MACHINE;
                    break;
                }
                t1 = network[j]->myType();
                t2 = network[k]->myType();
                k1 = network[j]->canAcceptConnection(t2);
                k2 = network[k]->canAcceptConnection(t1);
                if((k1==0)||(k2==0)) {
                    error_code = CONNECTION_REFUSED;
                    break;
                }
                network[j]->connect(ip2,t2);
                network[k]->connect(ip1,t1);
                break;
            case CHECK_CONNECTIONS:
                cout << "** Command CHECK_CONNECTIONS recognized" << endl;
                break;
            case DATAGRAM_CMD:
                cout << "** Command DATAGRAM recognized" << endl;
                error_code = ip1.parse(parsed_command[1]);
                if(error_code!=0) break;
                for(i=0; i<MAX_MACHINES; i++) {
                    if(network[i]==NULL) continue;
                    if(network[i]->amIThisComputer(ip1)) {
                        break;
                    }
                }
                if(i>=MAX_MACHINES) {
                    error_code = NO_SUCH_MACHINE;
                    break;
                }
                ec1 = ip1.parse(parsed_command[1]);
                ec2 = ip2.parse(parsed_command[2]);
                if((ec1!=0) || (ec2!=0)) error_code = BAD_IP_ADDRESS;
                else {
                    d2 = new datagram;
                    d2->makeDatagram(ip1, ip2, parsed_command[3]);
                    d2->display();
                    ((laptop*)network[i])->initiateDatagram(d2);
                    cout << endl;
                }
                break;
            case CONSUME_DATAGRAM:
                cout << "** Command CONSUME_DATAGRAM recognized" << endl;
                IPAddress ip_temp;
                ip_temp.parse(parsed_command[1]);
                
                for (i = 0; i < MAX_MACHINES; i++)
                {
                    if (network[i] != NULL)
                    {
                        if (network[i]->amIThisComputer(ip_temp))
                        {
                            ((laptop*)network[i])->consumeDatagram();
                        }
                    }
                }
                break;
            case TIME_CLICK:
                cout << "** Command TIME_CLICK recognized" << endl;
                for (i = 0; i < MAX_MACHINES; i++)
                {
                    if (network[i] != NULL)
                    {
                        if (network[i]->myType() == LAPTOP)
                        {
                            ((laptop*)network[i])->transferDatagram();
                        }
                        
                        if (network[i]->myType() == SERVER)
                        {
                            ((server*)network[i])->transferDatagram();
                        }
                        
                        if (network[i]->myType() == WAN_MACHINE)
                        {
                            ((WAN*)network[i])->transferDatagram();
                        }
                    }
                }
                break;
            case UNDEFINED_COMMAND:
                cout << "***ERROR:  Unrecognized command" << endl;
                break;
        }
        
        if(error_code!=0) printError(error_code);
        
        if(COMMANDS_FROM_FILE) wait();
        
        cout << endl;
    }
    while(!done);
    
    cout << "\n\n";
    
    freeSystemCommandList(); //deletes the list of command elements
    delete[] parsed_command;
    delete cmd_line;
    delete inp;
    
    //the following deletes any machines that were not "destroyed"
    for (int i = 0; i < MAX_MACHINES; i++)
    {
        if (network[i] != NULL)
        {
            if (network[i]->myType() == LAPTOP)
                delete (laptop*)network[i];
            else if (network[i]->myType() == SERVER)
                delete (server*)network[i];
            else if (network[i]->myType() == WAN_MACHINE)
                delete (WAN*)network[i];
        }
    }
}

