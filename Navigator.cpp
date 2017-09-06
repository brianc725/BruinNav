#include "provided.h"
#include "MyMap.h"
#include "support.h"
#include <string>
#include <vector>
#include <algorithm>

#include <iostream>

using namespace std;

class NavigatorImpl
{
public:
    NavigatorImpl();
    ~NavigatorImpl();
    bool loadMapData(string mapFile);
    NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
    SegmentMapper segMap;
    AttractionMapper attMap;
    vector<GeoCoord> Astar(const GeoCoord& start, const GeoCoord& end) const;
    vector<GeoCoord> reconstruct(const MyMap<GeoCoord, GeoCoord> &cameFrom, GeoCoord& current) const;
    GeoCoord lowestF(const MyMap <GeoCoord, double> &fScore, const vector <GeoCoord> open) const;
};

NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

bool NavigatorImpl::loadMapData(string mapFile)
{
    MapLoader mapLoad;
    if(mapLoad.load(mapFile) == false)
        return false;
    segMap.init(mapLoad);
    attMap.init(mapLoad);
    
    return true;
}

//get node in open with lowest fscore value
GeoCoord NavigatorImpl::lowestF(const MyMap <GeoCoord, double> &fScore, const vector <GeoCoord> open) const
{
    GeoCoord lowest = open[open.size()-1];  //set lowest to anything in case of error so undeclared var isnt returned
    double minimum = INFINITY; //start with maximum double size
    for (int i = 0; i < open.size(); i++)
    {
        GeoCoord tempLow = open[i];
        double tempMin = *fScore.find(tempLow);
        if (tempMin < minimum)
        {
            lowest = tempLow;
            minimum = tempMin;
        }
    }
    return lowest;
}

vector<GeoCoord> NavigatorImpl::reconstruct(const MyMap<GeoCoord, GeoCoord> &cameFrom, GeoCoord& current) const
{
    vector <GeoCoord> totalPath;
    totalPath.push_back(current);
    const GeoCoord *from = cameFrom.find(current);
    while (from != nullptr)
    {
        current = *from;
        totalPath.push_back(current);
        from = cameFrom.find(current);
    }
    return totalPath;
}

