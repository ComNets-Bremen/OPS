#!/usr/bin/python
#
# Script to build the 3 files required by UBM and Extended SWIM as 
# input. The 3 files are: locations.txt, events.txt and properties.txt
#
# Author: Asanga Udugama (adu@comnets.uni-bremen.de)
# Date: 16-mar-2018
#
import sys, os.path, random, math


# general parameters
rng_seed = 128                         # seed used to intialize RNG

# locations.txt file parameters
locations_file = "./locations.txt"
num_locations = 60                     # number of location iun mobility area
area_width  = 1500.0                   # mobility area width (x) in meters
area_height = 1500.0                   # mobility area height (y) in meters
location_radius = 25                   # radius of a location in meters
                                       # computed as min((area_width/num_locations), (area_height/num_locations))

# events.txt file parameters
events_file = "./events.txt"
mean_event_gen_time = 900.0            # in seconds - e.g., every 6 minutes -> 600
sim_time = 86400.0                     # simulation time in seconds
emergency_event_keyword = "emergency"  # keyword used for an emergency
percentage_of_emergency_events = 10    # percentage of emergency events during simulation time
emergency_radius_smallest = 100.0;     # radius of the emergency (between smallest and largest, decided randomly)
emergency_radius_largest = 200.0;      # radius of the emergency (between smallest and largest, decided randomly)
non_emergency_event_keywords = "shopping, sale; music, concert, outdoor; park, relax; cafe, coffee; restaurent, dinner; concert, jazz" 
                                       # event keywords separated by commas, groups separated by semi colons
start_time_earliest = 1800.0           # event start time (between smallest and largest, randomly decided)
start_time_latest = 3600.0             # event start time (between smallest and largest, randomly decided)
event_duration_smallest = 1800.0       # duration of event (between smallest and largest, randomly decided)
event_duration_largest = 10800.0       # duration of event (between smallest and largest, randomly decided)
assigned_popularity_smallest = 25.0    # popularity of event (between smallest and largest, randomly decided)
assigned_popularity_largest = 100.0    # popularity of event (between smallest and largest, randomly decided)

# properties.txt (keywords) file parameters
properties_file = "./properties.txt"
possible_keywords = "shopping, sale, music, concert, outdoor, park, relax, cafe, coffee, restaurent, dinner, jazz"
                                       # keywords assigned to nodes, separated by commas, groups separated by semi colons
assignable_keyword_sets = 30           # assignable keyword combinations
minimum_keywords_assigned = 2          # number of keywords per node (between minimum and maximum, randomly decided)
maximum_keywords_assigned = 5          # number of keywords per node (between minimum and maximum, randomly decided)

def get_dict_value(dict, key):
    val = dict.get(key)
    if val == None:
        print ("Variable", key, "not defined.")
        sys.exit()
    val = val.replace("\"", "")
    val = val.replace("\'", "")
    return val
    
