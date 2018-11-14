//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
// Copyright (C) 2004 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

/**
 * @file SimpleNodeEntry.h
 * @author Bernhard Heep
 * @date 2006/11/30
 */

#ifndef __SIMPLENODEENTRY_H
#define __SIMPLENODEENTRY_H

#include <omnetpp.h>
#include "BaseLocation.h"

using namespace omnetpp;

class NodeRecord: public BaseLocation
{
  public:
    //NodeRecord(uint32_t dim);
    NodeRecord();
    ~NodeRecord();
    NodeRecord(const NodeRecord& nodeRecord);
    NodeRecord& operator=(const NodeRecord& nodeRecord);
    bool operator==(const BaseLocation& rhs);
    void debugOutput(int dim);

    double* coords;
    //IPvXAddress ip;
    static uint8_t dim;
    static void setDim(uint8_t dimension) { dim = dimension; };
    uint8_t getDim() const { return dim; };
};

/**
 * representation of a single node in the GlobalNodeList
 *
 * @author Bernhard Heep
 */
class SimpleNodeEntry : public cObject
{
public:

    ~SimpleNodeEntry()
    {
        if (index == -1) delete nodeRecord;
    }

    /**
     * Simple constructor
     */
    SimpleNodeEntry(cModule* node);

    /**
     * Getter for SimpleUDP ingate
     *
     * @return the ingate
     */
    inline cGate* getUdpIPv4Gate() const
    {
        return UdpIPv4ingate;
    };

    /**
     * Getter for SimpleUDP ingate
     *
     * @return the ingate
     */
    inline cGate* getTcpIPv4Gate() const
    {
        return TcpIPv4ingate;
    };

    /**
     * OMNeT++ info method
     *
     * @return infostring
     */
    std::string info() const;

    /**
     * Stream output
     *
     * @param out output stream
     * @param entry the terminal
     * @return reference to stream out
     */
    friend std::ostream& operator<<(std::ostream& out, const SimpleNodeEntry& entry);

    inline void setX(float x) { nodeRecord->coords[0] = x; };
    inline void setY(float y) { nodeRecord->coords[1] = y; };
    inline float getX() const { return nodeRecord->coords[0]; };
    inline float getY() const { return nodeRecord->coords[1]; };
    inline float getCoords(int dim) const { return nodeRecord->coords[dim]; };
    inline uint8_t getDim() const { return nodeRecord->getDim(); };

    int getRecordIndex() const { return index; };
    NodeRecord* getNodeRecord() const { return nodeRecord; };



protected:

    /**
     * Calculates euclidean distance between two terminals
     *
     * @param entry destination entry
     * @return the euclidean distance
     */
    float operator-(const SimpleNodeEntry& entry) const;

    cGate* UdpIPv4ingate; //!< IPv4 ingate of the SimpleUDP module of this terminal
    cGate* TcpIPv4ingate; //!< IPv4 ingate of the SimpleTCP module of this terminal

    NodeRecord* nodeRecord;
    int index;
};


#endif // __SIMPLENODEENTRY_H
