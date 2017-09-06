//
//  support.h
//  Project4
//
//  Created by Brian Chan on 3/13/17.
//  Copyright © 2017 Brian Chan. All rights reserved.
//

#ifndef support_h
#define support_h

#include "provided.h"

bool operator<(const GeoCoord &a, const GeoCoord &b);
bool operator>(const GeoCoord &a, const GeoCoord &b);
bool operator ==(const GeoCoord &a, const GeoCoord &b);


std::string getAngle(const GeoSegment &a); //gets the angle of a geosegment

#endif /* support_h */
