#ifndef VIEW_H
#define VIEW_H

#include "Geometry.h"
#include <string>

/* This class provides the interface for all of view objects. */

class View {
public:
    virtual ~View() {}
    
	virtual void update_location(const std::string& name, Point location) {}
    
    virtual void update_fuel(const std::string& name, double fuel) {}
    
    virtual void update_course(const std::string& name, double course) {}
    
    virtual void update_speed(const std::string& name, double speed) {}
    
	// Remove the ship; no error if the name is not present.
	virtual void update_remove(const std::string& name) = 0;
    
	// prints out the current map
	virtual void draw() = 0;
	
	// Discard the saved information - drawing will show only a empty pattern
	virtual void clear() = 0;
};

#endif
