#ifndef USD_NOTICE_FRAMEWORK_BROKER_H
#define USD_NOTICE_FRAMEWORK_BROKER_H

/// \file unf/broker.h

#include "unf/api.h"
#include "unf/capturePredicate.h"
#include "unf/notice.h"

#include <pxr/base/plug/plugin.h>
#include <pxr/base/plug/registry.h>
#include <pxr/base/tf/refBase.h>
#include <pxr/base/tf/refPtr.h>
#include <pxr/base/tf/weakBase.h>
#include <pxr/base/tf/weakPtr.h>
#include <pxr/pxr.h>
#include <pxr/usd/usd/common.h>
#include <pxr/usd/usd/stage.h>

#include <functional>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace unf {

class Broker;
class Dispatcher;

/// Convenient alias for Broker reference pointer.
using BrokerPtr = PXR_NS::TfRefPtr<Broker>;

/// Convenient alias for Broker weak pointer.
using BrokerWeakPtr = PXR_NS::TfWeakPtr<Broker>;

/// Convenient alias for Dispatcher reference pointer.
using DispatcherPtr = PXR_NS::TfRefPtr<Dispatcher>;

/// \class Broker
///
/// \brief
/// Intermediate object between the Usd Stage and any clients that needs
/// asynchronous handling and upstream filtering of notices.
class Broker : public PXR_NS::TfRefBase, public PXR_NS::TfWeakBase {
  public:
    /// \brief
    /// Create a broker from a Usd Stage.
    ///
    /// If a broker has already been created from this \p stage, it will be
    /// returned. Otherwise, a new one will be created and returned.
    UNF_API static BrokerPtr Create(const PXR_NS::UsdStageWeakPtr& stage);

    UNF_API virtual ~Broker() = default;

    /// Remove default copy constructor.
    UNF_API Broker(const Broker&) = delete;

    /// Remove default assignment operator.
    UNF_API Broker& operator=(const Broker&) = delete;

    /// Return Usd Stage associated with the broker.
    UNF_API const PXR_NS::UsdStageWeakPtr GetStage() const;

    /// \brief
    /// Indicate whether a notice transaction has been started.
    /// \sa BeginTransaction
    UNF_API bool IsInTransaction();

    /// \brief
    /// Start a notice transaction.
    ///
    /// Notices derived from UnfNotice::StageNotice will be held during
    /// the transaction and emitted at the end.
    ///
    /// By default, all UnfNotice::StageNotice notices will be captured during
    /// the entire scope of the transaction. A CapturePredicate can be passed to
    /// influence which notices are captured. Notices that are not captured
    /// will not be emitted.
    ///
    /// \warning
    /// Each transaction started must be closed with EndTransaction.
    /// It is preferrable to use NoticeTransaction over this API to safely
    /// manage transactions.
    ///
    /// \sa EndTransaction
    /// \sa NoticeTransaction
    UNF_API void BeginTransaction(
        CapturePredicate predicate = CapturePredicate::Default());

    /// \brief
    /// Start a notice transaction with a capture predicate function.
    ///
    /// The following example will filter out all 'Foo' notices emitted during
    /// the transaction.
    ///
    /// \code{.cpp}
    /// broker->BeginTransaction([&](const unf::UnfNotice::StageNotice& n) {
    ///     return (n.GetTypeId() != typeid(Foo).name());
    /// });
    /// \endcode
    ///
    /// \warning
    /// Each transaction started must be closed with EndTransaction.
    /// It is preferrable to use NoticeTransaction over this API to safely
    /// manage transactions.
    ///
    /// \sa EndTransaction
    /// \sa NoticeTransaction
    UNF_API void BeginTransaction(const CapturePredicateFunc&);

    /// \brief
    /// Stop a notice transaction.
    ///
    /// This will trigger the emission of all captured
    /// UnfNotice::StageNotice notices. Each notice type will be
    /// consolidated before emission if applicable.
    ///
    /// \warning
    /// It is preferrable to use NoticeTransaction over this API to safely
    /// manage transactions.
    ///
    /// \sa BeginTransaction
    /// \sa NoticeTransaction
    UNF_API void EndTransaction();

    /// \brief
    /// Create and send a UnfNotice::StageNotice notice via the broker.
    ///
    /// \note
    /// The associated stage will be used as sender.
    template <class UnfNotice, class... Args>
    void Send(Args&&... args);

    /// \brief
    /// Send a UnfNotice::StageNotice notice via the broker.
    ///
    /// \note
    /// The associated stage will be used as sender.
    UNF_API void Send(const UnfNotice::StageNoticeRefPtr&);

