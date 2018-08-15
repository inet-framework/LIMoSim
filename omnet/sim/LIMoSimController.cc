//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "LIMoSimController.h"

#include "inet/common/ModuleAccess.h"
#include "inet/common/geometry/common/GeographicCoordinateSystem.h"

#include "LIMoSim/sim/simulation.h"


namespace inet {

LIMoSimController *eventSchedulerInstance = 0;

Define_Module(LIMoSimController);

LIMoSimController::LIMoSimController()
{

}

LIMoSimController::~LIMoSimController()
{
    for(auto it=m_events.begin(); it!=m_events.end(); it++)
    {
        deleteEvent(it->first);
    }
}

void LIMoSimController::initialize()
{
    std::string mapFile = par("map").stringValue();
    m_geographicCoordinateSystemModule = getModuleFromPar<IGeographicCoordinateSystem>(par("geographicCoordinateSystemModule"), this);
    LIMoSim::Simulation *sim = LIMoSim::Simulation::getInstance(this);
    sim->load(mapFile, "", *this);
}

void LIMoSimController::scheduleEvent(LIMoSim::Event *_event)
{
    Enter_Method("scheduleEvent");

    cMessage *event = new cMessage(_event->getInfo().c_str());
    event->setContextPointer(_event);
    m_events[_event] = event;

    scheduleAt(_event->getTimestamp(), event);
}

void LIMoSimController::cancelEvent(LIMoSim::Event *_event)
{
    auto it = m_events.find(_event);
    if(it != m_events.end())
    {
        cancelAndDelete(it->second);

        m_events.erase(it);
        delete _event;
    }
}

void LIMoSimController::deleteEvent(LIMoSim::Event *_event)
{
    cancelEvent(_event);
}

void LIMoSimController::handleMessage(cMessage *_message)
{
    if(_message->isSelfMessage())
    {
        LIMoSim::Event *event = (LIMoSim::Event *)_message->getContextPointer();
        if (event) {
            auto it = m_events.find(event);
            if(it != m_events.end() && it->second == _message)
            {
                m_events.erase(it);
                event->handle();

                //
            }
            else
                throw cRuntimeError("Module error: event not in the map: %s", event->getInfo().c_str());
        }
        delete _message;
    }
}

void LIMoSimController::setOrigin(const LIMoSim::Position &_origin)
{
    auto inetOrigin = m_geographicCoordinateSystemModule->getPlaygroundPosition();
    if ((inetOrigin.longitude != deg(_origin.x)) || (inetOrigin.latitude != deg(_origin.y)))
        throw cRuntimeError("LIMoSIM origin (%g,%g) and INET origin (%g,%g) are differ", _origin.y, _origin.x, inetOrigin.latitude.get(), inetOrigin.longitude.get());
}

LIMoSim::Vector3d LIMoSimController::getOffset(const LIMoSim::Position &_node) const
{
    GeoCoord gc(deg(_node.y), deg(_node.x), m(0));
    auto offs = m_geographicCoordinateSystemModule->computePlaygroundCoordinate(gc);
    return LIMoSim::Vector3d(offs.x, offs.y, offs.z);
}

} //namespace
