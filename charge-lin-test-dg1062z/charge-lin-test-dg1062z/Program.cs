using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;

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

            /// Ensure the US "culture" is used, so a dot is correctly sent to
            /// the pulse generator as a decimal separator. (SCPI commands use
            /// English-style decimal separators -- dots).
            CultureInfo culture = new CultureInfo("en-US");
            Thread.CurrentThread.CurrentCulture = culture;

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

            try
            {
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

                    Console.WriteLine("You will now be asked to input the start and end voltage for the pulses.");
                    Console.WriteLine("Please use dots as decimal separators!");
                    Thread.Sleep(2000);

                    while (true)
                    {
                        Console.WriteLine();
                        Console.Write("Please input start voltage (V): ");
                        startVoltage = Convert.ToDouble(Console.ReadLine());
                        Console.Write("Please input end voltage (V): ");
                        endVoltage = Convert.ToDouble(Console.ReadLine()); ;
                        Console.Write("Please input voltage increase on each step (V): ");
                        voltageStep = Convert.ToDouble(Console.ReadLine());

                        if (startVoltage > endVoltage)
                        {
                            Console.WriteLine("Starting voltage cannot be higher than ending voltage!");
                            Console.WriteLine("Please try again...");
                            Thread.Sleep(1000);
                            continue;
                        }

                        if (voltageStep > endVoltage - startVoltage)
                        {
                            Console.WriteLine("Voltage step must be between end voltage and start voltage!");
                            Console.WriteLine("Please try again...");
                            Thread.Sleep(1000);
                            continue;
                        }

                        break;
                    }

                    Console.WriteLine();
                    Console.WriteLine("Selected voltages translate in the following charge range: [" +
                        CAP * startVoltage + ", " + CAP * endVoltage + "] pF, in " +
                        CAP * voltageStep + " pF increments.");

                    Thread.Sleep(1000);

                    Console.WriteLine();
                    Console.WriteLine("If this is incorrect, hit <Ctrl+C> on your keyboard to end the program");
                    Console.WriteLine("and start over...");
                    Console.WriteLine();

                    Thread.Sleep(2000);

                    Console.WriteLine("Start your DAQ for at least " +
                        + Math.Ceiling(5 * (voltageStep + endVoltage - startVoltage) / voltageStep) +
                        "s...");
                    Console.WriteLine();

                    Thread.Sleep(2000);

                    Console.Write("Press ENTER key to start applying pulses...");
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
                        /// Command format:
                        ///   :APPL:PULS [width];[voltage];[offset];[delay]
                        /// 
                        /// For the DG1062, [offset] = [voltage]/2, so that
                        /// the pulse has its baseline at 0 V.
                        cmd = ":APPL:PULS 1000," + i.ToString("N3") +
                                             "," + (i / 2).ToString("N3") +
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
            }
            catch (Ivi.Visa.NativeVisaException e)
            {
                Console.WriteLine("Exception thrown by the VISA driver:");
                Console.WriteLine();
                Console.WriteLine("    " + e.Message);
                Console.WriteLine();
                Console.WriteLine("Perhaps pulse generator not connected to a USB port?");
                Console.WriteLine();
            }

            Console.WriteLine();
            Console.WriteLine("Press ENTER key to exit.");
            Console.ReadKey();
        }
    }
}
