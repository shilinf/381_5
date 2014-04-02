#ifndef UTILITIES_H
#define UTILITIES_H

#include "Island.h"
#include <exception>

class Error : public std::exception {
public:
	Error(const char* msg_) : msg(msg_) {}
    const char* what() const noexcept override
        {return msg;}
private:
	const char* msg;
};

struct Island_comp {
    bool operator() (const std::shared_ptr<Island> island1, const std::shared_ptr<Island> island2) const
    {return island1->get_name() < island2->get_name();}
};


#endif
