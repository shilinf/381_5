#include "Controller.h"
#include "Model.h"
#include "View.h"
#include "Views.h"
#include "Ship.h"
#include "Island.h"
#include "Geometry.h"
#include "Ship_factory.h"
#include "Utility.h"
#include <iostream>
#include <utility>
#include <algorithm>
#include <functional>

using std::string;
using std::cout; using std::cin; using std::endl;
using std::map;
using std::shared_ptr;
using std::for_each; using std::find_if;
using std::mem_fn;

Controller::Controller()
{
    commands_map["open_map_view"] = &Controller::open_map_view;
    commands_map["close_map_view"] = &Controller::close_map_view;
    commands_map["open_sailing_view"] = &Controller::open_sailing_view;
    commands_map["close_sailing_view"] = &Controller::close_sailing_view;
    commands_map["open_bridge_view"] = &Controller::open_bridge_view;
    commands_map["close_bridge_view"] = &Controller::close_bridge_view;
    
    commands_map["default"] = &Controller::restore_default_map;
    commands_map["size"] = &Controller::set_map_size;
    commands_map["zoom"] = &Controller::set_map_scale;
    commands_map["pan"] = &Controller::set_map_origin;
    commands_map["show"] = &Controller::draw_map;
    commands_map["status"] = &Controller::show_object_status;
    commands_map["go"] = &Controller::update_all_objects;
    commands_map["create"] = &Controller::create_new_ship;
    
    commands_map["course"] = &Controller::set_ship_course;
    commands_map["position"] = &Controller::set_ship_to_position;
    commands_map["destination"] = &Controller::set_ship_destination_island;
    commands_map["load_at"] = &Controller::set_ship_load_island;
    commands_map["unload_at"] = &Controller::set_ship_unload_island;
    commands_map["dock_at"] = &Controller::set_ship_dock_island;
    commands_map["attack"] = &Controller::set_ship_attack_target;
    commands_map["refuel"] = &Controller::set_ship_refuel;
    commands_map["stop"] = &Controller::set_ship_stop;
    commands_map["stop_attack"] = &Controller::set_ship_stop_attack;
}

void Controller::run()
{
    string first_word, command;
    while (true) {
        cout << "\nTime " << Model::get_instance().get_time() << ": Enter command: ";
        cin >> first_word;
        if (first_word == "quit") {
            quit();
            return;
        }
        try {
            if (Model::get_instance().is_ship_present(first_word)) {
                target_ship = Model::get_instance().get_ship_ptr(first_word);
                cin >> command;
            }
            else
                command = first_word;
            auto cfp = commands_map[command];
            if (cfp)
                (this->*cfp)();
            else {
                commands_map.erase(command);
                cout << "Unrecognized command!" << endl;
                discard_input_remainder();
            }
        } catch (Error& error) {
            cout << error.what() << endl;
            discard_input_remainder();
        } catch (...) {
            cout << "Unknown exception caught." << endl;
            return;
        }
        target_ship.reset();
    }
}

void Controller::open_map_view()
{
    if (map_view_ptr)
        throw Error("Map view is already open!");
    map_view_ptr.reset(new Map_view);
    draw_view_order.push_back(map_view_ptr);
    Model::get_instance().attach(map_view_ptr);
}

void Controller::close_map_view()
{
    check_map_view_exist();
    Model::get_instance().detach(map_view_ptr);
    remove_view(map_view_ptr);
    map_view_ptr.reset();
}

void Controller::open_sailing_view()
{
    if (sailing_view_ptr)
        throw Error("Sailing data view is already open!");
    sailing_view_ptr.reset(new Sailing_view);
    draw_view_order.push_back(sailing_view_ptr);
    Model::get_instance().attach(sailing_view_ptr);
}

void Controller::close_sailing_view()
{
    if (!sailing_view_ptr)
        throw Error("Sailing data view is not open!");
    Model::get_instance().detach(sailing_view_ptr);
    remove_view(sailing_view_ptr);
    sailing_view_ptr.reset();
}

void Controller::open_bridge_view()
{
    string ship_name = read_string();
    if (bridge_view_container.find(ship_name) != bridge_view_container.end())
        throw Error("Bridge view is already open for that ship!");
    shared_ptr<Bridge_view> new_bridge_view(new Bridge_view(ship_name));
    bridge_view_container[ship_name] = new_bridge_view;
    draw_view_order.push_back(new_bridge_view);
    Model::get_instance().attach(new_bridge_view);
}


