#include "Tanker.h"
#include "Utility.h"
#include "Island.h"
#include <iostream>
#include <cassert>

using std::string;
using std::cout; using std::endl;
using std::shared_ptr;


void Tanker::set_destination_position_and_speed(Point destination, double speed)
{
    check_no_cargo_destination();
    Ship::set_destination_position_and_speed(destination, speed);
}

void Tanker::set_course_and_speed(double course, double speed)
{
    check_no_cargo_destination();
    Ship::set_course_and_speed(course, speed);
}


void Tanker::stop()
{
    Ship::stop();
    clear_destination();
    cout << get_name() << " now has no cargo destinations" << endl;
}

void Tanker::describe() const
{
    cout << "\nTanker ";
    Ship::describe();
    cout << "Cargo: " << cargo << " tons";
    switch (tanker_state) {
        case NO_CARGO_DESTINATIONS:
            cout << ", no cargo destinations";
            break;
        case UNLOADING:
            cout << ", unloading";
            break;
        case MOVING_TO_LOADING:
            cout << ", moving to loading destination";
            break;
        case LOADING:
            cout << ", loading";
            break;
        case MOVING_TO_UNLOADING:
            cout << ", moving to unloading destination";
            break;
        default:
            assert(false);
            break;
    }
    cout << endl;
}

void Tanker::set_load_destination(shared_ptr<Island> destination)
{
    check_no_cargo_destination();
    load_destination = destination;
    if (destination == unload_destination)
        throw Error("Load and unload cargo destinations are the same!");
     cout <<  get_name() << " will load at " << destination->get_name() << endl;
    if (unload_destination)
        start_cycle();
}


void Tanker::set_unload_destination(shared_ptr<Island> destination)
{
    check_no_cargo_destination();
    unload_destination = destination;
    if (destination == load_destination)
        throw Error("Load and unload cargo destinations are the same!");
    cout << get_name() << " will unload at " << destination->get_name() << endl;
    if (load_destination)
        start_cycle();
}

void Tanker::start_cycle()
{
    if (is_docked()) {
        if (get_docked_Island() == load_destination)
            tanker_state = LOADING;
        else if (get_docked_Island() == unload_destination)
            tanker_state = UNLOADING;
    }
    else if (cargo == 0. && can_dock(load_destination)) {
        dock(load_destination);
        tanker_state = LOADING;
    }
    else if (cargo > 0. && can_dock(unload_destination)) {
        dock(load_destination);
        tanker_state = UNLOADING;
    }
    else if (cargo == 0.) {
        Ship::set_destination_position_and_speed(load_destination->get_location(),
                                                 get_maximum_speed());
        tanker_state = MOVING_TO_LOADING;
    }
    else if (cargo > 0.) {
        Ship::set_destination_position_and_speed(unload_destination->get_location(),
                                                 get_maximum_speed());
        tanker_state = MOVING_TO_UNLOADING;
    }
}

void Tanker::update()
{
    Ship::update();
    if (!can_move()) {
        if (tanker_state != NO_CARGO_DESTINATIONS) {
            clear_destination();
            cout << get_name() << " now has no cargo destinations" << endl;
        }
        return;
    }
    switch (tanker_state) {
        case NO_CARGO_DESTINATIONS:
            break;
        case MOVING_TO_LOADING:
            if (!is_moving() && can_dock(load_destination)) {
                dock(load_destination);
                tanker_state = LOADING;
            }
            break;
        case MOVING_TO_UNLOADING:
            if (!is_moving() && can_dock(unload_destination)) {
                dock(unload_destination);
                tanker_state = UNLOADING;
            }
            break;
        case LOADING:
        {
            refuel();
            double cargo_needed = cargo_capacity - cargo;
            if (cargo_needed < 0.005) {
                cargo = cargo_capacity;
                Ship::set_destination_position_and_speed(unload_destination->get_location(),
                                                         get_maximum_speed());
                tanker_state = MOVING_TO_UNLOADING;
            }
            else {
                cargo += load_destination->provide_fuel(cargo_needed);
                cout << get_name() <<  " now has " <<cargo << " of cargo" << endl;
            }
            break;
        }
        case UNLOADING:
            if (cargo == 0.) {
                Ship::set_destination_position_and_speed(load_destination->get_location(),
                                                         get_maximum_speed());
                tanker_state = MOVING_TO_LOADING;
            }
            else {
                unload_destination->accept_fuel(cargo);
                cargo = 0.;
            }
            break;
        default:
            assert(false);
            break;
    }
}

void Tanker::check_no_cargo_destination()
{
    if (tanker_state != NO_CARGO_DESTINATIONS)
        throw Error("Tanker has cargo destinations!");
}

void Tanker::clear_destination()
{
    load_destination.reset();
    unload_destination.reset();
    tanker_state = NO_CARGO_DESTINATIONS;
}


