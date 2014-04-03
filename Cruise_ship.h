#ifndef CRUISE_SHIP_H
#define CRUISE_SHIP_H


#include "Ship.h"
#include "Utility.h"
#include <vector>
#include <set>
#include <memory>

class Island;

class Cruise_ship : public Ship {
public:
	Cruise_ship(const std::string& name_, Point position_);
    
    void set_destination_position_and_speed(Point destination, double speed) override;
    
	void set_course_and_speed(double course, double speed) override;
    
    void stop() override;
	
	void update() override;
    
	void describe() const override;
    
private:
    enum Cruise_state_e {NO_DESTINATION, MOVING, REFUEL, WAIT, FIND_NEXT_ISLAND, MOVING_TO_START_ISLAND};
    Cruise_state_e cruise_state;
    std::shared_ptr<Island> start_island;
    std::shared_ptr<Island> current_destination;
    std::set<std::shared_ptr<Island>, Island_comp> remaining_islands;
    double cruise_speed;
    void check_cancle_cruise();
    void get_next_destination();
    std::shared_ptr<Island> is_island_position(Point position);
};


#endif
