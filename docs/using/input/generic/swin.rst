.. _swin:

swin
====

Specify the size of the support (i.e., the rate of change) for spline-based surface definitions (see :ref:`elecsrfm`).
The syntax is:

.. code-block:: bash
   
   swin {win}

where ``win`` is a floating point number for the spline window width (in Å).
Usually 0.3 Å.</p>
Note that, per the analysis of Nina, Im, and Roux (`article <http://dx.doi.org/10.1016/S0301-4622(98)00236-1>`_)</a>, the force field parameters (radii) generally need to be adjusted if the spline window is changed.
