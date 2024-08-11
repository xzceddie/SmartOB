#ifndef ID_GEN_H
#define ID_GEN_H


#include <memory>

namespace sob {

/**
 *  @brief An ( Singleton ) Id Generator
 *          Prduces self-incrementing Ids, starting from 0
 */
class IdGen
{
private:
    IdGen(){}
    static std::unique_ptr<IdGen> instance;
    mutable int id = 0;

public:
    static IdGen& getInstance()
    {
        if( instance.get() == nullptr ) {
            instance.reset(new IdGen());
        }
        return *instance;
    }

    static int genId()
    {
        return getInstance().id++;
    }

    void reset()
    { 
        id = 0;
    }

}; // struct IDGen
//
/**
 *  @brief An ( Singleton ) Id Generator
 *          Prduces self-decrementing Ids, starting from 0
 *          This is used pose fake orders in the case of guessing the up-to-date orderbook status
 */
class IdGenNeg
{
private:
    IdGenNeg(){}
    static std::unique_ptr<IdGenNeg> instance;
    mutable int id = -1;

public:
    static IdGenNeg& getInstance()
    {
        if( instance.get() == nullptr ) {
            instance.reset(new IdGenNeg());
        }
        return *instance;
    }

    static int genId()
    {
        return getInstance().id--;
    }

    void reset()
    { 
        id = -1;
    }

}; // struct IDGenNeg


} // namespace sob







#endif
