//
// This program is property of its copyright holder. All rights reserved.
//

import inet.common.INETDefs;

cplusplus {{
#include "LIMoSim/sim/event.h"
}}

namespace LIMoSim;

cplusplus {{
typedef std::vector<Event *> EventList;
inline std::ostream& operator<<(std::ostream& out,const LIMoSim::EventList) {return out;}
}}

class EventList {
    @existingClass;
    @descriptor(readonly);
}

namespace inet;

class LIMoEvent extends cMessage
{
    LIMoSim::EventList events;
}

