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
        auto k = it->second->getEventArraySize();
        for (size_t i=0; i<k; i++) {
            auto event = it->second->getEventForUpdate(i);
            if (event != nullptr)
                delete event;
        }
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

    simtime_t timestamp = _event->getTimestamp();
    auto it = m_events.find(timestamp);
    if (it == m_events.end()) {
        auto event = new LIMoEvent();
        event->insertEvent(_event);
        m_events[timestamp] = event;
        scheduleAt(timestamp, event);
    }
    else {
        it->second->insertEvent(_event);
    }
}

void LIMoSimController::cancelEvent(LIMoSim::Event *_event)
{
    auto it = m_events.find(_event->getTimestamp());
    if(it != m_events.end())
    {
        auto k = it->second->getEventArraySize();
        for (size_t i=0; i<k; i++) {
            if (it->second->getEvent(i) == _event) {
                it->second->eraseEvent(i);
                delete _event;
                _event = nullptr;
                break;
            }
        }
        if (it->second->getEventArraySize() == 0) {
            cancelAndDelete(it->second);
            m_events.erase(it);
        }
    }
    if (_event != nullptr)
        throw cRuntimeError("Model error: event not in the map: %s", _event->getInfo().c_str());
}

void LIMoSimController::deleteEvent(LIMoSim::Event *_event)
{
    cancelEvent(_event);
}

void LIMoSimController::handleMessage(cMessage *_message)
{
    if(_message->isSelfMessage())
    {
        auto _event = check_and_cast<LIMoEvent*>(_message);
        while (_event->getEventArraySize() > 0) {
            LIMoSim::Event *event = _event->getEventForUpdate(0);
            _event->eraseEvent(0);
            if (event) {
                if(simtime_t(event->getTimestamp()) == _message->getArrivalTime())
                    event->handle();
                else
                    throw cRuntimeError("Model error: event not in the map: %s", event->getInfo().c_str());
            }
        }
        m_events.erase(_message->getArrivalTime());
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
