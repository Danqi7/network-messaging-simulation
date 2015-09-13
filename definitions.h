#ifndef DEFINITIONS

#define DEFINITIONS

// more constants
#define MAX_CMD_LINE_LENGTH 255
#define MAX_TOKENS_ON_A_LINE 10

#define COMMANDS_FROM_FILE	 1
#define ECHO_COMMAND		 1

// Error codes
#define BAD_IP_ADDRESS	   200
#define UNKNOWN_MACHINE_TYPE	201
#define NETWORK_FULL			202
#define NO_SUCH_MACHINE		203
#define CONNECTION_REFUSED	204

// System commands
#define NUMBER_OF_COMMANDS	 9

#define HALT			    50
#define SYSTEM_STATUS		51
#define CREATE_MACHINE		60
#define DESTROY_MACHINE		61
#define CONNECT			    62
#define CHECK_CONNECTIONS	63
#define DATAGRAM_CMD		70
#define CONSUME_DATAGRAM	71
#define TIME_CLICK		    80

#define UNDEFINED_COMMAND	99

#define LAPTOP				1
#define SERVER				2
#define WAN_MACHINE			3
#define MAX_MACHINES		10


#endif
