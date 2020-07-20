.. _mac:

mac
===

TABI-PB parameter, multipole acceptance criterion (MAC), that controls distance ratio at which the method uses direct summation or Taylor approximation (a particle-cluster interaction) to calculate the integral kernels.
The syntax is:

.. code-block:: bash

   mac {theta}

where ``theta`` is a floating-point number from 0 to 1 controlling the distance ratio.
This multipole acceptance criterion (MAC) is :math:`\frac{r_c}{R}\leqslant \theta`, where :math:`r_c` is the cluster radius, and :math:`R` is the distance of the particle to the cluster center.
If the above relationship is satisfied, the Taylor approximation will be used instead of direct summation.
A typical value for this parameter is 0.8.