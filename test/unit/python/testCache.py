# -*- coding: utf-8 -*-

from pxr import Usd
from usd_notice_framework import NoticeCache, Broker, BrokerNotice
from usd_notice_framework_test import TestNotice

import pytest


@pytest.mark.parametrize("notice", [
    Usd.Notice.StageNotice,
    Usd.Notice.ObjectsChanged,
    Usd.Notice.StageContentsChanged,
    Usd.Notice.StageEditTargetChanged,
    Usd.Notice.LayerMutingChanged,
    BrokerNotice.StageNotice,
], ids=[
    "Usd.Notice.StageNotice",
    "Usd.Notice.ObjectsChanged",
    "Usd.Notice.StageContentsChanged",
    "Usd.Notice.StageEditTargetChanged",
    "Usd.Notice.LayerMutingChanged",
    "BrokerNotice.StageNotice",
])
def test_incorrect_notice(notice):
    """Start cache with incorrect notice."""
    with pytest.raises(RuntimeError) as error:
        NoticeCache(notice)

    assert (
        "Expecting a notice derived from BrokerNotice::StageNotice."
    ) in str(error)


def test_custom_unmergeable_notice():
    """Cache custom mergeable notices."""
    stage = Usd.Stage.CreateInMemory()
    broker = Broker.Create(stage)

    cache = NoticeCache(TestNotice.UnMergeableNotice)

    notice1 = TestNotice.UnMergeableNoticeWrapper.Init()
    broker.Send(notice1)

    notice2 = TestNotice.UnMergeableNoticeWrapper.Init()
    broker.Send(notice2)

    notice3 = TestNotice.UnMergeableNoticeWrapper.Init()
    broker.Send(notice3)

    # Ensure that three notices has been cached.
    assert cache.Size() == 3

    cache.MergeAll()

    # Ensure that we still have three notice after consolidation.
    assert cache.Size() == 3
