#ifndef VIEWS_H
#define VIEWS_H

#include "View.h"
#include "Navigation.h"
#include "Geometry.h"
#include <map>
#include <string>

class Map_view : public View {
public:
    Map_view();
    
    void update_location(const std::string& name, Point location) override;
	
	// Remove the name and its location; no error if the name is not present.
    void update_remove(const std::string& name) override;
    
	// prints out the current map
    void draw() override;
	
	// Discard the saved information - drawing will show only a empty pattern
    void clear() override;
    
	// modify the display parameters
	// if the size is out of bounds will throw Error("New map size is too big!")
	// or Error("New map size is too small!")
	void set_size(int size_);
	
	// If scale is not postive, will throw Error("New map scale must be positive!");
	void set_scale(double scale_);
	
	// any values are legal for the origin
	void set_origin(Point origin_);
	
	// set the parameters to the default values
	void set_defaults();

private:
    int size;			// current size of the display
	double scale;		// distance per cell of the display
	Point origin;		// coordinates of the lower-left-hand corner
    std::map<std::string, Point> points;
    
	// Calculate the cell subscripts corresponding to the location parameter, using the
	// current size, scale, and origin of the display.
	// Return true if the location is within the map, false if not
	bool get_subscripts(int &ix, int &iy, Point location);
};



class Sailing_view : public View {
public:
    void update_fuel(const std::string& name, double fuel) override;
    
    void update_course(const std::string& name, double course) override;
    
    void update_speed(const std::string& name, double speed) override;
    
    // Remove the ship; no error if the name is not present.
    void update_remove(const std::string& name) override;
    
	// prints out the current map
    void draw() override;
	
	// Discard the saved information - drawing will show only a empty pattern
    void clear() override;
    
private:
    struct Fuel_course_speed
    {
        Course_speed cs;
        double fuel;
        Fuel_course_speed(Course_speed cs_ = Course_speed(), double fuel_ = 0.) :
        cs(cs_), fuel(fuel_){}
    };
    std::map<std::string, Fuel_course_speed> ships_info;
};


class Bridge_view : public View {
public:
    Bridge_view(std::string ownship_name_) :
        ownship_name(ownship_name_), sunk(false) {}

    void update_course(const std::string& name, double course) override;
    
    void update_location(const std::string& name, Point location) override;
	
	// Remove the ship; no error if the name is not present.
    void update_remove(const std::string& name) override;
    
	// prints out the current map
    void draw() override;
	
	// Discard the saved information - drawing will show only a empty pattern
    void clear() override;

private:
    std::string ownship_name;
    double ownship_course;
    bool sunk;
    std::map<std::string, Point> points;
    
    bool compute_subscribt(double bearing, int &x);
};


#endif
