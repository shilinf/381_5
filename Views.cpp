#include "Views.h"
#include "Utility.h"
#include "Navigation.h"
#include <iostream>
#include <cmath>
#include <vector>
#include <string>
#include <iomanip>


using std::cout; using std::endl;
using std::vector;
using std::string;
using std::ios; using std::setw;
using std::streamsize;




Map_view::Map_view()
{
    set_defaults();
}

void Map_view::set_size(int size_)
{
    if (size_ <= 6)
        throw Error("New map size is too small!");
    if (size_ > 30)
        throw Error("New map size is too big!");
    size = size_;
}

void Map_view::set_scale(double scale_)
{
    if (scale_ <= 0.0)
        throw Error("New map scale must be positive!");
    scale = scale_;
}

void Map_view::set_origin(Point origin_)
{
    origin = origin_;
}

void Map_view::set_defaults()
{
    size = 25;
    scale = 2.0;
    origin = Point(-10, -10);
}

void Map_view::update_location(const std::string& name, Point location)
{
    auto pair_it = points.find(name);
    if (pair_it != points.end())
        points.erase(pair_it);
    points.insert(make_pair(name, location));
}

void Map_view::update_remove(const std::string& name)
{
    points.erase(points.find(name));
}

void Map_view::draw()
{
    cout << "Display size: " <<size << ", scale: " << scale << ", origin: " << origin << endl;
    vector< vector<string> > output(size, vector<string>(size, ". "));
    bool exist_out_of_map = false;
    for (auto map_pair : points) {
        int x, y;
        if (get_subscripts(x, y, map_pair.second)) {
            if (output[x][y] == ". ")
                output[x][y] = map_pair.first.substr(0, 2);
            else
                output[x][y] = "* ";
        }
        else {
            if (exist_out_of_map)
                cout << ", " << map_pair.first;
            else {
                cout << map_pair.first;
                exist_out_of_map = true;
            }
        }
    }
    if (exist_out_of_map)
        cout << " outside the map" << endl;
    ios::fmtflags old_settings = cout.flags(); //save previous format flags
    streamsize old_precision = cout.precision();
    cout.setf(ios::fixed, ios::floatfield); // set fixed floating format
    cout.precision(0);
    for (int i = 0; i < size; i++) {
        if ((size - i) % 3 == 1)
            cout << setw(4) << origin.y + scale * (size - i - 1) << ' ';
        else
            cout << "     ";
        for (int j = 0; j < size; j++)
            cout << output[j][size - i - 1];
        cout << endl;
    }
    for (int i = 0; i <= (size-1)/3 ; i++) {
        cout << "  " << setw(4) << origin.x + 3 * scale * i;
    }
    cout << endl;
    cout.flags(old_settings);
    cout.precision(old_precision);
}

void Map_view::clear()
{
    points.clear();
}

/* *** Use this function to calculate the subscripts for the cell. */

/* *** This code assumes the specified private member variables. */


// Calculate the cell subscripts corresponding to the supplied location parameter,
// using the current size, scale, and origin of the display.
// This function assumes that origin is a  member variable of type Point,
// scale is a double value, and size is an integer for the number of rows/columns
// currently being used for the grid.
// Return true if the location is within the grid, false if not
bool Map_view::get_subscripts(int &ix, int &iy, Point location)
{
	// adjust with origin and scale
	Cartesian_vector subscripts = (location - origin) / scale;
	// truncate coordinates to integer after taking the floor
	// floor function will produce integer smaller than even for negative values,
	// so - 0.05 => -1., which will be outside the array.
	ix = int(floor(subscripts.delta_x));
	iy = int(floor(subscripts.delta_y));
	// if out of range, return false
	if ((ix < 0) || (ix >= size) || (iy < 0) || (iy >= size)) {
		return false;
    }
	else
		return true;
}


void Sailing_view::update_fuel(const std::string& name, double fuel)
{
    auto pair_it = ships_info.find(name);
    if (pair_it != ships_info.end())
        pair_it->second.fuel = fuel;
    else
        ships_info.insert(make_pair(name, Fuel_course_speed(fuel)));
}



