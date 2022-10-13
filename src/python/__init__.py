# :coding: utf-8

from pxr import Tf
from . import _usd_notice_framework # pylint:disable=import-error

Tf.PrepareModule(_usd_notice_framework, locals())
del Tf
