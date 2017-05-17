#ifndef LIMOSIM_FOLLOWERMODEL_H
#define LIMOSIM_FOLLOWERMODEL_H

#include "core/vehicles/car.h"

namespace LIMOSIM
{

class FollowerModel
{
public:
    FollowerModel(Car *_car, const std::string &_type);

    virtual double computeAcceleration(Car *_car) = 0;

    std::string getType();


protected:
    Car *p_car;
    std::string m_type;
};

}


#endif // LIMOSIM_FOLLOWERMODEL_H
