// This file contains functions used at the system and main level.

#include <iostream>
#include <string>

#include "definitions.h"
#include "system_utilities.h"

using namespace std;

void wait() {
	// Prompt user for input and wait.
    string answer;

	cout << "Press ENTER to continue:";
    getline(cin, answer);
	return;
}

void printError(int errcode) {
    
    cout << "  ***ERROR:  ";
    switch(errcode) {
        case BAD_IP_ADDRESS:
            cout << "Bad IP address.\n";
            break;
        case UNKNOWN_MACHINE_TYPE:
            cout << "Unknown machine type.  No machine created.\n";
            break;
        case NETWORK_FULL:
            cout << "Network is full.  Computer was not added.\n";
            break;
        case NO_SUCH_MACHINE:
            cout << "Machine with this IP address not found.\n";
            break;
        case CONNECTION_REFUSED:
            cout << "The connection was refused.  The network is not changed\n";
            break;
        default:
            cout << "Unrecognized error code.\n";
    }
}

int parseCommandLine(string line, string tklist[]) {
    
    char token_terminator = ' ';
    string* accumulator = new string;
    *accumulator = "";
    int i = 0; //the index of the current character we're looking at
    int numTokens = 0; //the number of tokens we've collected so far
    
    if (line.length() > MAX_CMD_LINE_LENGTH)
    {
        return 0;
    }
    
    while (i < line.size()) { //while we have more characters to look at
        if (numTokens > MAX_TOKENS_ON_A_LINE)
        {
            return 0;
        }
        
        if (line[i] == ' ')
        {
            
            if (*accumulator != "" && token_terminator == ' ')
            {
                tklist[numTokens] = *accumulator;
                *accumulator = "";
                numTokens++;
            } else if (*accumulator != "")
            {
                *accumulator += " ";
            }
            i++; //move forward
        }
        else if (line[i] == '\"')
        {
            
            if (*accumulator != "")
            {
                tklist[numTokens] = *accumulator;
                *accumulator = "";
                numTokens++;
                token_terminator = ' ';
            }
            else
            {
                token_terminator = '\"';
            }
            i++;
        }
        else { //not a quote or space, so just add the char to accumulator
            *accumulator = *accumulator + line[i];
            i++;
        }
    }
    
    //the loop has finished, so if there's stuff left in the accumulator, add it
    if (*accumulator != "")
    {
        tklist[numTokens] = *accumulator;
        *accumulator = "";
        numTokens++;
    }
    
    delete accumulator;
        
    return numTokens;
}

void print_token_list(int num, string commands[])
{
    cout << "Number of tokens: " << num <<endl;
    for (int k = 0; k < num; k++)
    {
        cout << "token: " << commands[k] << "\t";
    }
    cout << endl;
}

command_element* system_commands[NUMBER_OF_COMMANDS];

void fillSystemCommandList() {
    system_commands[0] = new command_element;
    (*system_commands[0]).c	= "halt";
    (*system_commands[0]).cnum	= HALT;
    
    system_commands[1] = new command_element;
    (*system_commands[1]).c	= "system_status";
    (*system_commands[1]).cnum	= SYSTEM_STATUS;

    system_commands[2] = new command_element;
    (*system_commands[2]).c	= "create_machine";
    (*system_commands[2]).cnum	= CREATE_MACHINE;
    
    system_commands[3] = new command_element;
    (*system_commands[3]).c	= "destroy_machine";
    (*system_commands[3]).cnum	= DESTROY_MACHINE;
    
    system_commands[4] = new command_element;
    (*system_commands[4]).c	= "datagram";
    (*system_commands[4]).cnum	= DATAGRAM_CMD;
    
    system_commands[5] = new command_element;
    (*system_commands[5]).c	= "time_click";
    (*system_commands[5]).cnum	= TIME_CLICK;
    
    system_commands[6] = new command_element;
    (*system_commands[6]).c	= "connect";
    (*system_commands[6]).cnum	= CONNECT;
    
    system_commands[7] = new command_element;
    (*system_commands[7]).c	= "check_connections";
    (*system_commands[7]).cnum	= CHECK_CONNECTIONS;
    
    system_commands[8] = new command_element;
    (*system_commands[8]).c	= "consume_datagram";
    (*system_commands[8]).cnum	= CONSUME_DATAGRAM;
}

void freeSystemCommandList() {
    for (int i=0; i < NUMBER_OF_COMMANDS; i++)
    {
        delete system_commands[i];
    }
}

int getCommandNumber(string s) {
    for(int i=0; i<NUMBER_OF_COMMANDS; i++) {
        if (s.compare((*system_commands[i]).c) == 0)
            return (*system_commands[i]).cnum;
    }
    return UNDEFINED_COMMAND;
}


int convertStringToValue(string arg) {
    return stoi(arg);
}




