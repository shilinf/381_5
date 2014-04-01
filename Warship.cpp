#include "Warship.h"
#include "Utility.h"
#include <string>
#include <iostream>

using std::string;
using std::cout; using std::endl;
using std::shared_ptr;




Warship::~Warship() {}

void Warship::describe() const
{
    Ship::describe();
    if (warship_state == ATTACKING) {
        shared_ptr<Ship> sp = target_ptr.lock();
        if (!sp)
            cout << "Attacking absent ship" << endl;
        else
            cout << "Attacking " << sp->get_name() << endl;
    }
}


void Warship::attack(shared_ptr<Ship> target_ptr_)
{
    if (!is_afloat())
        throw Error("Cannot attack!");
    if (this == target_ptr_.get())
        throw Error("Warship may not attack itself!");
    if (target_ptr_ == target_ptr.lock())
        throw Error("Already attacking this target!");
    target_ptr = target_ptr_;
    warship_state = ATTACKING;
    cout << get_name() << " will attack " << target_ptr_->get_name() << endl;
}

void Warship::stop_attack()
{
    if (!is_attacking())
        throw Error("Was not attacking!");
    warship_state = NOT_ATTACKING;
    target_ptr.reset();
    cout << get_name() << " stopping attack" << endl;
}


// check the use of get_target, what about return a nullptr here???
void Warship::update()
{
    Ship::update();
    if (warship_state == ATTACKING) {
        shared_ptr<Ship> sp = target_ptr.lock();
        if (!is_afloat() || !sp || !sp->is_afloat()) {
            stop_attack();
        }
        else
            cout << get_name() << " is attacking " << endl;
    }
}

bool Warship::is_attacking() const
{
    return warship_state == ATTACKING;
}


//check whether return wrong type
void Warship::fire_at_target()
{
    cout << get_name() << " fires" << endl;
    get_target()->receive_hit(firepower, shared_from_this());
}

bool Warship::target_in_range() const
{
    return cartesian_distance(get_location(), get_target()->get_location()) <= maximum_range;
}

shared_ptr<Ship> Warship::get_target() const
{
    return target_ptr.lock();
}




