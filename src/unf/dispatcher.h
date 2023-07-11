#ifndef USD_NOTICE_FRAMEWORK_DISPATCHER_H
#define USD_NOTICE_FRAMEWORK_DISPATCHER_H

/// \file unf/dispatcher.h

#include "unf/broker.h"
#include "unf/notice.h"

#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/type.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>

namespace unf {

/// \class Dispatcher
///
/// \brief
/// Interface for objects emitting standalone notices triggered by incoming
/// PXR_NS::TfNotice derived notices.
class Dispatcher : public PXR_NS::TfRefBase, public PXR_NS::TfWeakBase {
  public:
    /// Revoke all registered listeners on destruction.
    virtual ~Dispatcher() { Revoke(); };

    /// \brief
    /// Get unique string identifier
    ///
    /// Identifier will be used to retrieve Dispatcher from the Broker.
    ///
    /// \sa
    /// Broker::GetDispatcher
    virtual std::string GetIdentifier() const = 0;

    ///  Register listeners to PXR_NS::TfNotice derived notices.
    virtual void Register() = 0;

    /// Revoke all registered listeners.
    virtual void Revoke();

  protected:
    /// Create a dispatcher targeting a Broker.
    Dispatcher(const BrokerWeakPtr&);

    /// \brief
    /// Convenient templated method to register a listener for incoming
    /// \p InputNotice notice which emits a \p OutputNotice notice.
    ///
    /// The following example will emit a UnfNotice::ObjectsChanged notice for
    /// each PXR_NS::UsdNotice::ObjectsChanged received.
    ///
    /// \code{.cpp}
    /// _Register<
    ///     PXR_NS::UsdNotice::ObjectsChanged,
    ///     UnfNotice::ObjectsChanged>();
    /// \endcode
    ///
    /// \warning
    /// The \p OutputNotice notice must be derived from
    /// UnfNotice::StageNotice and must have a constructor which takes an
    /// instance of \p InputNotice.
    template <class InputNotice, class OutputNotice>
    void _Register()
    {
        auto self = PXR_NS::TfCreateWeakPtr(this);
        auto cb = &Dispatcher::_OnReceiving<InputNotice, OutputNotice>;
        _keys.push_back(
            PXR_NS::TfNotice::Register(self, cb, _broker->GetStage()));
    }

    /// \brief
    /// Convenient templated method to emit a \p OutputNotice notice from an
    /// incoming \p InputNotice notice.
    ///
    /// \warning
    /// The \p OutputNotice notice must be derived from
    /// UnfNotice::StageNotice and must have a constructor which takes an
    /// instance of \p InputNotice.
    template <class InputNotice, class OutputNotice>
    void _OnReceiving(const InputNotice& notice)
    {
        PXR_NS::TfRefPtr<OutputNotice> _notice = OutputNotice::Create(notice);
        _broker->Send(_notice);
    }

    /// Broker that the dispatcher is attached to.
    BrokerWeakPtr _broker;

    /// List of handle-objects used for registering listeners.
    std::vector<PXR_NS::TfNotice::Key> _keys;
};

/// \class StageDispatcher
///
/// \brief
/// Default dispatcher which emits UnfNotice::StageNotice derived notices
/// corresponding to each PXR_NS::UsdNotice::StageNotice derived notice
/// received.
class StageDispatcher : public Dispatcher {
  public:
    virtual std::string GetIdentifier() const override
    {
        return "StageDispatcher";
    }

    /// \brief
    /// Register listeners to each PXR_NS::UsdNotice::StageNotice derived
    /// notices.
    virtual void Register() override;

  private:
    StageDispatcher(const BrokerWeakPtr& broker);

    /// Only a Broker can create a StageDispatcher.
    friend class Broker;
};

/// \class DispatcherFactory
///
/// \brief
/// Interface for building Dispatcher type.
///
/// \sa
/// DispatcherFactoryImpl
class DispatcherFactory : public PXR_NS::TfType::FactoryBase {
  public:
    /// Base constructor to create a Dispatcher.
    virtual PXR_NS::TfRefPtr<Dispatcher> New(
        const BrokerWeakPtr& broker) const = 0;
};

/// \class DispatcherFactoryImpl
///
/// \brief
/// Templated factory class which creates a specific type of Dispatcher.
///
/// \sa
/// DispatcherDefine
template <class T>
class DispatcherFactoryImpl : public DispatcherFactory {
  public:
    /// Create a Dispatcher and return reference pointer.
    virtual PXR_NS::TfRefPtr<Dispatcher> New(
        const BrokerWeakPtr& broker) const override
    {
        return PXR_NS::TfCreateRefPtr(new T(broker));
    }
};

/// \fn DispatcherDefine
///
/// \brief
/// Define a PXR_NS::TfType for a specific type of Dispatcher.
///
/// Typical usage to define a type for a dispatcher \p Foo would be:
///
/// \code{.cpp}
/// TF_REGISTRY_FUNCTION(PXR_NS::TfType)
/// {
///     DispatcherDefine<Foo, Dispatcher>();
/// }
/// \endcode
template <class T, class... Bases>
void DispatcherDefine()
{
    PXR_NS::TfType::Define<T, PXR_NS::TfType::Bases<Bases...> >()
        .template SetFactory<DispatcherFactoryImpl<T> >();
}

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_DISPATCHER_H
