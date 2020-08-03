from . import Coordinate


class SurfacePoint(Coordinate):
    """
    Thin abstraction over Coordinate class which keeps track of whether or not
    the given coordinate falls on the surface which encapsulates it.
    """

    def __init__(self, *args, **kwargs):
        self.is_on_surf: bool = False
        if "is_on_surf" in kwargs.keys():
            self.is_on_surf = kwargs["is_on_surf"]
            del kwargs["is_on_surf"]
        super().__init__(*args, **kwargs)
