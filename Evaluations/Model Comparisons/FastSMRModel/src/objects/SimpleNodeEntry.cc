//
// Copyright (C) 2006 Institut fuer Telematik, Universitaet Karlsruhe (TH)
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
 * @file SimpleNodeEntry.cc
 * @author Bernhard Heep
 */

#include <sstream>
//#include <cassert>

#include "SimpleNodeEntry.h"

uint8_t NodeRecord::dim;

NodeRecord::NodeRecord() {
    coords = new double[dim];
}

NodeRecord::~NodeRecord() {
    delete[] coords;
    coords = NULL;
}

NodeRecord::NodeRecord(const NodeRecord& nodeRecord) {
    coords = new double[dim];
    for (uint32_t i = 0; i < dim; ++i)
        coords[i] = nodeRecord.coords[i];
}

NodeRecord& NodeRecord::operator=(const NodeRecord& nodeRecord) {
    delete[] coords;
    coords = new double[dim];
    for (uint32_t i = 0; i < dim; ++i)
        coords[i] = nodeRecord.coords[i];

    return *this;
}

// beginning of behaviour code
bool NodeRecord::operator==(const BaseLocation& rhs) {
    const NodeRecord* temp = dynamic_cast<const NodeRecord*>(&rhs);

    if (this->getDim() != temp->getDim()) {
        return false;
    }

    bool ret = true;

    for (int i = 0; i < this->getDim(); i++) {
        if (abs(this->coords[i] - temp->coords[i]) > 0.00000001) {
            ret = false;
        }
    }
    return ret;
}

// end of behaviour code

SimpleNodeEntry::SimpleNodeEntry(cModule* node) {

    nodeRecord = new NodeRecord;
    index = -1;

    UdpIPv4ingate = node->gateHalf("udpgate", cGate::INPUT);
    TcpIPv4ingate = node->gateHalf("tcpgate", cGate::INPUT);
}

float SimpleNodeEntry::operator-(const SimpleNodeEntry& entry) const {
    double sum_of_squares = 0;
    for (uint32_t i = 0; i < nodeRecord->dim; i++) {
        sum_of_squares += pow(
                nodeRecord->coords[i] - entry.nodeRecord->coords[i], 2);
    }
    return sqrt(sum_of_squares);
}

std::string SimpleNodeEntry::info() const {
    std::ostringstream str;
    str << *this;
    return str.str();
}

std::ostream& operator<<(std::ostream& out, const SimpleNodeEntry& entry) {
    out << "(";
    for (uint8_t i = 0; i < NodeRecord::dim; i++) {
        out << "dim" << i << ": " << entry.nodeRecord->coords[i];
        out << ", ";
    }
    return out;
}
