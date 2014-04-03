#include "Cruise_ship.h"
#include "Model.h"
#include "Island.h"
#include <iostream>
#include <algorithm>
#include <cassert>


using std::cout; using std::endl;
using std::shared_ptr;
using std::find_if;

Cruise_ship::Cruise_ship(const std::string& name_, Point position_) :
    Ship(name_, position_, 500., 15., 2., 0), cruise_state(NO_DESTINATION)
{
    remaining_islands = Model::get_instance().get_all_islands();
}

void Cruise_ship::update()
{
    Ship::update();
    if (!can_move()) {
        check_cancle_cruise();
        return;
    }
    switch (cruise_state) {
        case NO_DESTINATION:
            break;
        case MOVING:
            if (!is_moving() && can_dock(current_destination)) {
                dock(current_destination);
                cruise_state = REFUEL;
            }
            break;
        case MOVING_TO_START_ISLAND:
            if (!is_moving() && can_dock(current_destination)) {
                dock(current_destination);
                cout << get_name() << " cruise is over at "
                    << start_island->get_name() << endl;
                cruise_state = NO_DESTINATION;
                remaining_islands = Model::get_instance().get_all_islands();
            }
            break;
        case REFUEL:
            refuel();
            cruise_state = WAIT;
            break;
        case WAIT:
            cruise_state = FIND_NEXT_ISLAND;
            break;
        case FIND_NEXT_ISLAND:
            get_next_destination();
            Ship::set_destination_position_and_speed(current_destination->get_location(),
                                                     cruise_speed);
            cout << get_name() << " will visit "
                << current_destination->get_name() << endl;
            break;
        default:
            assert(false);
            break;
    }
}

void Cruise_ship::describe() const
{
    cout << "\nCruise_ship ";
    Ship::describe();
    if (cruise_state == MOVING || cruise_state == MOVING_TO_START_ISLAND)
        cout << "On cruise to " << current_destination->get_name() << endl;
    else if(cruise_state != NO_DESTINATION)
        cout << "Waiting during cruise at " << current_destination->get_name() << endl;
}


void Cruise_ship::set_destination_position_and_speed(Point destination, double speed)
{
    check_cancle_cruise();
    shared_ptr<Island> island_ptr = is_island_position(destination);
    Ship::set_destination_position_and_speed(destination, speed);
    if (island_ptr) {
        cruise_state = MOVING;
        cout << get_name() << " will visit " << island_ptr->get_name() << endl;
        cout << get_name() <<  " cruise will start and end at "
            << island_ptr->get_name() << endl;
        cruise_speed = speed;
        start_island = island_ptr;
        current_destination = island_ptr;
        remaining_islands.erase(island_ptr);
    }
}



void Cruise_ship::set_course_and_speed(double course, double speed)
{
    check_cancle_cruise();
    Ship::set_course_and_speed(course, speed);
}


void Cruise_ship::stop()
{
    check_cancle_cruise();
    Ship::stop();
}

void Cruise_ship::check_cancle_cruise()
{
    if (cruise_state != NO_DESTINATION) {
        cout << get_name() << " canceling current cruise" << endl;
        cruise_state = NO_DESTINATION;
        remaining_islands = Model::get_instance().get_all_islands();
    }
}


void Cruise_ship::get_next_destination()
{
    if (remaining_islands.size() == 0) {
        cruise_state = MOVING_TO_START_ISLAND;
        current_destination = start_island;
        return;
    }
    shared_ptr<Island> closest_unvisited_island = *remaining_islands.begin();
    double closest_distance = cartesian_distance(current_destination->get_location(),
                                                 closest_unvisited_island->get_location());
    for (auto& island_ptr : remaining_islands) {
        double distance = cartesian_distance(current_destination->get_location(),
                                             island_ptr->get_location());
        if (distance < closest_distance) {
            closest_unvisited_island = island_ptr;
            closest_distance = distance;
        }
    }
    current_destination = closest_unvisited_island;
    cruise_state = MOVING;
    remaining_islands.erase(closest_unvisited_island);
}


std::shared_ptr<Island> Cruise_ship::is_island_position(Point position)
{
    auto set_it = find_if(remaining_islands.begin(), remaining_islands.end(),
                          [&position](const shared_ptr<Island> island_ptr)
                            {return position == island_ptr->get_location();});
    return (set_it == remaining_islands.end()) ? nullptr : *set_it;
}





