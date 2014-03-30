#include "Ship_factory.h"
#include "Geometry.h"
#include "Utility.h"
#include "Tanker.h"
#include "Cruiser.h"
#include "Cruise_ship.h"

using std::shared_ptr;

shared_ptr<Ship> create_ship(const std::string& name, const std::string& type, Point initial_position)
{
    if (type == "Cruiser")
        return shared_ptr<Ship>(new Cruiser(name, initial_position));
    else if (type == "Tanker")
        return shared_ptr<Ship>(new Tanker(name, initial_position));
    else if (type == "Cruise_ship")
        return shared_ptr<Ship>(new Cruise_ship(name, initial_position));
    else
        throw Error("Trying to create ship of unknown type!");
}