    /// Return dispatcher reference associated with \p identifier.
    UNF_API DispatcherPtr& GetDispatcher(std::string identifier);

    /// \brief
    /// Create and register a new dispatcher.
    ///
    /// This will call the Dispatcher::Register method.
    template <class T>
    void AddDispatcher();

    /// \brief
    /// Un-register broker.
    ///
    /// \warning
    /// The broker is not safe to use after this call.
    UNF_API void Reset();

    /// Un-register all brokers.
    UNF_API static void ResetAll();

  private:
    Broker(const PXR_NS::UsdStageWeakPtr&);

    /// Un-register brokers targeting expired stages.
    static void _CleanCache();

    /// Discover all dispatchers registered as plugins.
    void _DiscoverDispatchers();

    /// Register dispacther within broker by its identifier.
    UNF_API void _Add(const DispatcherPtr&);

    /// Create and register dispacther within broker without running the
    /// Dispatcher::Register method.
    template <class T>
    DispatcherPtr _AddDispatcher();

    /// Load all dispatchers from discovered factory types.
    template <class OutputPtr, class OutputFactory>
    void _LoadFromPlugins(const PXR_NS::TfType& type);

    struct UsdStageWeakPtrHasher {
        std::size_t operator()(const PXR_NS::UsdStageWeakPtr& ptr) const
        {
            return hash_value(ptr);
        }
    };

    /// Record each hashed stage pointer to its corresponding broker pointer.
    static std::unordered_map<
        PXR_NS::UsdStageWeakPtr, BrokerPtr, UsdStageWeakPtrHasher>
        Registry;

    class _NoticeMerger {
      public:
        _NoticeMerger(CapturePredicate predicate = CapturePredicate::Default());

        void Add(const UnfNotice::StageNoticeRefPtr&);
        void Join(_NoticeMerger&);
        void Merge();
        void PostProcess();
        void Send(const PXR_NS::UsdStageWeakPtr&);

      private:
        using _NoticePtrList = std::vector<UnfNotice::StageNoticeRefPtr>;
        using _NoticePtrMap = std::unordered_map<std::string, _NoticePtrList>;

        _NoticePtrMap _noticeMap;
        CapturePredicate _predicate;
    };

    /// Usd Stage associated with broker.
    PXR_NS::UsdStageWeakPtr _stage;

    /// List of NoticeMerger objects which handle transactions.
    std::vector<_NoticeMerger> _mergers;

    /// List of registered Dispatchers.
    std::unordered_map<std::string, DispatcherPtr> _dispatcherMap;
};

template <class UnfNotice, class... Args>
void Broker::Send(Args&&... args)
{
    PXR_NS::TfRefPtr<UnfNotice> _notice =
        UnfNotice::Create(std::forward<Args>(args)...);

    Send(_notice);
}

template <class T>
DispatcherPtr Broker::_AddDispatcher()
{
    static_assert(
        std::is_base_of<Dispatcher, T>::value,
        "Expecting a type derived from unf::Dispatcher.");

    auto self = PXR_NS::TfCreateWeakPtr(this);
    DispatcherPtr dispatcher = PXR_NS::TfCreateRefPtr(new T(self));
    _Add(dispatcher);
    return dispatcher;
}

template <class T>
void Broker::AddDispatcher()
{
    const auto& dispatcher = _AddDispatcher<T>();
    dispatcher->Register();
}

template <class OutputPtr, class OutputFactory>
void Broker::_LoadFromPlugins(const PXR_NS::TfType& type)
{
    PXR_NAMESPACE_USING_DIRECTIVE

    const PXR_NS::PlugPluginPtr plugin =
        PXR_NS::PlugRegistry::GetInstance().GetPluginForType(type);

    if (!plugin) {
        return;
    }

    if (!plugin->Load()) {
        TF_CODING_ERROR(
            "Failed to load plugin %s for %s",
            plugin->GetName().c_str(),
            type.GetTypeName().c_str());
        return;
    }

    OutputPtr output;
    OutputFactory* factory = type.GetFactory<OutputFactory>();

    if (factory) {
        output = factory->New(TfCreateWeakPtr(this));
    }

    if (!output) {
        TF_CODING_ERROR(
            "Failed to manufacture %s from plugin %s",
            type.GetTypeName().c_str(),
            plugin->GetName().c_str());
        return;
    }

    _Add(output);
}

}  // namespace unf

#endif  // USD_NOTICE_FRAMEWORK_BROKER_H
