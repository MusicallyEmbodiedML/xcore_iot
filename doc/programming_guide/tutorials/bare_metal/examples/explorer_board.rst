##############
Explorer Board
##############

This example application demonstrates various capabilities of the Explorer board.

The example consists of pdm_mics to a simple audio processing pipeline which
applies a variable gain.  Pressing button 0 will increase the gain.  Pressing
button 1 will decrease the gain.  The processed audio is sent to the DAC.

When button 0 is pressed, LED 0 will be lit.  When button 1 is pressed, LED 1
will be lit.  When the gain adjusted audio passes a frame power threshold, LED 2
will be lit.  Lastly, LED 3 will blink periodically.

Additionally, the example demonstrates a simple flash, UART loopback and SPI setup.

**********************
Preparing the hardware
**********************

The UART loopback section of the demo requires that a jumper cable be connected
between X1D36 and X1D39. This connects the Tx pin to the Rx pin.

******************************************
Deploying the firmware with Linux or macOS
******************************************

=====================
Building the firmware
=====================

Run the following commands in the xcore_iot root folder to build the firmware:

.. code-block:: console

    cmake -B build -DCMAKE_TOOLCHAIN_FILE=xmos_cmake_toolchain/xs3a.cmake
    cd build
    make example_bare_metal_explorer_board

====================
Running the firmware
====================

From the build folder run:

.. code-block:: console

    make run_example_bare_metal_explorer_board

================================
Debugging the firmware with xgdb
================================

From the build folder run:

.. code-block:: console

    make debug_example_bare_metal_explorer_board

***********************************
Deploying the firmware with Windows
***********************************

=====================
Building the firmware
=====================

Run the following commands in the xcore_iot root folder to build the firmware:

.. code-block:: console

    cmake -G "NMake Makefiles" -B build -DCMAKE_TOOLCHAIN_FILE=xmos_cmake_toolchain/xs3a.cmake
    cd build
    nmake example_bare_metal_explorer_board

====================
Running the firmware
====================

From the build folder run:

.. code-block:: console

    nmake run_example_bare_metal_explorer_board

================================
Debugging the firmware with xgdb
================================

From the build folder run:

.. code-block:: console

    nmake debug_example_bare_metal_explorer_board

