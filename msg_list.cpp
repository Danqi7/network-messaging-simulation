#include <iostream>

#include "msg_list.h"

msg_list::msg_list()
{
    //cout << "msg_list constructor is called" << endl;
	front = back = NULL;
}

void msg_list::display()
{
	msg_list_node *tmp;  int i;
	
	if(front == NULL)
    {
		cout << "** List is empty. **\n";
		return;
	}
	
	tmp = front;  i = 1;
	while(tmp != NULL)
    {
		cout << "Datagram " << i++ << ":  \n";
		(tmp->d)->display();
		cout << "\n";
		tmp = tmp->next;
	}
	
}

void msg_list::append(datagram *x)
{
	msg_list_node *tmp;
	
	tmp = new msg_list_node;
	tmp->next = NULL;
	tmp->d = x;
	if(front==NULL) front = tmp;
	else 		back->next = tmp;
	back = tmp;
}


void msg_list::deleteList()
{
    while (front != NULL)
    {
        msg_list_node* tmp = front;
        front = front->next;
        delete tmp->d;
        delete tmp;
    }
    back = NULL;
}

datagram* msg_list::returnFront()
{
    if (front == NULL && back == NULL)
    {
        return NULL;
    }
    msg_list_node* first = front;
    front = front->next;
    if (front == NULL)
        back = NULL;
    
    delete first;
    return first->d;
    
}

