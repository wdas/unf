# :coding: utf-8

from pxr import Tf
from . import _unf # pylint:disable=import-error

Tf.PrepareModule(_unf, locals())
del Tf
