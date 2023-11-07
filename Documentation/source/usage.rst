Usage
=====

Cloning the Repository
----------------------
To clone the repository of R-type run the following command:

.. code-block:: bash

    git clone git@github.com:EpitechPromo2026/B-CPP-500-PAR-5-1-rtype-sofiane.bassaler.git

Compiling the Program
---------------------
To compile the Program, run the following commands:

.. code-block:: bash

    cmake . -DCLIENT_MODE=ON -DSERVER_MODE=ON
    make  # use -j10 to compile faster

Launching the Program
---------------------

To launch the server application, use the following command:

.. code-block:: bash

    ./ServerRTYPE

To launch the client application, use the following command:

.. code-block:: bash

    ./ClientRtype