void Sailing_view::update_course_speed(const std::string& name, Course_speed cs)
{
    auto pair_it = ships_info.find(name);
    if (pair_it != ships_info.end())
        pair_it->second.cs = cs;
    //else
    //    ships_info.insert(make_pair(name, Fuel_course_speed(0., cs)));
}


void Sailing_view::update_speed(const std::string& name, double speed)
{
    auto pair_it = ships_info.find(name);
    if (pair_it != ships_info.end())
        pair_it->second.cs.speed = speed;
}

void Sailing_view::update_remove(const std::string& name)
{
    ships_info.erase(ships_info.find(name));
}


// prints out the current map
void Sailing_view::draw()
{
    cout << "----- Sailing Data -----" << endl;
    cout << setw(10) << "Ship" << setw(10) << "Fuel" << setw(10) << "Course" << setw(10) << "Speed" << endl;
    for (auto map_pair : ships_info)
        cout << setw(10) << map_pair.first << setw(10) << map_pair.second.fuel << setw(10) << map_pair.second.cs.course << setw(10) << map_pair.second.cs.speed << endl;
}

void Sailing_view::clear()
{
    ships_info.clear();
}


Bridge_view::Bridge_view(std::string ownship_name_, Point location_) : ownship_name(ownship_name_), ownship_location(location_), sunk(false) {}

void Bridge_view::update_course_speed(const std::string& name, Course_speed cs)
{
    if (ownship_name == name)
        ownship_course = cs.course;
}


// Considering combine this with the map_view
void Bridge_view::update_location(const std::string& name, Point location)
{
    auto pair_it = points.find(name);
    if (pair_it != points.end())
        points.erase(pair_it);
    points.insert(make_pair(name, location));
    if (name == ownship_name)
        ownship_location = location;
}

void Bridge_view::update_remove(const std::string& name)
{
    if (name == ownship_name)
        sunk = true;
    points.erase(points.find(name));
}



// prints out the current map
void Bridge_view::draw()
{
    vector< vector<string> > output;
    if (sunk)
        output = vector< vector<string> >(3, vector<string>(19, "w-"));
    else {
        output = vector< vector<string> >(3, vector<string>(19, ". "));
        for (auto map_pair : points) {
            Compass_position relative_position(ownship_location, map_pair.second);
            if (relative_position.range >= 0.005 && relative_position.range <= 20.) {
                int x;
                if (compute_subscribt(relative_position.bearing, x)) {
                    if (output[2][x] == ". ")
                        output[2][x] = map_pair.first.substr(0, 2);
                    else
                        output[2][x] = "**";
                }
            }
        }
    }
    for (int i = 0 ; i < 3; i++) {
        cout << "     ";
        for (int j = 0; j < 19; j++)
            cout << output[j][i];
        cout << endl;
    }
    
    ios::fmtflags old_settings = cout.flags(); //save previous format flags
    streamsize old_precision = cout.precision();
    cout.setf(ios::fixed, ios::floatfield); // set fixed floating format
    cout.precision(0);

    
    
    for (int i = 0; i <= 6 ; i++) {
        cout << "  " << setw(4) << -90 + 3 * 10 * i;
    }
    cout << endl;
    cout.flags(old_settings);
    cout.precision(old_precision);


}

// Discard the saved information - drawing will show only a empty pattern
void Bridge_view::clear()
{
    points.clear();
}


bool Bridge_view::compute_subscribt(double bearing, int &x)
{
    double bridge_view_angle = bearing - ownship_course;
    if (bridge_view_angle < -180.)
        bridge_view_angle += 360.;
    else if (bridge_view_angle > 180.)
        bridge_view_angle -= 360.;
    double delta_x = (bridge_view_angle + 90.) / 10.0;
	x = int(floor(delta_x));
	if (x < 0 || x > 18)
		return false;
	else
		return true;
}