//A* algorithm via pseudocode from online
vector<GeoCoord> NavigatorImpl::Astar(const GeoCoord& start, const GeoCoord& end) const
{
    vector <GeoCoord> closed; //the set of nodes already evaluated
    vector <GeoCoord> open; //set of discovered nodes not evaluated yet
    vector <StreetSegment> neighbor;
    vector <GeoCoord> neighbors;
    vector <GeoCoord> failure;
    vector <GeoCoord> solution;
    
    open.push_back(start);  //initially, only the start node is known
    
    MyMap <GeoCoord, GeoCoord> cameFrom; //for every node, which node it can most efficiently be reached from
    MyMap <GeoCoord, double> gScore; //cost of getting from start node to current node
    gScore.associate(start, 0); //going from start to start is 0
    
    MyMap <GeoCoord, double> fScore; //cost of start to goal directly
    double directCost = distanceEarthMiles(start, end); //get the direct ruler drawn distance from start to end
    fScore.associate(start, directCost);
    
    GeoCoord current;
    
    while (open.empty() == false) //while open set is not empty
    {
        //current = node in openset having lowest fscore value
        current = lowestF(fScore, open);
        
        if (current == end)
        {
            solution = reconstruct(cameFrom, current);
            return solution;
        }
        
        //remove from open set of current
        open.erase(remove(open.begin(), open.end(), current));
        closed.push_back(current); //add to closed current
        
        //load the neighbors in
        neighbor = segMap.getSegments(current); //gets the streets that neighbor the current segment we are on
        for (int i = 0; i < neighbor.size(); i++) //for each neighbor of current
        {
            //check attractions here
            for (int j = 0; j < neighbor[i].attractions.size(); j++)
                 {
                     if (neighbor[i].attractions[j].geocoordinates == end)
                     {
                         neighbors.push_back(neighbor[i].attractions[j].geocoordinates); //add the last segment if the attraction is on it
                     }
                 }
            //check attractions here
            if (neighbor[i].segment.start == current)
            {
                neighbors.push_back(neighbor[i].segment.end);   //if we are at start, neighbor is at the end
            }
            else if (neighbor[i].segment.end == current)
            {
                neighbors.push_back(neighbor[i].segment.start); //if we are at end, neighbor is at beginning
            }
            else
            {
                neighbors.push_back(neighbor[i].segment.end);
                neighbors.push_back(neighbor[i].segment.start); //we are on the segment, so begin and end are neighbors
            }
        }
        neighbor.clear(); //reset for next time
 
        for (int i = 0; i < neighbors.size(); i++)//iterate through all neighbor coords
        {
            vector<GeoCoord>::iterator GeoIt;
            double tempGScore;
            GeoIt = find(closed.begin(), closed.end(), neighbors[i]);
            
            if (GeoIt != closed.end())//if the coord has already been evaluated, skip it
                continue;
            if (gScore.find(current) == nullptr)//if the current has no gscore, we don't need it so skip
                continue;
            else
            {
                tempGScore = *gScore.find(current) + distanceEarthMiles(current, neighbors[i]);
            }
            
            GeoIt = find(open.begin(), open.end(), neighbors[i]);
            if (GeoIt == open.end())//if the neighbor isn't in the openset already, add it
            {
                open.push_back(neighbors[i]);
            }
            else if (tempGScore >= *gScore.find(neighbors[i])) //if this gscore is even bigger than the other ones, skip
                continue;//this isn't the optimal path
            
            cameFrom.associate(neighbors[i], current);//update the camefrom map
            gScore.associate(neighbors[i], tempGScore);//update the gscore
            fScore.associate(neighbors[i], tempGScore + distanceEarthMiles(neighbors[i], end));//update the fscore

        }
        neighbors.clear(); //reset neighbors vector for next run
    }
    return failure; // pass empty vector if it failed
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
    vector <GeoCoord> forwards;
    vector <GeoCoord> backwards;
    
    directions.clear(); //in case anything was here previously
    
    GeoCoord fillMeStart;
    bool foundAttractionStart = attMap.getGeoCoord(start, fillMeStart);
    if (!foundAttractionStart)  //if attraction can't be found at beginning return bad soruce
    {
        return NAV_BAD_SOURCE;
    }
    
    GeoCoord fillMeEnd;
    bool foundAttractionEnd = attMap.getGeoCoord(end, fillMeEnd);
    if (!foundAttractionEnd)    //if attraction can't be found at end return bad destination
    {
        return NAV_BAD_DESTINATION;
    }
    
    backwards = Astar(fillMeStart, fillMeEnd); //call astar and get the vector back
    if (backwards.size() ==  0)
    {
        return NAV_NO_ROUTE;    //if empty vector was returned, the astar failed to find a route
    }
    else
    {
        GeoSegment current;
        GeoSegment before;
        string streetCurrent;
        string streetBefore;
        
        /*
         // constructor for a Proceed NavSegment
         NavSegment(std::string direction, std::string streetName, double distance, const GeoSegment& gs)
         : m_command(PROCEED), m_direction(direction), m_streetName(streetName), m_distance(distance), m_geoSegment(gs)
         {}
         
         // constructor for a Turn NavSegment
         NavSegment(std::string direction, std::string streetName)
         : m_command(TURN), m_direction(direction), m_streetName(streetName)
         {}
         
         NavCommand	m_command;	    // PROCEED or TURN
         std::string	m_direction;	// e.g., "north" for proceed or "left" for turn
         std::string	m_streetName;	// e.g., Westwood Blvd
         double		m_distance;		// for proceed, distance in miles
         GeoSegment	m_geoSegment;
         */
        
        //reverse the vector back again and fill details
        for (int i = backwards.size()-1; i >= 0; i--)
        {
            forwards.push_back(backwards[i]); //pushes in from back to front which will place directions in correct order
        }
        for (int i = 0; i < forwards.size()-1 ; i++)
        {
            vector <StreetSegment> first = segMap.getSegments(forwards[i]);     //get street segments that have the geo
            vector <StreetSegment> second = segMap.getSegments(forwards[i+1]);  //coord on it
            
            for (int j = 0; j< first.size(); j++)
            {
                for (int k = 0; k < second.size(); k++)
                {
                    if ((first[j].segment.start == second[k].segment.start) && (first[j].segment.end == second[k].segment.end) ) //if two segments are the same, then it's an intersection and the segment we are looking for
                    {
                        current = first[j].segment; //set the current street segment to the first one
                        streetCurrent = first[j].streetName;
                    }
                }
            }
            GeoSegment nav;
            if (current.end == forwards[i] || current.start == forwards[i+1])   //flip orientation
            {                                                                           //if line goes from end to start
                nav.start = current.end;
                nav.end = current.start;
            }
            else
            {
                nav = current;
            }
            
            //check first and last segments for special conditions
            if (i == 0)
            {
                before = nav;
                streetBefore = streetCurrent;
                nav.start = fillMeStart;
            }
            if (i == forwards.size()-2)
            {
                nav.end = fillMeEnd;
            }
            
            string direction = getAngle(nav); //get the angle of this geosegment
            if (streetBefore != streetCurrent) //if street names aren't same, we need to turn
            {
                
                /*
                 // constructor for a Turn NavSegment
                 NavSegment(std::string direction, std::string streetName)
                 : m_command(TURN), m_direction(direction), m_streetName(streetName)
                 {}
                */
                
                if (angleBetween2Lines(before, current) < 180)
                {
                    NavSegment left("left", streetCurrent);
                    directions.push_back(left);
                }
                else
                {
                    NavSegment right("right", streetCurrent);
                    directions.push_back(right);
                }
            }
            //otherwise streets are the same so we just proceed
            /*
             // constructor for a Proceed NavSegment
             NavSegment(std::string direction, std::string streetName, double distance, const GeoSegment& gs)
             : m_command(PROCEED), m_direction(direction), m_streetName(streetName), m_distance(distance), m_geoSegment(gs)
             {}
            */
            string dir = getAngle(current);
            double dist = distanceEarthMiles(forwards[i], forwards[i+1]);
            NavSegment proc(dir, streetCurrent, dist, current);
            directions.push_back(proc);
            streetBefore = streetCurrent;
            before = current;
        }
        
           
    }

    
    return NAV_SUCCESS;  // This compiles, but may not be correct
}

//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
    m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
    delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
    return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
    return m_impl->navigate(start, end, directions);
}
 
