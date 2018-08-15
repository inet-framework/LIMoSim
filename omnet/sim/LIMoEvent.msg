//
// This program is property of its copyright holder. All rights reserved.
//

import inet.common.INETDefs;

cplusplus {{
#include "LIMoSim/sim/event.h"
}}

namespace LIMoSim;

class Event {
    @existingClass;
    @descriptor(readonly);
    double timestamp;
}

cplusplus {{
inline std::ostream& operator<<(std::ostream& out,const LIMoSim::Event*) {return out;}
}}

namespace inet;

class LIMoEvent extends cMessage
{
    LIMoSim::Event *event;
}

