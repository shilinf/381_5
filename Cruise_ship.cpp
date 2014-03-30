#include "Cruise_ship.h"
#include "Model.h"
#include "Island.h"
#include <iostream>
#include <vector>

using std::cout; using std::endl;
using std::shared_ptr;
using std::vector;

Cruise_ship::Cruise_ship(const std::string& name_, Point position_) : Ship(name_, position_, 500., 15., 2., 0), cruise_state(NO_DESTINATION) {}



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
            if (!is_moving() && can_dock(path.back())) {
                dock(path.back());
                if(path.front() != path.back() || path.size() <= 1)
                    cruise_state = REFUEL;
                else {
                    cout << get_name() << " cruise is over at " << path.back()->get_name() << endl;
                    cruise_state = NO_DESTINATION;
                    path.clear();
                }
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
            Ship::set_destination_position_and_speed(path.back()->get_location(), cruise_speed);
            cruise_state = MOVING;
            cout << get_name() << " will visit " << path.back()->get_name() << endl;
            break;
        default:
            break;
    }
}

void Cruise_ship::describe() const
{
    cout << "\nCruise_ship " << endl;
    Ship::describe();
    if (cruise_state == MOVING)
        cout << "On cruise to " << path.back()->get_name() << endl;
    else if(cruise_state != NO_DESTINATION)
        cout << "Waiting during cruise at " << path.back()->get_name() << endl;
}


void Cruise_ship::set_destination_position_and_speed(Point destination, double speed)
{
    shared_ptr<Island> island_ptr = Model::get_instance().is_island_position(destination);
    if (!island_ptr)
        check_cancle_cruise();
    else {
        cout << get_name() <<  " cruise will start and end at " << island_ptr->get_name() << endl;
        cruise_speed = speed;
        path.push_back(island_ptr);
    }
    Ship::set_destination_position_and_speed(destination, speed);
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
        path.clear();
    }
}




void Cruise_ship::get_next_destination()
{
    bool visited_all = true;
    vector<shared_ptr<Island>> islands_ordered_by_distance = Model::get_instance().islands_ordered_by_distance_to_point(path.back()->get_location());
    for (auto island_ptr : islands_ordered_by_distance) {
        auto path_it = find_if(path.begin(), path.end(), [&island_ptr](shared_ptr<Island>& path_island_ptr){return island_ptr == path_island_ptr;});
        if (path_it == path.end()) {
            path.push_back(island_ptr);
            visited_all = false;
            break;
        }
    }
    if (visited_all)
        path.push_back(path.front());
}






