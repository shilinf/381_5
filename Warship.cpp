#include "Warship.h"
#include "Utility.h"
#include <iostream>

using std::string;
using std::cout; using std::endl;
using std::shared_ptr;

Warship::~Warship() {}

void Warship::update()
{
    Ship::update();
    if (attacking) {
        shared_ptr<Ship> sp = get_target();
        if (!sp || !sp->is_afloat())
            stop_attack();
        else
            cout << get_name() << " is attacking " << endl;
    }
}

void Warship::attack(shared_ptr<Ship> target_ptr_)
{
    if (!is_afloat())
        throw Error("Cannot attack!");
    if (shared_from_this() == target_ptr_)
        throw Error("Warship may not attack itself!");
    if (target_ptr_ == get_target())
        throw Error("Already attacking this target!");
    target_ptr = target_ptr_;
    attacking = true;
    cout << get_name() << " will attack " << target_ptr_->get_name() << endl;
}

void Warship::stop_attack()
{
    if (!attacking)
        throw Error("Was not attacking!");
    attacking = false;
    target_ptr.reset();
    cout << get_name() << " stopping attack" << endl;
}



void Warship::describe() const
{
    Ship::describe();
    if (attacking) {
        shared_ptr<Ship> sp = get_target();
        if (!sp)
            cout << "Attacking absent ship" << endl;
        else
            cout << "Attacking " << sp->get_name() << endl;
    }
}


bool Warship::is_attacking() const
{
    return attacking;
}

void Warship::fire_at_target()
{
    cout << get_name() << " fires" << endl;
    get_target()->receive_hit(firepower, shared_from_this());
}

bool Warship::target_in_range() const
{
    return cartesian_distance(get_location(), get_target()->get_location())
        <= maximum_range;
}

shared_ptr<Ship> Warship::get_target() const
{
    return target_ptr.lock();
}




