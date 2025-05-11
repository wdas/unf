#include "./predicate.h"

#include "unf/capturePredicate.h"

#ifndef PXR_USE_INTERNAL_BOOST_PYTHON
#include <boost/python.hpp>
using namespace boost::python;
#else
#include <pxr/external/boost/python.hpp>
using namespace PXR_BOOST_PYTHON_NAMESPACE;
#endif

using namespace unf;

PXR_NAMESPACE_USING_DIRECTIVE


void wrapCapturePredicate()
{
    class_<CapturePredicate>(
        "CapturePredicate",
        "Predicate functor which indicates whether a notice can be captured "
        "during a transaction.",
        no_init)

        .def(
            "Default",
            &CapturePredicate::Default,
            "Create a predicate which return true for each notice type.")
        .staticmethod("Default")

        .def(
            "BlockAll",
            &CapturePredicate::BlockAll,
            "Create a predicate which return false for each notice type.")
        .staticmethod("BlockAll");
}
