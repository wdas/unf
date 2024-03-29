# -*- coding: utf-8 -*-

from pxr import Usd, Tf
import unf


def test_transaction_create_from_broker():
    """Create a transaction from broker."""
    stage = Usd.Stage.CreateInMemory()
    broker = unf.Broker.Create(stage)

    assert broker.IsInTransaction() is False

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.GetResyncedPaths() == ["/Foo"]
        received.append(notice)

    key = Tf.Notice.Register(unf.Notice.ObjectsChanged, _validate, stage)

    with unf.NoticeTransaction(broker) as transaction:
        stage.DefinePrim("/Foo")

        assert transaction.GetBroker() == broker
        assert broker.GetStage() == stage
        assert broker.IsInTransaction() is True

    assert broker.IsInTransaction() is False

    # Ensure that one notice was received.
    assert len(received) == 1

def test_transaction_create_from_broker_with_filter():
    """Create a transaction from broker with filter."""
    stage = Usd.Stage.CreateInMemory()
    broker = unf.Broker.Create(stage)

    assert broker.IsInTransaction() is False

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.GetResyncedPaths() == ["/Foo"]
        received.append(notice)

    def _filter(notice):
        """Filter out ObjectsChanged notice."""
        return type(notice) != unf.Notice.ObjectsChanged

    key = Tf.Notice.Register(unf.Notice.ObjectsChanged, _validate, stage)

    with unf.NoticeTransaction(broker, predicate=_filter) as transaction:
        stage.DefinePrim("/Foo")

        assert transaction.GetBroker() == broker
        assert broker.GetStage() == stage
        assert broker.IsInTransaction() is True

    assert broker.IsInTransaction() is False

    # Ensure that no notice was received.
    assert len(received) == 0

def test_transaction_create_from_broker_with_default_predicate():
    """Create a transaction from broker with default predicate."""
    stage = Usd.Stage.CreateInMemory()
    broker = unf.Broker.Create(stage)

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.GetResyncedPaths() == ["/Foo"]
        received.append(notice)

    key = Tf.Notice.Register(unf.Notice.ObjectsChanged, _validate, stage)

    with unf.NoticeTransaction(
        broker, predicate=unf.CapturePredicate.Default()
    ) as transaction:

        stage.DefinePrim("/Foo")

        broker = transaction.GetBroker()
        assert broker.GetStage() == stage
        assert broker.IsInTransaction() is True

    assert broker.IsInTransaction() is False

    # Ensure that one notice was received.
    assert len(received) == 1

def test_transaction_create_from_broker_with_blockall_predicate():
    """Create a transaction and block all notices in that scope."""
    stage = Usd.Stage.CreateInMemory()
    broker = unf.Broker.Create(stage)

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        received.append(notice)

    key = Tf.Notice.Register(unf.Notice.ObjectsChanged, _validate, stage)

    with unf.NoticeTransaction(
        broker, predicate=unf.CapturePredicate.BlockAll()
    ) as transaction:

        stage.DefinePrim("/Foo")

        broker = transaction.GetBroker()
        assert broker.GetStage() == stage
        assert broker.IsInTransaction() is True

    assert broker.IsInTransaction() is False

    # Ensure that no notices were received.
    assert len(received) == 0

def test_transaction_create_from_stage():
    """Create a transaction from stage."""
    stage = Usd.Stage.CreateInMemory()

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.GetResyncedPaths() == ["/Foo"]
        received.append(notice)

    key = Tf.Notice.Register(unf.Notice.ObjectsChanged, _validate, stage)

    with unf.NoticeTransaction(stage) as transaction:
        stage.DefinePrim("/Foo")

        broker = transaction.GetBroker()
        assert broker.GetStage() == stage
        assert broker.IsInTransaction() is True

    assert broker.IsInTransaction() is False

    # Ensure that one notice was received.
    assert len(received) == 1

