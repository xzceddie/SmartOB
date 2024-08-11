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


} // namespace sob







#endif
