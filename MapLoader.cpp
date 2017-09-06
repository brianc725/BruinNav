#include "provided.h"
//#include "support.h"
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
using namespace std;

class MapLoaderImpl
{
public:
    MapLoaderImpl();
    ~MapLoaderImpl();
    bool load(string mapFile);
    size_t getNumSegments() const;
    bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
    vector<StreetSegment> streets;
    int m_numSegs;
};

MapLoaderImpl::MapLoaderImpl()
{
    m_numSegs = 0;
}

MapLoaderImpl::~MapLoaderImpl()
{
    m_numSegs = 0;
}

bool MapLoaderImpl::load(string mapFile)
{
    ifstream infile(mapFile);   //reads in file
    if(!infile)
    {
        return false;   //if file doesn't open return false
    }
    string name = "";
    while(getline(infile, name))    //while there are still lines
    {
        StreetSegment a;
        a.streetName = name;    //assign the name from getline
        string coord;
        int elements;
        getline(infile, coord); //get the next line and store in the coordinates string
        string lat1 = "";
        string long1 = "";
        string lat2 = "";
        string long2 = "";
        int index1 = 0;
        int index2 = 0;
        int index3 = 0;
        int index4 = 0;
        int index5 = 0;
        int index6 = 0;
        for (int i = 0; i < coord.size(); i++)
        {
            if ((!isdigit(coord[i])&&coord[i]!='.'))
            {
                index1 = i;     //get the index where the first latitude ends
                break;
            }
        }
        for (int i = 0; i < index1; i++)
        {
            lat1 += coord[i];   //append the latitude to lat1
        }
        
        for (int i = index1; i < coord.size(); i++)
        {
            if((coord[i] != ' ')&&(coord[i] != ','))
            {
                index2 = i; //get the index where long1 begins
                break;
            }
        }
        for (int i = index2; i < coord.size(); i++)
        {
            if (!isdigit(coord[i])&&coord[i]!='.'&&coord[i]!='-')
            {
                index3 = i; //get index where long2 ends
                break;
            }
        }
        
        for (int i = index2; i < index3; i++)
        {
            long1 += coord[i];  //append long to long1
        }

        for (int i = index3; i < coord.size(); i++)
        {
            if(isdigit(coord[i])||(coord[i] == '-'))
            {
                index4 = i;     //get index where lat2 begins
                break;
            }
        }
        
        for (int i = index4; i < coord.size(); i++)
        {
            if (!isdigit(coord[i])&&coord[i]!='.'&&coord[i]!='-')
            {
                index5 = i; //get index where lat 2 ends
                break;
            }
        }
        for(int i = index4; i < index5; i++)
        {
            lat2 += coord[i];   //append lat to lat2
        }
        
        for(int i = index5; i < coord.size(); i++)
        {
            if((coord[i] != ' ')&&(coord[i] != ','))
            {
                index6 = i; //get index where long2 begins
                break;
            }
        }
        for (int i = index6; i < coord.size(); i++)
        {
            long2 += coord[i];  //append long to long 2
        }
        
        GeoCoord start = GeoCoord(lat1, long1); //assign the proper values
        GeoCoord end = GeoCoord(lat2, long2);
        a.segment = GeoSegment(start, end); //assign the segment from the gathered data
        infile >> elements;
        infile.ignore(10000, '\n'); //stop reading endl following getline of numbers
        vector<Attraction> attractions;
        while (elements != 0)       //if there are attractions
        {
            string actName;
            getline(infile, actName);   //get the attraction name
            Attraction b;
            int index = 0;
            
            for (int i = 0; i < actName.size(); i++)
            {
                if(actName[i] == '|')
                    index = i;
            }
            string name = actName.substr(0, index); //name is up to the |
            string restOfIt = "";
            for (int i = index+1; i < actName.size(); i++)
            {
                restOfIt += actName[i];
            }
            
            b.name = name;
            string lat = "";
            string longt = "";
            int index11 = 0;
            int index12 = 0;

            for (int i = 0; i < restOfIt.size(); i++)
            {
                if ((!isdigit(restOfIt[i])&&restOfIt[i]!='.'))
                {
                    index11 = i;     //get the index where the first latitude ends
                    break;
                }
            }
            for (int i = 0; i < index11; i++)
            {
                lat += restOfIt[i];   //append the latitude to lat
            }

            for(int i = index1; i < restOfIt.size(); i++)
            {
                if(isdigit(restOfIt[i])||(restOfIt[i] == '-'))
                {
                    index12 = i; //get index where long2 begins
                    break;
                }
            }
            for (int i = index12; i < restOfIt.size(); i++)
            {
                longt += restOfIt[i];  //append long to long 2
            }
            
            GeoCoord coords = GeoCoord(lat, longt);
            b.geocoordinates = coords;  //push the coordinates onto the attraction vector
            attractions.push_back(b);
            elements--;
        }
        a.attractions = attractions;    //assign the attractions to the street segment
        streets.push_back(a);   //assign this whole streetsegment to the vector of streets
    }
    
    return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
    return streets.size();  //returns the size of the vector
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
    if ((segNum < 0)||(segNum > getNumSegments()-1))
    {
        return false;   //if out of bounds return false
    }
    
    seg = streets[segNum];  //assigns seg to the correct data from the streets vector
    return true;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
    m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
    delete m_impl;
}

bool MapLoader::load(string mapFile)
{
    return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
    return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
    return m_impl->getSegment(segNum, seg);
}
