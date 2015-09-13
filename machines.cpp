#include <iostream>
#include <stdlib.h>
#include <string>

#include "machines.h"

// node class functions

node::node(string n, IPAddress a)
{
    name = new string;
    *name = n;
    my_address = a;
}

node::~node()
{
    delete name;
}

void node::display()
{
    
    cout << "   Name: " << *name << "   IP address: ";
    my_address.display();
}

int node::amIThisComputer(IPAddress a)
{
    if(my_address.sameAddress(a))
        return 1;
    else
        return 0;
}

int node::myType()
{
    return 0;
}

IPAddress node::myAddress()
{
    return my_address;
}


// laptop class functions

laptop::laptop(string n, IPAddress a) : node(n,a)
{
    incoming = outgoing = NULL;
    my_server.parse("0.0.0.0");
}

int laptop::myType() {
    return LAPTOP;
}

void laptop::initiateDatagram(datagram* d)
{
    outgoing = d;
}

void laptop::receiveDatagram(datagram* d)
{
    incoming = d;
}

int  laptop::canAcceptConnection(int machine_type)
{
    if(machine_type!=SERVER) return 0;
    return my_server.isNULL();
}

void laptop::connect(IPAddress x, int machine_type)
{
    if(machine_type==SERVER) my_server = x;
}

void laptop::transferDatagram()
{
    int i;
    extern node* network[MAX_MACHINES];
    
    if(outgoing==NULL) return;
    if(my_server.isNULL()) return;
    for (i = 0; i < MAX_MACHINES; i++)
    {
        if (network[i] != NULL)
        {
            if (network[i]->amIThisComputer(my_server))
                break;
        }
    }
    network[i]->receiveDatagram(outgoing);
    outgoing = NULL;
}

void laptop::display()
{
    
    cout << "Laptop computer:  ";
    node::display();
    
    if(my_server.isNULL()) {
        cout << "\n    Not connected to any server.\n";
    }
    else {
        cout << "\nConnected to ";
        my_server.display();
        cout << "\n";
    }
    
    cout << "\n   Incoming datagram:  ";
    if(incoming==NULL) cout << "No datagram.";
    else               {cout << "\n";  incoming->display(); }
    
    cout << "\n   Outgoing datagram:  ";
    if(outgoing==NULL) cout << "No datagram.";
    else               {cout << "\n"; outgoing->display(); }
    
}


laptop::~laptop()
{
    if (incoming != NULL)
        delete incoming;
    if (outgoing != NULL)
        delete outgoing;
}

void laptop::consumeDatagram()
{
    incoming = NULL;
}

int laptop::canReceiveDatagram()
{
    if (incoming == NULL)
    {
        return 1;
    }
    
    return 0;
}

//*****************************************//
// server class functions

server::server(string n, IPAddress a) : node(n,a)  {
    number_of_laptops = number_of_wans = 0;
    dlist = new msg_list;
}


int server::myType() {
    return SERVER;
}

int  server::canAcceptConnection(int machine_type) {
    if(machine_type==LAPTOP)
        return (number_of_laptops<8);
    else if(machine_type==WAN_MACHINE)
        return (number_of_wans<4);
    return 0;
}

void server::connect(IPAddress x, int machine_type) {
    if(machine_type==LAPTOP)
        laptop_list[number_of_laptops++] = x;
    else if(machine_type==WAN_MACHINE)
        WAN_list[number_of_wans++] = x;
}

void server::receiveDatagram(datagram* d) {
    dlist->append(d);
}

void server::display() {
    int i;
    
    cout << "Server computer:  ";
    node::display();
    
    cout << "\n   Connections to laptops: ";
    if(number_of_laptops==0) cout << "    List is empty.";
    else for(i=0; i<number_of_laptops; i++) {
        cout << "\n      Laptop " << i+1 << ":   ";
        laptop_list[i].display();
    }
    cout << "\n\n   Connections to WANs:    ";
    if(number_of_wans==0) cout << "    List is empty.";
    else for(i=0; i<number_of_wans; i++) {
        cout << "\n         WAN " << i+1 << ":   ";
        WAN_list[i].display();
    }
    
    cout << "\n\n   Message list:\n";
    dlist->display();
    
    cout << "\n\n";
    
}

server::~server()
{
    dlist->deleteList();
}

