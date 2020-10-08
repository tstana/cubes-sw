# Charge Linearity Test Program using Rigol DG1062Z Pulse Generator

This program can be used to command a pulse generator for sending out a 1-us
pulse signal in an "amplitude sweep": pulse signals at increasing amplitudes.
The frequency is kept constant at **1 kHz** and the pulse width at **1 us**.
A higher amplitude based on the user's selection is output every 5 seconds.

The figure below shows the pulse sweep that will be created by the program.

(fig/ampl-sweep-example.png)

The intended application for the program is to test the linearity and range
of the Citiroc HG and LG chains via the IN_CALIB input on the Citiroc, hence
the "Charge Linearity" in the name of the program.

SCPI commands to the pulse generator are used to achieve the pulse sweep. This
program can also be used as an example of sending SCPI commands to instruments
using Visual Studio and the NI-VISA .NET drivers.

# Getting Started

1. Download and install Visual Studio Community version. (VS2017 was used for
   developing this software, but any version will work, presumably.)
2. Download the IVI drivers from the [Rigol website](https://beyondmeasure.rigoltech.com/acton/attachment/1579/f-076a/1/-/-/-/-/DG1000Z%28IVI_DRV%29update_1.0.0.zip).
3. Connect the DG1062Z pulse generator to a USB port on your PC.
4. Double-click the solution (`.sln`) file for the project.
5. Put VS in release mode.
6. Build and run the code via the "Start" button.
7. The program should execute in a command line window.

If the program doesn't build, the fun starts... Use the VS output pane for
details on why the program didn't build, go through the code, etc.

One potential build issue that could occur is the IVI drivers not being
found. If this happens, edit the reference path to the IVI driver install
location. The process for this is:

- **Right-click** the `charge-lin-test-dg1062z` project in the **Solution
  Explorer** in VS
- Click **Properties**
- Navigate to **Reference Paths**
- Click **Browse** to find the path to the IVI drivers on your computer.