void Controller::close_bridge_view()
{
    string ship_name = read_string();
    auto bridge_view_it = bridge_view_container.find(ship_name);
    if (bridge_view_it == bridge_view_container.end())
    	throw Error("Bridge view for that ship is not open!");
    Model::get_instance().detach(bridge_view_it->second);
    remove_view(bridge_view_it->second);
    bridge_view_container.erase(bridge_view_it);
}



void Controller::restore_default_map()
{
    check_map_view_exist();
    map_view_ptr->set_defaults();
}

void Controller::set_map_size()
{
    check_map_view_exist();
    int size;
    if (!(cin >> size))
        throw Error("Expected an integer!");
    map_view_ptr->set_size(size);
}

void Controller::set_map_scale()
{
    check_map_view_exist();
    double scale = read_double();
    map_view_ptr->set_scale(scale);
}

void Controller::set_map_origin()
{
    check_map_view_exist();
    map_view_ptr->set_origin(read_point());
}

// draw all the exist maps
void Controller::draw_map()
{
    for_each(draw_view_order.begin(), draw_view_order.end(), mem_fn(&View::draw));
}

void Controller::check_map_view_exist()
{
    if (!map_view_ptr)
        throw Error("Map view is not open!");
}

void Controller::show_object_status()
{
    Model::get_instance().describe();
}

void Controller::update_all_objects()
{
    Model::get_instance().update();
}

void Controller::create_new_ship()
{
    string name = read_string();
    if (name.length() < 2)
        throw Error("Name is too short!");
    if (Model::get_instance().is_name_in_use(name))
        throw Error("Name is already in use!");
    string ship_type;
    cin >> ship_type;
    shared_ptr<Ship> new_ship = create_ship(name, ship_type, read_point());
    Model::get_instance().add_ship(new_ship);
}

void Controller::set_ship_course()
{
    double course = read_double();
    double speed = read_check_speed();
    if (course < 0.0 || course >= 360.0)
        throw Error("Invalid heading entered!");
    target_ship->set_course_and_speed(course, speed);
}

void Controller::set_ship_to_position()
{
    Point position = read_point();
    double speed = read_check_speed();
    target_ship->set_destination_position_and_speed(position, speed);
}

void Controller::set_ship_destination_island()
{
    Point island_location = read_get_island()->get_location();
    target_ship->set_destination_position_and_speed(island_location, read_check_speed());
}

void Controller::set_ship_load_island()
{
    target_ship->set_load_destination(read_get_island());
}

void Controller::set_ship_unload_island()
{
    target_ship->set_unload_destination(read_get_island());
}

void Controller::set_ship_dock_island()
{
    target_ship->dock(read_get_island());
}

void Controller::set_ship_attack_target()
{
    string ship_name = read_string();
    shared_ptr<Ship> attack_ship = Model::get_instance().get_ship_ptr(ship_name);
    target_ship->attack(attack_ship);
}

void Controller::set_ship_refuel()
{
    target_ship->refuel();
}

void Controller::set_ship_stop()
{
    target_ship->stop();
}

void Controller::set_ship_stop_attack()
{
    target_ship->stop_attack();
}


void Controller::quit()
{
    cout << "Done" << endl;
}


double Controller::read_check_speed()
{
    double speed = read_double();
    if (speed < 0.0)
        throw Error("Negative speed entered!");
    return speed;
}



// Read to new line
void Controller::discard_input_remainder()
{
    cin.clear();
    while (cin.get() != '\n')
        ;
}

Point Controller::read_point()
{
    double x = read_double();
    double y = read_double();
    return Point(x, y);
}


double Controller::read_double()
{
    double temp;
    if (!(cin >> temp))
        throw Error("Expected a double!");
    return temp;
}

string Controller::read_string()
{
    string read_string;
    cin>>read_string;
    return read_string;
}

shared_ptr<Island> Controller::read_get_island()
{
    string island_name = read_string();
    return Model::get_instance().get_island_ptr(island_name);
}

void Controller::remove_view(std::shared_ptr<View> view)
{
    auto view_it = find_if(draw_view_order.begin(), draw_view_order.end(),
                           [&view](shared_ptr<View> view_ptr){return view_ptr == view;});
    draw_view_order.erase(view_it);
}








