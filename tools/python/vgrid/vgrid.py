# This file was created automatically by SWIG.
# Don't modify this file, modify the SWIG interface instead.
# This file is compatible with both classic and new-style classes.

import _vgrid

def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "this"):
        if isinstance(value, class_type):
            self.__dict__[name] = value.this
            if hasattr(value,"thisown"): self.__dict__["thisown"] = value.thisown
            del value.thisown
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static) or hasattr(self,name) or (name == "thisown"):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError,name

import types
try:
    _object = types.ObjectType
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0
del types



null_array = _vgrid.null_array
class Vgrid(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, Vgrid, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, Vgrid, name)
    def __repr__(self):
        return "<%s.%s; proxy of C Vgrid instance at %s>" % (self.__class__.__module__, self.__class__.__name__, self.this,)
    def __init__(self, *args):
        _swig_setattr(self, Vgrid, 'this', _vgrid.new_Vgrid(*args))
        _swig_setattr(self, Vgrid, 'thisown', 1)
    def __del__(self, destroy=_vgrid.delete_Vgrid):
        try:
            if self.thisown: destroy(self)
        except: pass

    __swig_setmethods__["nx"] = _vgrid.Vgrid_nx_set
    __swig_getmethods__["nx"] = _vgrid.Vgrid_nx_get
    if _newclass:nx = property(_vgrid.Vgrid_nx_get, _vgrid.Vgrid_nx_set)
    __swig_setmethods__["ny"] = _vgrid.Vgrid_ny_set
    __swig_getmethods__["ny"] = _vgrid.Vgrid_ny_get
    if _newclass:ny = property(_vgrid.Vgrid_ny_get, _vgrid.Vgrid_ny_set)
    __swig_setmethods__["nz"] = _vgrid.Vgrid_nz_set
    __swig_getmethods__["nz"] = _vgrid.Vgrid_nz_get
    if _newclass:nz = property(_vgrid.Vgrid_nz_get, _vgrid.Vgrid_nz_set)
    __swig_setmethods__["hx"] = _vgrid.Vgrid_hx_set
    __swig_getmethods__["hx"] = _vgrid.Vgrid_hx_get
    if _newclass:hx = property(_vgrid.Vgrid_hx_get, _vgrid.Vgrid_hx_set)
    __swig_setmethods__["hy"] = _vgrid.Vgrid_hy_set
    __swig_getmethods__["hy"] = _vgrid.Vgrid_hy_get
    if _newclass:hy = property(_vgrid.Vgrid_hy_get, _vgrid.Vgrid_hy_set)
    __swig_setmethods__["hzed"] = _vgrid.Vgrid_hzed_set
    __swig_getmethods__["hzed"] = _vgrid.Vgrid_hzed_get
    if _newclass:hzed = property(_vgrid.Vgrid_hzed_get, _vgrid.Vgrid_hzed_set)
    __swig_setmethods__["xmin"] = _vgrid.Vgrid_xmin_set
    __swig_getmethods__["xmin"] = _vgrid.Vgrid_xmin_get
    if _newclass:xmin = property(_vgrid.Vgrid_xmin_get, _vgrid.Vgrid_xmin_set)
    __swig_setmethods__["ymin"] = _vgrid.Vgrid_ymin_set
    __swig_getmethods__["ymin"] = _vgrid.Vgrid_ymin_get
    if _newclass:ymin = property(_vgrid.Vgrid_ymin_get, _vgrid.Vgrid_ymin_set)
    __swig_setmethods__["zmin"] = _vgrid.Vgrid_zmin_set
    __swig_getmethods__["zmin"] = _vgrid.Vgrid_zmin_get
    if _newclass:zmin = property(_vgrid.Vgrid_zmin_get, _vgrid.Vgrid_zmin_set)
    __swig_setmethods__["data"] = _vgrid.Vgrid_data_set
    __swig_getmethods__["data"] = _vgrid.Vgrid_data_get
    if _newclass:data = property(_vgrid.Vgrid_data_get, _vgrid.Vgrid_data_set)

class VgridPtr(Vgrid):
    def __init__(self, this):
        _swig_setattr(self, Vgrid, 'this', this)
        if not hasattr(self,"thisown"): _swig_setattr(self, Vgrid, 'thisown', 0)
        _swig_setattr(self, Vgrid,self.__class__,Vgrid)
_vgrid.Vgrid_swigregister(VgridPtr)


delete_vgrid = _vgrid.delete_vgrid

Vgrid_ctor2 = _vgrid.Vgrid_ctor2

Vgrid_dtor = _vgrid.Vgrid_dtor

Vgrid_dtor2 = _vgrid.Vgrid_dtor2

Vgrid_writeUHBD = _vgrid.Vgrid_writeUHBD

Vgrid_writeDX = _vgrid.Vgrid_writeDX

Vgrid_readDX = _vgrid.Vgrid_readDX

startVio = _vgrid.startVio

Vgrid_value = _vgrid.Vgrid_value

Vgrid_curvature = _vgrid.Vgrid_curvature

Vgrid_gradient = _vgrid.Vgrid_gradient

Vgrid_ctor = _vgrid.Vgrid_ctor

