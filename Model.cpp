#include "Model.h"
#include "Sim_object.h"
#include "Island.h"
#include "Ship.h"
#include "Ship_factory.h"
#include "View.h"
#include "Geometry.h"
#include <algorithm>
#include <functional>

using std::string;
using std::mem_fn; using std::bind;
using std::placeholders::_1; using std::ref;
using std::map; using std::set;
using std::shared_ptr;
using std::for_each;


Model& Model::get_instance()
{
    static Model the_model;
    return the_model;
}

Model::Model() : time(0){
	island_container["Exxon"] = shared_ptr<Island>(new Island("Exxon", Point(10, 10), 1000, 200));
	island_container["Shell"] = shared_ptr<Island>(new Island("Shell", Point(0, 30), 1000, 200));
	island_container["Bermuda"] = shared_ptr<Island>(new Island("Bermuda", Point(20, 20)));
    island_container["Treasure_Island"] = shared_ptr<Island>(new Island("Treasure_Island", Point(50, 5), 100, 5));
	
	ship_container["Ajax"] = create_ship("Ajax", "Cruiser", Point (15, 15));
	ship_container["Xerxes"] = create_ship("Xerxes", "Cruiser", Point (25, 25));
	ship_container["Valdez"] = create_ship("Valdez", "Tanker", Point (30, 30));
    
    for (auto& island_pair : island_container)
        object_container[island_pair.first.substr(0, 2)] = island_pair.second;
    for (auto& ship_pair : ship_container)
        object_container[ship_pair.first.substr(0, 2)] = ship_pair.second;
}

bool Model::is_name_in_use(const std::string& name) const
{
    return object_container.find(name.substr(0, 2)) != object_container.end();
}

bool Model::is_island_present(const std::string& name) const
{
    return island_container.find(name) != island_container.end();
}

void Model::add_island(shared_ptr<Island> new_island)
{
    island_container[new_island->get_name()] = new_island;
    object_container[new_island->get_name().substr(0, 2)] = new_island;
    new_island->broadcast_current_state();
}

shared_ptr<Island> Model::get_island_ptr(const std::string& name) const
{
    auto island_container_it = island_container.find(name);
    if (island_container_it == island_container.end())
        throw Error("Island not found!");
    return island_container_it->second;
}

bool Model::is_ship_present(const std::string& name) const
{
    return ship_container.find(name) != ship_container.end();
}

void Model::add_ship(shared_ptr<Ship> new_ship)
{
    ship_container[new_ship->get_name()] = new_ship;
    object_container[new_ship->get_name().substr(0, 2)] = new_ship;
    new_ship->broadcast_current_state();
}


shared_ptr<Ship> Model::get_ship_ptr(const std::string& name) const
{
    auto ship_container_it = ship_container.find(name);
    if (ship_container_it == ship_container.end())
        throw Error("Ship not found!");
    return ship_container_it->second;
}
                              
void Model::describe() const
{    
    for_each(object_container.begin(), object_container.end(),
             bind(&Sim_object::describe,
                  bind(& map<string, shared_ptr<Sim_object> >::value_type::second, _1)));
}


void Model::update()
{
    ++time;
    for_each(object_container.begin(), object_container.end(),
             bind(&Sim_object::update,
                  bind(& map<string, shared_ptr<Sim_object> >::value_type::second, _1)));
}

void Model::attach(shared_ptr<View> view)
{
    view_container.insert(view);
    for_each(object_container.begin(), object_container.end(),
             bind(&Sim_object::broadcast_current_state,
                  bind(& map<string, shared_ptr<Sim_object> >::value_type::second, _1)));
}

void Model::detach(shared_ptr<View> view)
{
    view_container.erase(view);
}

void Model::notify_location(const std::string& name, Point location)
{
    for_each(view_container.begin(), view_container.end(),
             bind(&View::update_location, _1, ref(name), ref(location)));
}

void Model::notify_fuel(const std::string& name, double fuel)
{
    for_each(view_container.begin(), view_container.end(),
             bind(&View::update_fuel, _1, ref(name), ref(fuel)));
}

void Model::notify_course(const std::string& name, double course)
{
    for_each(view_container.begin(), view_container.end(),
             bind(&View::update_course, _1, ref(name), ref(course)));
}


void Model::notify_speed(const std::string& name, double speed)
{
    for_each(view_container.begin(), view_container.end(),
             bind(&View::update_speed, _1, ref(name), ref(speed)));
}


void Model::notify_gone(const std::string& name)
{
    for_each(view_container.begin(), view_container.end(),
             bind(&View::update_remove, _1, ref(name)));
}


void Model::remove_ship(shared_ptr<Ship> ship_ptr)
{
    ship_container.erase(ship_ptr->get_name());
    object_container.erase(ship_ptr->get_name().substr(0, 2));
}


set<shared_ptr<Island>, Island_comp> Model::get_all_islands() const
{
    set<shared_ptr<Island>, Island_comp> all_islands;
    for (auto& map_pair : island_container)
        all_islands.insert(map_pair.second);
    return all_islands;
}

