#ifndef CRUISE_SHIP_H
#define CRUISE_SHIP_H


#include "Ship.h"
#include <vector>

class Island;

class Cruise_ship : public Ship {
public:
    // initialize, then output constructor message
	Cruise_ship(const std::string& name_, Point position_);
	// output destructor message
	//~Cruise_ship();

    
    void set_destination_position_and_speed(Point destination, double speed) override;
    
	void set_course_and_speed(double course, double speed) override;
    
    
    
    void stop() override;
	
	void update() override;
    
	void describe() const override;

    
    
    
    
    
private:
    enum Cruise_state_e {NO_DESTINATION, MOVING, REFUEL, WAIT, FIND_NEXT_ISLAND};
    Cruise_state_e cruise_state;
    std::vector<std::shared_ptr<Island> > path;
    
    // do we need to store the speed?????
    double cruise_speed;
    
    
    void check_cancle_cruise();
    void get_next_destination();
};


#endif