void server::transferDatagram()
{
    msg_list* temp = new msg_list;
    datagram* first;
    int inside;
    int transfer;
    
    first = dlist->returnFront();
    while (first != NULL)
    {
        inside = 0;
        transfer = 0;
        //laptop case
        for (int i = 0; i < 8; i++)
        {
            if (laptop_list[i].isNULL() != 1)
            {
                if (laptop_list[i].firstOctad() == (first->destinationAddress()).firstOctad())
                {
                    inside = 1;
                    if (laptop_list[i].sameAddress(first->destinationAddress()))
                    {
                        IPAddress lap_ip = laptop_list[i];
                    
                        extern node* network[MAX_MACHINES];
                    
                        for (int j = 0; j < MAX_MACHINES; j++)
                        {
                            if (network[j] != NULL && network[j]->myType() == LAPTOP)
                            {
                                if (network[j]->amIThisComputer(lap_ip))
                                {
                                    if (((laptop*)network[j])->canReceiveDatagram())
                                    {
                                        ((laptop*)network[j])->receiveDatagram(first);
                                        transfer = 1;
                                    }
                                }
                            }
                       
                        }
                    }
                    
                }
            }
        }
       
        //WAN case
        int dif,min,location = 0;
        min = 500; // more than 255
        if (inside == 0)
        {
            for (int i = 0; i < 4; i++)
            {
                if (WAN_list[i].isNULL() != 1)
                {
                    dif = abs(WAN_list[i].firstOctad() -
                              (first->destinationAddress()).firstOctad());
                    
                    if (dif < min)
                    {
                        min = dif;
                        location = i;
                    }
                }
            }
            
            extern node* network[MAX_MACHINES];
            if (number_of_wans > 0)
            {
                
                for (int i = 0; i < MAX_MACHINES; i++)
                {
                    if (network[i] != NULL && network[i]->myType() == WAN_MACHINE)
                    {
                        if (network[i]->amIThisComputer(WAN_list[location]))
                        {
                            ((WAN*)network[i])->receiveDatagram(first);
                            transfer = 1;
                        }
            
                    }
                }
            }
            
        }
        
        if (transfer == 0)
        {
            temp->append(first);
        }
        
        first = dlist->returnFront();
    }
    
    dlist = temp;
    
}

// WAN class functions

WAN::WAN(string n, IPAddress a) : node(n,a)
{
    number_of_servers = number_of_wans = 0;
    dlist = new msg_list;
}

int WAN::myType()
{
    return WAN_MACHINE;
}

int  WAN::canAcceptConnection(int machine_type)
{
    if(machine_type==SERVER)
        return (number_of_servers<4);
    else if(machine_type==WAN_MACHINE)
        return (number_of_wans<4);
    
    return 0;
}

void WAN::connect(IPAddress x, int machine_type)
{
    if(machine_type==SERVER)
        server_list[number_of_servers++] = x;
    else if(machine_type==WAN_MACHINE)
        WAN_list[number_of_wans++] = x;
}

void WAN::receiveDatagram(datagram* d)
{
    dlist->append(d);
}


void WAN::display()
{
    int i;
    
    cout << "WAN computer:  ";
    node::display();
    
    cout << "\n   Connections to servers: ";
    if(number_of_servers==0) cout << "    List is empty.";
    else for(i=0; i<number_of_servers; i++) {
        cout << "\n      Server " << i+1 << ":   ";
        server_list[i].display();
    }
    cout << "\n\n   Connections to WANs:    ";
    if(number_of_wans==0) cout << "    List is empty.";
    else for(i=0; i<number_of_wans; i++) {
        cout << "\n         WAN " << i+1 << ":   ";
        WAN_list[i].display();
    }
    
    cout << "\n\n   Message list:\n";
    dlist->display();
    
    cout << "\n\n";
    
}

WAN::~WAN()
{
    dlist->deleteList();
}

void WAN::transferDatagram()
{
    datagram* first;
    int inside = 0;
    
    first = dlist->returnFront();
    while (first != NULL)
    {
        
        for (int i = 0; i < 4; i++)
        {
            if (server_list[i].isNULL() != 1)
            {
                if (server_list[i].firstOctad() == (first->destinationAddress()).firstOctad())
                {
                    inside = 1;
                    
                    extern node* network[MAX_MACHINES];
                    
                    for (int j = 0; j < MAX_MACHINES; j++)
                    {
                        if (network[j] != NULL && network[j]->myType() == SERVER)
                        {
                            if (network[j]->amIThisComputer(server_list[i]))
                            {
                                ((server*)network[j])->receiveDatagram(first);
                            }
                        }
                    }
                }
            }
        }
        
        if (!inside)
        {
            int location = 0;
            int min = 500;
            int dif = 0;
            
            for (int i = 0; i < 4; i++)
            {
                if (WAN_list[i].isNULL() != 1)
                {
                    dif = abs((WAN_list[i].firstOctad() -
                               (first->destinationAddress()).firstOctad()));
                    
                    if (dif < min)
                    {
                        min = dif;
                        location = i;
                    }
                        
                }
            }
            
            extern node* network[MAX_MACHINES];
            
            if (number_of_wans > 0)
            {
                for (int i = 0; i < MAX_MACHINES; i++)
                {
                    if (network[i] != NULL && network[i]->myType() == WAN_MACHINE)
                    {
                        if (network[i]->amIThisComputer(WAN_list[location]))
                            {
                                ((WAN*)network[i])->receiveDatagram(first);
                            }
                        
                    }
                }
            }
        }
        
        first = dlist->returnFront();
    }
}



