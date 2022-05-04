#include "../cache.h"
#include "../broker.h"
#include "./listener.h"
#include "./notice.h"

#include "gtest/gtest.h"
#include "pxr/pxr.h"
#include "pxr/usd/sdf/path.h"
#include "pxr/usd/usd/primRange.h"
#include "pxr/usd/usd/stage.h"

#include <unordered_map>
#include <vector>
#include <string>

using namespace UsdBrokerNotice;

TEST(NoticeCache, base)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    std::vector<std::unique_ptr<BaseNoticeCache> > caches;
    caches.push_back(std::make_unique<NoticeCache<StageNotice> >());
    caches.push_back(std::make_unique<NoticeCache<StageContentsChanged> >());
    caches.push_back(std::make_unique<NoticeCache<ObjectsChanged> >());
    caches.push_back(std::make_unique<NoticeCache<StageEditTargetChanged> >());
    caches.push_back(std::make_unique<NoticeCache<LayerMutingChanged> >());
    caches.push_back(std::make_unique<NoticeCache<MergeableNotice> >());
    caches.push_back(std::make_unique<NoticeCache<UnMergeableNotice> >());

    for (auto& cache: caches) {
        ASSERT_EQ(cache->GetAll().size(), 0);
    }
}

TEST(NoticeCache, addPrim)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    std::vector<std::unique_ptr<BaseNoticeCache> > caches;
    caches.push_back(std::make_unique<NoticeCache<StageNotice> >());
    caches.push_back(std::make_unique<NoticeCache<StageContentsChanged> >());
    caches.push_back(std::make_unique<NoticeCache<ObjectsChanged> >());
    caches.push_back(std::make_unique<NoticeCache<StageEditTargetChanged> >());
    caches.push_back(std::make_unique<NoticeCache<LayerMutingChanged> >());
    caches.push_back(std::make_unique<NoticeCache<MergeableNotice> >());
    caches.push_back(std::make_unique<NoticeCache<UnMergeableNotice> >());

    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});

    ASSERT_EQ(caches.at(0)->GetAll().size(), 2);
    ASSERT_EQ(caches.at(1)->GetAll().size(), 1);
    ASSERT_EQ(caches.at(2)->GetAll().size(), 1);
    ASSERT_EQ(caches.at(3)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(4)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(5)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(6)->GetAll().size(), 0);

    for (auto& cache: caches) {
        cache->MergeAll();
    }

    ASSERT_EQ(caches.at(0)->GetAll().size(), 1);
    ASSERT_EQ(caches.at(1)->GetAll().size(), 1);
    ASSERT_EQ(caches.at(2)->GetAll().size(), 1);
    ASSERT_EQ(caches.at(3)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(4)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(5)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(6)->GetAll().size(), 0);
}

TEST(NoticeCache, addPrims)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    std::vector<std::unique_ptr<BaseNoticeCache> > caches;
    caches.push_back(std::make_unique<NoticeCache<StageNotice> >());
    caches.push_back(std::make_unique<NoticeCache<StageContentsChanged> >());
    caches.push_back(std::make_unique<NoticeCache<ObjectsChanged> >());
    caches.push_back(std::make_unique<NoticeCache<StageEditTargetChanged> >());
    caches.push_back(std::make_unique<NoticeCache<LayerMutingChanged> >());
    caches.push_back(std::make_unique<NoticeCache<MergeableNotice> >());
    caches.push_back(std::make_unique<NoticeCache<UnMergeableNotice> >());

    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});
    stage->DefinePrim(PXR_NS::SdfPath {"/Bar"});
    stage->DefinePrim(PXR_NS::SdfPath {"/Baz"});

    ASSERT_EQ(caches.at(0)->GetAll().size(), 6);
    ASSERT_EQ(caches.at(1)->GetAll().size(), 3);
    ASSERT_EQ(caches.at(2)->GetAll().size(), 3);
    ASSERT_EQ(caches.at(3)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(4)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(5)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(6)->GetAll().size(), 0);

    for (auto& cache: caches) {
        cache->MergeAll();
    }

    ASSERT_EQ(caches.at(0)->GetAll().size(), 1);
    ASSERT_EQ(caches.at(1)->GetAll().size(), 1);
    ASSERT_EQ(caches.at(2)->GetAll().size(), 1);
    ASSERT_EQ(caches.at(3)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(4)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(5)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(6)->GetAll().size(), 0);
}

