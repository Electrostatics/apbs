UHBD scalar data format
=======================

We also support scalar data output in the legacy "UHBD format" for use with programs such as `UHBD <http://browndye.ucsd.edu/>`_ and `SDA <https://mcm.h-its.org/sda/>`_.

UHBD data is written in the format:

.. code-block:: c

    /* Write out the header */
    Vio_printf(sock, "%72s\n", title);
    Vio_printf(sock, "%12.5e%12.5e%7d%7d%7d%7d%7d\n", 1.0, 0.0, -1, 0,
      nz, 1, nz);
    Vio_printf(sock, "%7d%7d%7d%12.5e%12.5e%12.5e%12.5e\n", nx, ny, nz,
      hx, (xmin-hx), (ymin-hx), (zmin-hx));
    Vio_printf(sock, "%12.5e%12.5e%12.5e%12.5e\n", 0.0, 0.0, 0.0, 0.0);
    Vio_printf(sock, "%12.5e%12.5e%7d%7d", 0.0, 0.0, 0, 0);

    /* Write out the entries */
    icol = 0;
    for (k=0; k<nz; k++) {
        Vio_printf(sock, "\n%7d%7d%7d\n", k+1, thee->nx, thee->ny);
        icol = 0;
        for (j=0; j<ny; j++) {
            for (i=0; i<nx; i++) {
                u = k*(nx)*(ny)+j*(nx)+i;
                icol++;
                Vio_printf(sock, " %12.5e", thee->data[u]);
                if (icol == 6) {
                    icol = 0;
                    Vio_printf(sock, "\n");
                }
            }
        }
    }
