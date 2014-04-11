#include "Cruiser.h"
#include <iostream>

using std::string;
using std::cout; using std::endl;


void Cruiser::update()
{
    Warship::update();
    if (is_attacking()) {
        if (target_in_range())
            fire_at_target();
        else {
            cout << get_name() << " target is out of range" << endl;
            stop_attack();
        }
    }
}

void Cruiser::describe() const
{
    cout << "\nCruiser ";
    Warship::describe();
}

void Cruiser::receive_hit(int hit_force, std::shared_ptr<Ship> attacker_ptr)
{
    Ship::receive_hit(hit_force, attacker_ptr);
    if (!is_attacking() && is_afloat())
        Warship::attack(attacker_ptr);
}


