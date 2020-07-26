=============================
Virtual reality with UnityMol
=============================

Molecular visualization software packages provide the ability for users to explore the 3D representations molecular structures and properties.
Typical user interaction is limited to panning, zooming, and rotating the molecule using a mouse and keyboard while viewing on a standard computing monitor.
These techniques support a pseudo 3-dimensional view of a molecule to understand its structure but lack the true depth perception people are used to with stereoscopic vision in the real world.

New advancements in virtual reality (VR) technologies has resulted in lower costs and systems that are easier to use to many consumers.
Compared to past VR hardware, these new systems have several key advancements including lower latency, higher frame rates, and improved resolution.
Additionally, these systems are equipped with better optics and motion tracking and a more robust software ecosystem.

We are extending the visualization capabilities for APBS through the incorporation of a VR device with molecular rendering software.
We are currently experimenting with the HTC Vive, which allows a person to walk around a 15' by 15' physical space while wearing a head mounted display.
Precise head movements are matched in virtual reality with no noticeable latency.
Additionally, the HTC Vive controllers are motion tracked with millimeter precision and provide a valuable method for interacting with virtual objects.
We have enabled VR using the HTC Vive in the `UnityMol molecular visualization software <http://www.baaden.ibpc.fr/umol/>`_ (created by Baaden, et al.) and incorporated electrostatic surface data (see figure below and a `YouTube video <https://www.youtube.com/watch?v=Xxb3W8jnnp8&t=21s>`_).
New viewing capabilities now include walking around, grabbing (using the motion controllers), and scaling (gestures) of molecules.
We are actively working with Dr. Baaden and his group to determine the best use of interaction techniques for users to interact with molecular models through his software.

.. figure:: /media/1fas_VR.png

   View of UnityMol form the monitor as it is being used in VR with controllers.

For future work, we would like to further extend UnityMol in the HTC Vive to include natural user interactions for viewing multiple molecules, vary the electrostatic results from APBS, and change molecular attributes.
We envision this tool will also enable virtual collaboration for participant in different locations.
Each participant will be able to view, gesture and interact with the same data in the same VR space.
Finally, we would like to explore the use of VR for research related to docking of different molecules.
