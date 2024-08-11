#include <IdGen.h>



namespace sob {

std::unique_ptr<IdGen> IdGen::instance {nullptr};

std::unique_ptr<IdGenNeg> IdGenNeg::instance {nullptr};

} // namespace sob
