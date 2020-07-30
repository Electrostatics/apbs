from typing import List
import numpy as np
from ...mytypes import *

class Grid:
    '''
    Pulled over from src/mg/vgrid.(h|c)
    int nx;       /**< Number grid points in x direction */
    int ny;       /**< Number grid points in y direction */
    int nz;       /**< Number grid points in z direction */
    double hx;    /**< Grid spacing in x direction */
    double hy;    /**< Grid spacing in y direction */
    double hzed;  /**< Grid spacing in z direction */
    double xmin;  /**< x coordinate of lower grid corner */
    double ymin;  /**< y coordinate of lower grid corner */
    double zmin;  /**< z coordinate of lower grid corner */
    double xmax;  /**< x coordinate of upper grid corner */
    double ymax;  /**< y coordinate of upper grid corner */
    double zmax;  /**< z coordinate of upper grid corner */
    double *data; /**< nx*ny*nz array of data */
    int readdata; /**< flag indicating whether data was read from file */
    int ctordata; /**< flag indicating whether data was included at
    '''
    def __init__(self, dims, spaces, mins, maxs, data):
        self.dims: Array[int] = dims # prev: nx, ny, nz
        self.spaces: Array[int] = spaces # prev: hx, ...
        self.mins: Array[int] = mins # prev: minx, ...
        self.maxs: Array[int] = maxs # prev: minx, ...
        self.data: Array[float] = data
        self.readdata: bool
        self.ctordata: bool