def read_parameters(args):
    global rng_seed
    global locations_file
    global num_locations
    global area_width
    global area_height
    global location_radius
    
    global events_file
    global mean_event_gen_time
    global sim_time
    global emergency_event_keyword
    global percentage_of_emergency_events
    global emergency_radius_smallest
    global emergency_radius_largest
    global non_emergency_event_keywords
    global start_time_earliest
    global start_time_latest
    global event_duration_smallest
    global event_duration_largest
    global assigned_popularity_smallest
    global assigned_popularity_largest
    
    global properties_file
    global possible_keywords
    global assignable_keyword_sets
    global minimum_keywords_assigned
    global maximum_keywords_assigned
    
    if len(args) != 2:
        print ("usage:", args[0], "paramfile")
        sys.exit()
    if not os.path.isfile(args[1]):
        print ("Parameters file", args[1], "not found")
        sys.exit()

    param_dict = {}
    with open(args[1]) as file_ptr:
        for line in file_ptr:
            if len(line.strip()) == 0:
                continue
            if line.startswith("#"):
                continue
            (key, val) = line.split("=")
            param_dict[key.strip()] = val.strip()

    # General parameters
    rng_seed = int(get_dict_value(param_dict, "rng_seed"))
    
    # Location parameters
    locations_file = get_dict_value(param_dict, "locations_file")
    num_locations = int(get_dict_value(param_dict, "num_locations"))
    area_width = float(get_dict_value(param_dict, "area_width"))
    area_height = float(get_dict_value(param_dict, "area_height"))
    location_radius = float(get_dict_value(param_dict, "location_radius"))
    
    # Event parameters
    events_file = get_dict_value(param_dict, "events_file")
    mean_event_gen_time = float(get_dict_value(param_dict, "mean_event_gen_time"))
    sim_time = float(get_dict_value(param_dict, "sim_time"))
    emergency_event_keyword = get_dict_value(param_dict, "emergency_event_keyword")
    percentage_of_emergency_events = float(get_dict_value(param_dict, "percentage_of_emergency_events"))
    emergency_radius_smallest = float(get_dict_value(param_dict, "emergency_radius_smallest"))
    emergency_radius_largest = float(get_dict_value(param_dict, "emergency_radius_largest"))
    non_emergency_event_keywords = get_dict_value(param_dict, "non_emergency_event_keywords")
    start_time_earliest = float(get_dict_value(param_dict, "start_time_earliest"))
    start_time_latest = float(get_dict_value(param_dict, "start_time_latest"))
    event_duration_smallest = float(get_dict_value(param_dict, "event_duration_smallest"))
    event_duration_largest = float(get_dict_value(param_dict, "event_duration_largest"))
    assigned_popularity_smallest = float(get_dict_value(param_dict, "assigned_popularity_smallest"))
    assigned_popularity_largest = float(get_dict_value(param_dict, "assigned_popularity_largest"))
    
    # Keywords parameters
    properties_file = get_dict_value(param_dict, "properties_file")
    possible_keywords = get_dict_value(param_dict, "possible_keywords")
    assignable_keyword_sets = int(get_dict_value(param_dict, "assignable_keyword_sets"))
    minimum_keywords_assigned = int(get_dict_value(param_dict, "minimum_keywords_assigned"))
    maximum_keywords_assigned = int(get_dict_value(param_dict, "maximum_keywords_assigned"))
    
    # show  values
    print("=================================================")
    print("")
    print("Data creation parameters")
    print("-------------------------------------------------")
    print("")
    print("-- General Details --")
    print("RNG Seed:                            ", rng_seed)
    print("")
    print("-- Location Details --")
    print("Locations file:                      ", locations_file)
    print("Number of locations:                 ", num_locations)
    print("Area - width (X):                    ", area_width, "meters")
    print("Area - height (Y):                   ", area_height, "meters")
    print("Location radius:                     ", location_radius, "meters")
    print("")
    print("-- Event Details --")
    print("Events file:                         ", events_file)
    print("Mean event generation time:          ", mean_event_gen_time, "seconds")
    print("Simulation time:                     ", sim_time, "seconds")
    print("Emergency event keyword:             ", emergency_event_keyword)
    print("Emergency event percentage:          ", percentage_of_emergency_events, "% for the simulation duration of", sim_time, "seconds")
    print("Emergency radius (random) between:   ", emergency_radius_smallest, "and", emergency_radius_largest, "meters")
    print("Non-emergency event keywords:        ", non_emergency_event_keywords)
    print("Event start time between:            ", start_time_earliest, "and", start_time_latest, "seconds")
    print("Event duration between:              ", event_duration_smallest, "and", event_duration_largest, "seconds")
    print("Popularity assigned between:         ", assigned_popularity_smallest, "and", assigned_popularity_largest, "in %")
    print("")
    print("-- Keyword Details --")
    print("Keywords file:                       ", properties_file)
    print("Possible keywords of a node:         ", possible_keywords)
    print("Assignable keyword sets for nodes:   ", assignable_keyword_sets)
    print("Keywords assigned to a node between: ", minimum_keywords_assigned, "and", maximum_keywords_assigned)
    print("")

