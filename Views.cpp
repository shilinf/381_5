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

const int axes_label_gap = 3;
const int label_width = 4;
const int sailing_view_field_width = 10;

class Cout_format_saver {
public:
    Cout_format_saver() :
    old_flags(cout.flags()), old_precision(cout.precision()) {}
    ~Cout_format_saver() {
        cout.flags(old_flags);
        cout.precision(old_precision);
    }
private:
    ios::fmtflags old_flags;
    streamsize old_precision;
};


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
    points[name] = location;
}

void Map_view::update_remove(const std::string& name)
{
    points.erase(name);
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
    Cout_format_saver cfs;
    cout.precision(0);
    for (int i = 0; i < size; i++) {
        cout << setw(label_width);
        if ((size - i) % axes_label_gap == 1)
            cout << origin.y + scale * (size - i - 1);
        else
            cout << "";
        cout << ' ';
        for (int j = 0; j < size; j++)
            cout << output[j][size - i - 1];
        cout << endl;
    }
    for (int i = 0; i <= (size-1)/axes_label_gap ; i++) {
        cout << "  " << setw(label_width) << origin.x + axes_label_gap * scale * i;
    }
    cout << endl;
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
    ships_info[name].fuel = fuel;
}


void Sailing_view::update_course_speed(const std::string& name, Course_speed cs)
{
    ships_info[name].cs = cs;
}


void Sailing_view::update_speed(const std::string& name, double speed)
{
    ships_info[name].cs.speed = speed;
}

void Sailing_view::update_remove(const std::string& name)
{
    ships_info.erase(name);
}


// prints out the current map
void Sailing_view::draw()
{
    cout << "----- Sailing Data -----" << endl;
    cout << setw(sailing_view_field_width) << "Ship" << setw(sailing_view_field_width) << "Fuel" << setw(sailing_view_field_width) << "Course" << setw(sailing_view_field_width) << "Speed" << endl;
    for (auto map_pair : ships_info)
        cout << setw(sailing_view_field_width) << map_pair.first << setw(sailing_view_field_width) << map_pair.second.fuel << setw(sailing_view_field_width) << map_pair.second.cs.course << setw(sailing_view_field_width) << map_pair.second.cs.speed << endl;
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
    if (name == ownship_name)
        ownship_location = location;
    points[name] = location;
}

void Bridge_view::update_remove(const std::string& name)
{
    if (name == ownship_name)
        sunk = true;
    points.erase(name);
}


// prints out the current map
void Bridge_view::draw()
{
    vector< vector<string> > output;
    if (sunk) {
        cout << "Bridge view from " << ownship_name << " sunk at " << ownship_location << endl;
        output = vector< vector<string> >(3, vector<string>(19, "w-"));
    }
    else {
        cout << "Bridge view from " << ownship_name <<  " position " << ownship_location << " heading " << ownship_course << endl;
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
    
    for (int i = 0 ; i < axes_label_gap; i++) {
        cout << setw(label_width) << "" << ' ';
        for (int j = 0; j < 19; j++)
            cout << output[i][j];
        cout << endl;
    }
    Cout_format_saver cfs;
    cout.precision(0);
    for (int i = 0; i <= 6 ; i++) {
        cout << "  " << setw(label_width) << -90 + axes_label_gap * 10 * i;
    }
    cout << endl;
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


