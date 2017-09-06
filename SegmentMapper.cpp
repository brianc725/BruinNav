#include "provided.h"
#include "MyMap.h"
//#include "support.h"
#include <vector>
using namespace std;

class SegmentMapperImpl
{
public:
    SegmentMapperImpl();
    ~SegmentMapperImpl();
    void init(const MapLoader& ml);
    vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
    vector<StreetSegment> streets;
    MyMap<GeoCoord, vector<StreetSegment>> coordStreet;
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
    for (int i = 0; i < ml.getNumSegments(); i++)
    {
        StreetSegment segmentHere;
        ml.getSegment(i, segmentHere);  //gets the segment at location
        GeoCoord startCoord = segmentHere.segment.start;    //start geocoordinate
        GeoCoord endCoord = segmentHere.segment.end;    //end geocoordinate

        vector<StreetSegment> *streetPoint1;
        streetPoint1 = coordStreet.find(startCoord); //finds the starting coordinate and points to it
        if (streetPoint1 == nullptr)    //if there is no vector at that geocoordinate in the map yet
        {
            coordStreet.associate(startCoord, streets); //create a new vector associated with that coordinate
            coordStreet.find(startCoord)->push_back(segmentHere);   //push back that segment to the coordinate
        }
        else
            streetPoint1->push_back(segmentHere);//otherwise just push back that segment to the geocoord already there

        string attractLat;
        string attractLong;
        for (int j = 0; j < segmentHere.attractions.size(); j++)    //if theres an attraction there
        {
            vector<StreetSegment> *streetPoint2;
            attractLat = segmentHere.attractions[j].geocoordinates.latitudeText;    //get the latitude of attract
            attractLong = segmentHere.attractions[j].geocoordinates.longitudeText;  //get the long of attract
            GeoCoord attractGeo = GeoCoord(attractLat, attractLong);    //create a geocoord with the lat and long
            streetPoint2 = coordStreet.find(attractGeo);    //find that geocoordinate in the map
            if(streetPoint2 == nullptr) //if not already created
            {
                coordStreet.associate(attractGeo, streets); //associate a vector with that geocoord
                coordStreet.find(attractGeo)->push_back(segmentHere);   //push back that segment to the corodinate
            }
            else
                streetPoint2->push_back(segmentHere);//otherwise just push back that segment to the geocoord already there
        }
        
        vector<StreetSegment> *streetPoint3;
        streetPoint3 = coordStreet.find(endCoord);  //finds the ending coordinate and points to it
        if(streetPoint3 == nullptr) //if there is no vector at that geocoordinate in the map yet
        {
            coordStreet.associate(endCoord, streets);   //create a new vector associated with that coordinate
            coordStreet.find(endCoord)->push_back(segmentHere); //push back that segment to the coordinate
        }
        else
            streetPoint3->push_back(segmentHere);   //otherwise just push back that segment to the geocoord already there
    }
    
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
    vector<StreetSegment> segments;
    if(coordStreet.find(gc) == nullptr)
    {
        return segments;    //if that geo coordinate does not exist, return the empty segment vector
    }
   else
       segments = *coordStreet.find(gc); //find returns a pointer to the vector, we dereference it
   
    return segments;    //return the segments vector thats loaded
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
    m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
    delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
    m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
    return m_impl->getSegments(gc);
}
