# :coding: utf-8

from pxr import Tf
from . import _usd_notice_broker_test # pylint:disable=import-error

Tf.PrepareModule(_usd_notice_broker_test, locals())
del Tf
