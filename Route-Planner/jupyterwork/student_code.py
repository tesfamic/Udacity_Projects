##########
# The following is the link where I got additional information about A* to complete the project.
# Reference: https://www.redblobgames.com/pathfinding/a-star/introduction.html
##########
import math
import queue as Q
def get_cost(state1, state2):
    ''' calculates the euclidean distance
        between the two input states/coordinates 
    '''
    dx = state1[0] - state2[0]
    dy = state1[1] - state2[1]
    cost = math.sqrt(dx**2 + dy**2)
    
    return cost

def shortest_path(M,start,goal):
    print("shortest path called")
    # variable to collect the path
    short_path =[]
    
    # calculate the heuristic distance(h) between the start and the goal
    # for initialization of the frontier. path cost(g) = 0
    init_cost = get_cost(M.intersections[start],M.intersections[goal])
    
    # states with estimated costs. Tuple is used in the queue
    frontiers = Q.PriorityQueue()    
    frontiers.put((init_cost, start))
    
    # parent_tree keeps track of how the paths are expanded based on which
    # minimum cost path is connected
    parent_tree = {}
    parent_tree[start] = 0 
    
    # temporary storage of expanded paths from parent/current state
    next_states = []
 
    # collects the path costs
    path_costs = {}
    path_costs[start] = 0  #start with 0 path coast for the starting point
        
    goal_found = (start == goal)   
    
    # iterate until all frontiers are checked. If the goal is reached before all
    # the frontiers are examined, the loop stops.
    while not frontiers.empty():       
        # get the current state.
        #   --> the first value of the tuple is the estimated distance and 
        #   --> the second value is the state
        state_val = frontiers.get()
        curr_state = state_val[1] 

        if curr_state == goal:
            goal_found = True
            #print('goal found')
            break
        
        # expand the path
        next_states = M.roads[curr_state] 
        for next_state in next_states: 
            # if next_state is not a member of next_states, it's already explored.
            # I don't need to save the explored states.
            
            # prepare the states to calculate cost
            curr_intersec = M.intersections[curr_state]
            next_intersec = M.intersections[next_state]
            goal_intersec = M.intersections[goal]
            
            # compute the path cost
            g = get_cost(curr_intersec, next_intersec) + path_costs[curr_state]            
            if next_state not in path_costs or g < path_costs[next_state]:
                
                # calculate the estimated path cost (heuristic function: euclidean distance)
                h = get_cost(next_intersec, goal_intersec)
                
                # total estimated cost
                f = g + h
                
                # put the next state in frontiers dictionary
                frontiers.put((f, next_state))
                
                # add/update the g_costs dictionary
                path_costs[next_state] = g
                parent_tree[next_state] = curr_state
    
    # At this point the goal is assumed to be reached. So, construct the path information 
    # using the parent_tree dictionary starting from the goal point to the starting point. 
    curr_state = goal
    while curr_state != start:
        # inserts the current point at the top of the list
        short_path.insert(0, curr_state) 
        curr_state = parent_tree[curr_state]
    short_path.insert(0, start)    
    #print('sh paths', short_path)
    
    return short_path
   
