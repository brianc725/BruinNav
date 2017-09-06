//
//  support.cpp
//  Project4
//
//  Created by Brian Chan on 3/13/17.
//  Copyright © 2017 Brian Chan. All rights reserved.
//

#include "support.h"
#include <string>
using namespace std;


bool operator<(const GeoCoord &a, const GeoCoord &b)
{
    if(a.latitude < b.latitude)
        return true;
    else if (a.latitude > b.latitude)
        return false;
    else if (a.longitude < b.longitude)
        return true;
    else
        return false;
}

bool operator>(const GeoCoord &a, const GeoCoord &b)
{
    if(a.latitude > b.latitude)
        return true;
    else if (a.latitude < b.latitude)
        return false;
    else if (a.longitude > b.longitude)
        return true;
    else
        return false;}

bool operator==(const GeoCoord &a, const GeoCoord &b)
{
    if ((a.latitude == b.latitude)&&(a.longitude == b.longitude))
    {
        return true;
    }
    return false;

}
string getAngle(const GeoSegment &a)
{
    double ang = angleOfLine(a); //uses angle of line to find the angle of a geosegment
    if ((ang >= 0)&&(ang <= 22.5))
        return "east";
    else if (ang <= 67.5)
        return "northeast";
    else if (ang <= 112.5)
        return "north";
    else if (ang <= 157.5)
        return "northwest";
    else if(ang <= 202.5)
        return "west";
    else if(ang <= 247.5)
        return "southwest";
    else if(ang <= 292.5)
        return "south";
    else if(ang <= 337.5)
        return "southeast";
    else if(ang < 360)
        return "east";
    return "east";
}

