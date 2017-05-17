#ifndef LIMOSIM_VEHICLEPARSER_H
#define LIMOSIM_VEHICLEPARSER_H

#include "parser.h"
#include "core/map/map.h"

namespace LIMOSIM
{

class VehicleParser : public Parser
{
public:
    VehicleParser();

    void loadScenario(DOMElement *_dom);

private:
    void parseCar(DOMElement *_dom);
    void parseCarTag(const std::string &_key, const Variant &_value, Car *_car);
    std::vector<Node*> parsePath(DOMElement *_dom);

private:
    Map *p_map;
};

}

#endif // VEHICLEPARSER_H
