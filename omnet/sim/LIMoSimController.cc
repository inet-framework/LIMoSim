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


namespace LIMoSimInet {

LIMoSimController *eventSchedulerInstance = 0;

Define_Module(LIMoSimController);

LIMoSimController::LIMoSimController()
{

}

LIMoSimController::~LIMoSimController()
{
    for(auto& elem: m_events) {
        auto& limoEvents = elem.second->getEventsForUpdate();
        for (auto event : limoEvents)
            delete event;
    }
}

void LIMoSimController::initialize()
{
    std::string mapFile = par("map").stringValue();
    m_geographicCoordinateSystemModule = inet::getModuleFromPar<inet::IGeographicCoordinateSystem>(par("geographicCoordinateSystemModule"), this);
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
        event->getEventsForUpdate().reserve(100);      // set initial capacity
        event->getEventsForUpdate().push_back(_event);
        m_events[timestamp] = event;
        scheduleAt(timestamp, event);
    }
    else {
        it->second->getEventsForUpdate().push_back(_event);
    }
}

void LIMoSimController::cancelEvent(LIMoSim::Event *_event)
{
    auto it = m_events.find(_event->getTimestamp());
    if(it != m_events.end())
    {
        auto& events = it->second->getEventsForUpdate();
        for (auto it = events.begin(); it != events.end(); ++it) {
            if (*it == _event) {
                delete _event;
                events.erase(it);
                _event = nullptr;
                break;
            }
        }
        if (events.empty()) {
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
        auto& events = _event->getEventsForUpdate();
        for (auto event: events) {
            if(simtime_t(event->getTimestamp()) == _message->getArrivalTime())
                event->handle();
            else
                throw cRuntimeError("Model error: event not in the map: %s", event->getInfo().c_str());
        }
        m_events.erase(_message->getArrivalTime());
        delete _message;
    }
}

void LIMoSimController::setOrigin(const LIMoSim::Position &_origin)
{
    auto inetOrigin = m_geographicCoordinateSystemModule->getPlaygroundPosition();
    if ((inetOrigin.longitude != inet::deg(_origin.x)) || (inetOrigin.latitude != inet::deg(_origin.y)))
        throw cRuntimeError("LIMoSIM origin (%g,%g) and INET origin (%g,%g) are differ", _origin.y, _origin.x, inetOrigin.latitude.get(), inetOrigin.longitude.get());
}

LIMoSim::Vector3d LIMoSimController::getOffset(const LIMoSim::Position &_node) const
{
    inet::GeoCoord gc(inet::deg(_node.y), inet::deg(_node.x), inet::m(0));
    auto offs = m_geographicCoordinateSystemModule->computePlaygroundCoordinate(gc);
    return LIMoSim::Vector3d(offs.x, offs.y, offs.z);
}

} //namespace
