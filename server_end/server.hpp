#ifndef SERVER_HPP
#define SERVER_HPP


// defines arguments used for the conn accept event in server side
struct EventArgsAccept
{
    sockaddr_in sin;
};

struct EventArgsRead {
    event ev_read;
};


#endif // SERVER_HPP