TEST(NoticeCache, muteLayer)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    stage->DefinePrim(PXR_NS::SdfPath {"/Foo"});
    stage->DefinePrim(PXR_NS::SdfPath {"/Bar"});

    std::vector<std::unique_ptr<BaseNoticeCache> > caches;
    caches.push_back(std::make_unique<NoticeCache<StageNotice> >());
    caches.push_back(std::make_unique<NoticeCache<StageContentsChanged> >());
    caches.push_back(std::make_unique<NoticeCache<ObjectsChanged> >());
    caches.push_back(std::make_unique<NoticeCache<StageEditTargetChanged> >());
    caches.push_back(std::make_unique<NoticeCache<LayerMutingChanged> >());
    caches.push_back(std::make_unique<NoticeCache<MergeableNotice> >());
    caches.push_back(std::make_unique<NoticeCache<UnMergeableNotice> >());

    stage->MuteLayer("/Foo");
    stage->MuteLayer("/Bar");

    ASSERT_EQ(caches.at(0)->GetAll().size(), 2);
    ASSERT_EQ(caches.at(1)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(2)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(3)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(4)->GetAll().size(), 2);
    ASSERT_EQ(caches.at(5)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(6)->GetAll().size(), 0);

    for (auto& cache: caches) {
        cache->MergeAll();
    }

    ASSERT_EQ(caches.at(0)->GetAll().size(), 1);
    ASSERT_EQ(caches.at(1)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(2)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(3)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(4)->GetAll().size(), 1);
    ASSERT_EQ(caches.at(5)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(6)->GetAll().size(), 0);
}

TEST(NoticeCache, changeEditTarget)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto rootLayer = stage->GetRootLayer();

    auto s1 = PXR_NS::SdfLayer::CreateAnonymous(".usda");
    auto s2 = PXR_NS::SdfLayer::CreateAnonymous(".usda");
    rootLayer->SetSubLayerPaths({
        s1->GetIdentifier(),
        s2->GetIdentifier(),
    });

    auto broker = PXR_NS::NoticeBroker::Create(stage);

    std::vector<std::unique_ptr<BaseNoticeCache> > caches;
    caches.push_back(std::make_unique<NoticeCache<StageNotice> >());
    caches.push_back(std::make_unique<NoticeCache<StageContentsChanged> >());
    caches.push_back(std::make_unique<NoticeCache<ObjectsChanged> >());
    caches.push_back(std::make_unique<NoticeCache<StageEditTargetChanged> >());
    caches.push_back(std::make_unique<NoticeCache<LayerMutingChanged> >());
    caches.push_back(std::make_unique<NoticeCache<MergeableNotice> >());
    caches.push_back(std::make_unique<NoticeCache<UnMergeableNotice> >());

    stage->SetEditTarget(PXR_NS::UsdEditTarget(s1));
    stage->SetEditTarget(PXR_NS::UsdEditTarget(s2));

    ASSERT_EQ(caches.at(0)->GetAll().size(), 2);
    ASSERT_EQ(caches.at(1)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(2)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(3)->GetAll().size(), 2);
    ASSERT_EQ(caches.at(4)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(5)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(6)->GetAll().size(), 0);

    for (auto& cache: caches) {
        cache->MergeAll();
    }

    ASSERT_EQ(caches.at(0)->GetAll().size(), 1);
    ASSERT_EQ(caches.at(1)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(2)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(3)->GetAll().size(), 1);
    ASSERT_EQ(caches.at(4)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(5)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(6)->GetAll().size(), 0);
}

TEST(NoticeCache, customNotices)
{
    auto stage = PXR_NS::UsdStage::CreateInMemory();
    auto broker = PXR_NS::NoticeBroker::Create(stage);

    std::vector<std::unique_ptr<BaseNoticeCache> > caches;
    caches.push_back(std::make_unique<NoticeCache<StageNotice> >());
    caches.push_back(std::make_unique<NoticeCache<StageContentsChanged> >());
    caches.push_back(std::make_unique<NoticeCache<ObjectsChanged> >());
    caches.push_back(std::make_unique<NoticeCache<StageEditTargetChanged> >());
    caches.push_back(std::make_unique<NoticeCache<LayerMutingChanged> >());
    caches.push_back(std::make_unique<NoticeCache<MergeableNotice> >());
    caches.push_back(std::make_unique<NoticeCache<UnMergeableNotice> >());

    broker->Send<MergeableNotice>();
    broker->Send<MergeableNotice>();
    broker->Send<MergeableNotice>();

    broker->Send<UnMergeableNotice>();
    broker->Send<UnMergeableNotice>();
    broker->Send<UnMergeableNotice>();

    ASSERT_EQ(caches.at(0)->GetAll().size(), 6);
    ASSERT_EQ(caches.at(1)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(2)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(3)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(4)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(5)->GetAll().size(), 3);
    ASSERT_EQ(caches.at(6)->GetAll().size(), 3);

    for (auto& cache: caches) {
        cache->MergeAll();
    }

    // TODO: Does it make sense that base class is instantiable?
    ASSERT_EQ(caches.at(0)->GetAll().size(), 1);
    ASSERT_EQ(caches.at(1)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(2)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(3)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(4)->GetAll().size(), 0);
    ASSERT_EQ(caches.at(5)->GetAll().size(), 1);
    ASSERT_EQ(caches.at(6)->GetAll().size(), 3);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
