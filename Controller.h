#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <string>
#include <map>
#include <vector>
#include <memory>

/* Controller
This class is responsible for controlling the Model and View according to interactions
with the user.
*/

class View;
class Map_view;
class Sailing_view;
class Bridge_view;
class Ship;
class Island;
class Controller;
struct Point;

using Command_map_t = std::map<std::string, void(Controller::*)()>;


class Controller {
public:	
	// output constructor message
	Controller();
	// output destructor message
	~Controller();

	// create View object, run the program by acccepting user commands, then destroy View object
	void run();
    
    
private:
    std::shared_ptr<Map_view> map_view_ptr;
    std::shared_ptr<Sailing_view> sailing_view_ptr;
    std::map<std::string, std::shared_ptr<Bridge_view>> bridge_view_container;
    std::vector<std::shared_ptr<View>> draw_view_order;
    
    
    std::shared_ptr<Ship> target_ship;
    
    void discard_input_remainder();
    void quit();
    void load_command_map(Command_map_t &commands_map);
    void restore_default_map();
    
    
    void open_map_view();
    void close_map_view();
    void open_sailing_view();
    void close_sailing_view();
    void open_bridge_view();
    void close_bridge_view();
    
    
    void set_map_size();
    void set_map_scale();
    void set_map_origin();
    void draw_map();
    void show_object_status();
    void update_all_objects();
    void create_new_ship();
    
    void set_ship_course();
    void set_ship_to_position();
    void set_ship_destination_island();
    void set_ship_load_island();
    void set_ship_unload_island();
    void set_ship_dock_island();
    void set_ship_attack_target();
    void set_ship_refuel();
    void set_ship_stop();
    void set_ship_stop_attack();
    
    void check_map_view_exist();
    
    
    Point read_point();
    double read_double();
    double read_check_speed();
    std::string read_string();
    std::shared_ptr<Island> read_get_island();
    void remove_view(std::shared_ptr<View> view);
};

#endif