def create_files():
    global rng_seed
    global locations_file
    global num_locations
    global area_width
    global area_height
    global location_radius
    
    global events_file
    global mean_event_gen_time
    global sim_time
    global emergency_event_keyword
    global percentage_of_emergency_events
    global emergency_radius_smallest
    global emergency_radius_largest
    global non_emergency_event_keywords
    global start_time_earliest
    global start_time_latest
    global event_duration_smallest
    global event_duration_largest
    global assigned_popularity_smallest
    global assigned_popularity_largest
    
    global properties_file
    global possible_keywords
    global assignable_keyword_sets
    global minimum_keywords_assigned
    global maximum_keywords_assigned
    
    random.seed(rng_seed)
    
    # create location
    out_file = open(locations_file, "w")
    for i in range(0, num_locations):
        x_coord = int(random.uniform((location_radius * 2.0), (area_width - (location_radius * 2.0))))
        y_coord = int(random.uniform((location_radius * 2.0), (area_height - (location_radius * 2.0))))
        out_file.write("%d %d 0 0\n" % (x_coord, y_coord))
    out_file.close()
    
    # create events
    num_total_events = int(sim_time / mean_event_gen_time)
    num_emergency_events = int(num_total_events * percentage_of_emergency_events / 100)
    
    # build non-emergency keyword array
    comma_removed_keywords = non_emergency_event_keywords.replace(",", "")
    event_keyword_combination_list = comma_removed_keywords.split(";")
    for i in range(0, len(event_keyword_combination_list)):
        event_keyword_combination_list[i] = event_keyword_combination_list[i].strip()
    
    num_emergency_events_inserted = 0
    out_file = open(events_file, "w")
    for i in range(0, num_total_events):
        # decide randomly to insert an emergency event
        is_emergency_event = False
        if num_emergency_events_inserted < num_emergency_events:
            rnd_num = random.uniform(0.0, 100.0)
            if rnd_num <= percentage_of_emergency_events:
                is_emergency_event = True
        
        # identify a location for event and get event location
        rnd_location_index = random.randint(0, (num_locations - 1))
        in_file = open(locations_file, "r")
        line_num = 0
        event_loc_x = 0.0
        event_loc_y = 0.0
        for line in in_file:
            if line_num == rnd_location_index:
                event_loc = line.split()
                event_loc_x = int(event_loc[0])
                event_loc_y = int(event_loc[1])
                break
            line_num += 1
        in_file.close()        
        
        # get event start time and duration
        relative_event_start_time = int(random.uniform(start_time_earliest, start_time_latest))
        relative_event_duration = int(random.uniform(event_duration_smallest, event_duration_largest))
        
        event_start_time = (i * mean_event_gen_time) + relative_event_start_time
        event_end_time = event_start_time + relative_event_duration
        
        if is_emergency_event:
            emergency_radius = int(random.uniform(emergency_radius_smallest, emergency_radius_largest))
            out_file.write("%s %d %d 0 %dm %d %d 100p\n" % (emergency_event_keyword, event_loc_x, event_loc_y, emergency_radius, event_start_time, event_end_time))
            num_emergency_events_inserted += 1
        else:
            rnd_index = random.randint(0, (len(event_keyword_combination_list) - 1))
            assigned_popularity = int(random.uniform(assigned_popularity_smallest, assigned_popularity_largest))
            assigned_popularity = int(math.ceil(assigned_popularity / 25.0)) * 25
            out_file.write("%s %d %d 0 %d %d %dp\n" % (event_keyword_combination_list[rnd_index], event_loc_x, event_loc_y, event_start_time, event_end_time, assigned_popularity))
    out_file.close()
    
    # build list of keywords assignable to nodes
    keyword_list = possible_keywords.split(",")
    for i in range(0, len(keyword_list)):
        keyword_list[i] = keyword_list[i].strip()
    
    # create keyword combinations for nodes
    out_file = open(properties_file, "w")
    for i in range(0, assignable_keyword_sets):
        keyword_count = random.randint(minimum_keywords_assigned, maximum_keywords_assigned)
        keyword_str = ""
        for j in range(0, keyword_count):
            keyword_index = random.randint(0, (len(keyword_list) - 1))
            if keyword_list[keyword_index] not in keyword_str:
                keyword_str += (keyword_list[keyword_index] + " ")
        out_file.write("%s \n" % (keyword_str))
    out_file.close()
    
    return True

if __name__ == "__main__":
    read_parameters(sys.argv)
    create_files()
    print("Files successfully created.")
    print("")
