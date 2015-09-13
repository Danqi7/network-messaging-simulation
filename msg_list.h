#ifndef MSG_LIST_H
#define MSG_LIST_H

#include "datagram.h"

class msg_list;

class msg_list_node
{
	private:
		msg_list_node* next;
		datagram* d;
		
	friend	msg_list;
};


class msg_list
{
	private:
        msg_list_node* front;
        msg_list_node* back;
		
	public:
		msg_list();
		void display();
		void append(datagram* x);
        datagram* returnFront();
        void deleteList();
};


#endif