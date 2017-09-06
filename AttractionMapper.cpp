#include "provided.h"
//#include "support.h"
#include "MyMap.h"
#include <cctype>
#include <string>
using namespace std;

class AttractionMapperImpl
{
public:
    AttractionMapperImpl();
    ~AttractionMapperImpl();
    void init(const MapLoader& ml);
    bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
    MyMap<string, GeoCoord> attractMap;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
    for(int i = 0; i < ml.getNumSegments(); i++)
    {
        StreetSegment segmentHere;
        ml.getSegment(i, segmentHere);  //gets the first street segment from map loader
        for(int j = 0; j < segmentHere.attractions.size(); j++)
        {
            string normalName = segmentHere.attractions[j].name;    //grabs attraction name at that location
            string lowerName;
            for(int i = 0; i < normalName.size(); i++)
            {
                lowerName += tolower(normalName[i]);    //lowercases the attraction name
            }
            attractMap.associate(lowerName, segmentHere.attractions[j].geocoordinates); //associates the name with the
                                                                                        //coordinates
        }
    }
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
    string attractLow = "";
    for(int i = 0; i < attraction.size(); i++)
    {
        attractLow += tolower(attraction[i]);   //lowercases the searching attraction name
    }

    const GeoCoord *locAttract= attractMap.find(attractLow);    //receives a pointer to the attraction
    if (locAttract == nullptr)      //if the pointer is nullptr(attraction not found) return false
        return false;
    gc = *locAttract;   //otherwise provide the pointer
    
    return true;  // return true if correct
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
    m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
    delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
    m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
    return m_impl->getGeoCoord(attraction, gc);
}
