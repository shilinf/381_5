#include "Island.h"
#include "Model.h"
#include <iostream>

using std::string;
using std::cout; using std::endl;

double Island::provide_fuel(double request)
{
    double provided_amount = (request > fuel) ? fuel : request;
    fuel -= provided_amount;
    cout << "Island " << get_name() << " supplied " << provided_amount
        << " tons of fuel" << endl;
    return provided_amount;
}

void Island::accept_fuel(double amount)
{
    fuel += amount;
    cout << "Island " << get_name() << " now has " << fuel << " tons" << endl;
}

void Island::update()
{
    if (production_rate > 0)
        accept_fuel(production_rate);
}

void Island::describe() const
{
    cout << "\nIsland " << get_name() << " at position " << position << endl;
    cout << "Fuel available: " << fuel << " tons" << endl;
}

void Island::broadcast_current_state()
{
    Model::get_instance().notify_location(get_name(), position);
}
