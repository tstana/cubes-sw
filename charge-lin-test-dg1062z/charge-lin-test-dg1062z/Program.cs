using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

using NationalInstruments.Visa;

namespace charge_lin_test_dg1062z
{
    class Program
    {
        /// <summary>
        /// Apply subsequent commands to a pulse generator to achieve an "amplitude
        /// sweep" for a 1-us wide pulse signal at 1 kHz frequency.
        /// 
        /// If changes are needed:
        ///   1. Replace VISAID for your instrument's ID, if different.
        ///   2. Make sure the serial number matches the VISAID in the call to
        ///      resp.Contains().
        ///   2. Replace the CAP variable for the capacitance you are using, if
        ///      different from 100 pF.
        ///      
        /// </summary>
        /// <param name="args"></param>
        static void Main(string[] args)
        {
            ///
            /// Change these if needed... 
            ///
            const string VISAID = "USB0::0x1AB1::0x0642::DG1ZA194405466::INSTR";
            const double CAP = 100; // pF
            ///
            ///
            ///

            double startVoltage, endVoltage, voltageStep;

            Console.WriteLine("================================================================================");
            Console.WriteLine("  Charge Linearity Test using DG1062Z Pulse Generator");
            Console.WriteLine("================================================================================");
            Console.WriteLine();

            Console.WriteLine("Attempting to connect to pulse generator with USB ID:");
            Console.WriteLine("    " + VISAID);
            Console.WriteLine("    (If you are using another pulse generator, " +
                "substitute the ID in the code ");
            Console.WriteLine("     before running...)");
            Console.WriteLine();

            UsbSession sesh = new UsbSession(VISAID);

            var pgen = sesh.RawIO;
            string cmd, resp;

            Console.WriteLine("Sending ID query to pulse generator...");
            Console.WriteLine();

            Console.WriteLine(">>> *IDN?");
            pgen.Write("*IDN?");
            resp = Encoding.ASCII.GetString(pgen.Read());
            Console.Write("<<< ");
            Console.WriteLine(resp);

            if (resp.Contains("DG1ZA194405466"))    // NOTE: Should be same as VISAID!
            {
                Console.WriteLine("Instrument serial number matches USB ID - proceeding!");
                Console.WriteLine();

                Console.Write("Please input start voltage (V): ");
                startVoltage = Convert.ToDouble(Console.ReadLine());
                Console.Write("Please input end voltage (V): ");
                endVoltage = Convert.ToDouble(Console.ReadLine()); ;
                Console.Write("Please input voltage increase on each step: ");
                voltageStep = Convert.ToDouble(Console.ReadLine());

                Console.WriteLine();
                Console.WriteLine("Selected voltages translate in the following charge range: [" +
                    CAP*startVoltage + ", " + CAP*endVoltage +"] pF.");

                Thread.Sleep(2000);

                Console.WriteLine();
                Console.WriteLine("If this is incorrect, hit <Ctrl+C> on your keyboard to end the program");
                Console.WriteLine("and start over...");                
                Console.WriteLine();

                Thread.Sleep(2000);

                Console.WriteLine("Start your DAQ for at least " + 
                    + (5*(voltageStep + endVoltage-startVoltage)/voltageStep) + "s.");

                Thread.Sleep(2000);

                Console.Write("Press any key to start applying pulses...");
                Console.ReadLine();

                Console.WriteLine("Applying 1-us pulse signals at 1 kHz...");
                Console.WriteLine();

                cmd = ":FUNC:PULS:WIDTH 1e-6";
                Console.WriteLine(">>> " + cmd);
                pgen.Write(cmd);

                cmd = ":OUTP:STATE ON";
                Console.WriteLine(">>> " + cmd);
                pgen.Write(cmd);

                for (double i = startVoltage; i <= endVoltage; i += voltageStep)
                {
                    cmd = ":APPL:PULS 1000," + i.ToString("N3") +
                                         "," + (i/2).ToString("N3") +
                                         ",0";
                    Console.WriteLine(">>> " + cmd);
                    pgen.Write(cmd);

                    Thread.Sleep(5000);
                }

                cmd = ":OUTP:STATE OFF";
                Console.WriteLine(">>> " + cmd);
                pgen.Write(cmd);
            }

            // End of program.
            sesh.Dispose();

            Console.WriteLine();
            Console.WriteLine("Press any key to continue...");
            Console.ReadKey();
        }
    }
}
