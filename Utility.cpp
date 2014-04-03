#include "Utility.h"
#include "Island.h"


bool Island_comp::operator() (const std::shared_ptr<Island> island1,
                 const std::shared_ptr<Island> island2) const
{
    return island1->get_name() < island2->get_name();
}