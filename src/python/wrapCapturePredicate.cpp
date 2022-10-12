#include "./predicate.h"

#include "unf/capturePredicate.h"

#include <boost/python.hpp>

using namespace boost::python;
using namespace unf;

PXR_NAMESPACE_USING_DIRECTIVE


void wrapCapturePredicate()
{
    class_<CapturePredicate>("CapturePredicate", no_init)

        .def(
            "Default",
            &CapturePredicate::Default)
        .staticmethod("Default")

        .def(
            "BlockAll",
            &CapturePredicate::BlockAll)
        .staticmethod("BlockAll");
}