def test_transaction_create_from_stage_with_filter():
    """Create a transaction from stage with filter."""
    stage = Usd.Stage.CreateInMemory()

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.GetResyncedPaths() == ["/Foo"]
        received.append(notice)

    def _filter(notice):
        """Filter out ObjectsChanged notice."""
        return type(notice) != unf.Notice.ObjectsChanged

    key = Tf.Notice.Register(unf.Notice.ObjectsChanged, _validate, stage)

    with unf.NoticeTransaction(stage, predicate=_filter) as transaction:
        stage.DefinePrim("/Foo")

        broker = transaction.GetBroker()
        assert broker.GetStage() == stage
        assert broker.IsInTransaction() is True

    assert broker.IsInTransaction() is False

    # Ensure that no notice was received.
    assert len(received) == 0

def test_transaction_create_from_stage_with_default_predicate():
    """Create a transaction from stage with default predicate."""
    stage = Usd.Stage.CreateInMemory()

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.GetResyncedPaths() == ["/Foo"]
        received.append(notice)

    key = Tf.Notice.Register(unf.Notice.ObjectsChanged, _validate, stage)

    with unf.NoticeTransaction(
        stage, predicate=unf.CapturePredicate.Default()
    ) as transaction:

        stage.DefinePrim("/Foo")

        broker = transaction.GetBroker()
        assert broker.GetStage() == stage
        assert broker.IsInTransaction() is True

    assert broker.IsInTransaction() is False

    # Ensure that one notice was received.
    assert len(received) == 1

def test_transaction_create_from_stage_with_blockall_predicate():
    """Create a transaction and block all notices in that scope."""
    stage = Usd.Stage.CreateInMemory()

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        received.append(notice)

    key = Tf.Notice.Register(unf.Notice.ObjectsChanged, _validate, stage)

    with unf.NoticeTransaction(
        stage, predicate=unf.CapturePredicate.BlockAll()
    ) as transaction:

        stage.DefinePrim("/Foo")

        broker = transaction.GetBroker()
        assert broker.GetStage() == stage
        assert broker.IsInTransaction() is True

    assert broker.IsInTransaction() is False

    # Ensure that no notices were received.
    assert len(received) == 0

def test_transaction_nested():
    """Create nested transactions."""
    stage = Usd.Stage.CreateInMemory()

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.GetResyncedPaths() == ["/Bar", "/Foo"]
        received.append(notice)

    key = Tf.Notice.Register(unf.Notice.ObjectsChanged, _validate, stage)

    with unf.NoticeTransaction(stage) as transaction1:
        stage.DefinePrim("/Foo")

        broker = transaction1.GetBroker()
        assert broker.GetStage() == stage
        assert broker.IsInTransaction() is True

        with unf.NoticeTransaction(stage) as transaction2:
            stage.DefinePrim("/Bar")

            _broker = transaction2.GetBroker()
            assert _broker == broker
            assert _broker.GetStage() == stage
            assert _broker.IsInTransaction() is True

        assert broker.IsInTransaction() is True

    assert broker.IsInTransaction() is False

    # Ensure that one notice was received.
    assert len(received) == 1

def test_transaction_nested_with_filter():
    """Create nested transactions with filter."""
    stage = Usd.Stage.CreateInMemory()

    received = []

    def _validate(notice, stage):
        """Validate notice received."""
        assert notice.GetResyncedPaths() == ["/Foo"]
        received.append(notice)

    def _filter(notice):
        """Filter out ObjectsChanged notice."""
        return type(notice) != unf.Notice.ObjectsChanged

    key = Tf.Notice.Register(unf.Notice.ObjectsChanged, _validate, stage)

    with unf.NoticeTransaction(stage) as transaction1:
        stage.DefinePrim("/Foo")

        broker = transaction1.GetBroker()
        assert broker.GetStage() == stage
        assert broker.IsInTransaction() is True

        with unf.NoticeTransaction(stage, predicate=_filter) as transaction2:
            stage.DefinePrim("/Bar")

            _broker = transaction2.GetBroker()
            assert _broker == broker
            assert _broker.GetStage() == stage
            assert _broker.IsInTransaction() is True

        assert broker.IsInTransaction() is True

    assert broker.IsInTransaction() is False

    # Ensure that one notice was received.
    assert len(received) == 1
