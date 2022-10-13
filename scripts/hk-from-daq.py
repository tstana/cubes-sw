#!/usr/bin/python3

def main():
	filename = input("Enter .dat file name: ")
	if (filename[-4:] != ".dat"):
		filename += ".dat"

	print()
	print("Reading from data file \"" + filename + "\"...")
	print()

	with open(filename, "rb") as f:

		# Try reading "Unix time" string from .dat file
		if (bytes.decode(f.read(10), "ascii") != "Unix time:"):
			print("ERR: No \"Unix time\" in data file. Corrupt?")
			return

		# Read past the space, get Unix time at DAQ start and read past "\r\n"
		f.read(1)
		time_end = f.read(10);
		f.read(2)

		# Read the rest of the HK elements in the histogram header
		board_id = bytes.decode(f.read(2), "ascii")

		time_start = int.from_bytes(f.read(4), "big")

		temp_citi_start = float(int.from_bytes(f.read(2), "big"))
		temp_citi_start = ((2.7 - (temp_citi_start*2)/1e3)/8e-3)

		temp_hvps_start = float(int.from_bytes(f.read(2), "big"))
		temp_hvps_start = (temp_hvps_start*1.907e-5 - 1.035) / (-5.5e-3)

		hvps_volt_start = float(int.from_bytes(f.read(2), "big"))
		hvps_volt_start *= 1.812e-3

		hvps_curr_start = float(int.from_bytes(f.read(2), "big"))
		hvps_curr_start *= 5.194e-3

		f.read(114)

		daq_time = int.from_bytes(f.read(2), "big")
		daq_time /= 256
		act_daq_time = int.from_bytes(f.read(2), "big")
		act_daq_time /= 256

		trig_count_0 = int.from_bytes(f.read(4), "big")
		trig_count_16 = int.from_bytes(f.read(4), "big")
		trig_count_31 = int.from_bytes(f.read(4), "big")
		trig_count_or32 = int.from_bytes(f.read(4), "big")

		temp_citi_end = float(int.from_bytes(f.read(2), "big"))
		temp_citi_end = ((2.7 - (temp_citi_end*2)/1e3)/8e-3)

		temp_hvps_end = float(int.from_bytes(f.read(2), "big"))
		temp_hvps_end = (temp_hvps_end*1.907e-5 - 1.035) / (-5.5e-3)

		hvps_volt_end = float(int.from_bytes(f.read(2), "big"))
		hvps_volt_end *= 1.812e-3

		hvps_curr_end = float(int.from_bytes(f.read(2), "big"))
		hvps_curr_end *= 5.194e-3

		f.read(91)

		bin_cfg = f.read(6)

		print("Board ID: " + board_id)
		print()
		print("DAQ start...")
		print("  Unix time     : " + str(time_start))
		print("  Citiroc temp. : " + "%.3f" % temp_citi_start + " C")
		print("  HVPS temp.    : " + "%.3f" % temp_hvps_start + " C")
		print("  HVPS voltage  : " + "%.3f" % hvps_volt_start + " V")
		print("  HVPS current  : " + "%.3f" % hvps_curr_start + " mA")
		print()
		print("DAQ end...")
		print("  Unix time     : " + bytes.decode(time_end, "ascii"))
		print("  DAQ time      : " + "%.1f" % daq_time + " s")
		print("  Live time     : " + "%.1f" % act_daq_time + " s")
		print("  Ch. 0 Hits    : " + str(trig_count_0))
		print("  Ch. 16 Hits   : " + str(trig_count_16))
		print("  Ch. 31 Hits   : " + str(trig_count_31))
		print("  OR32 Hits     : " + str(trig_count_or32))
		print("  HVPS temp.    : " + "%.3f" % temp_hvps_end+ " C")
		print("  HVPS voltage  : " + "%.3f" % hvps_volt_end+ " V")
		print("  HVPS current  : " + "%.3f" % hvps_curr_end+ " mA")
		print()
		print("Bin config's...")
		print("  HG, Ch. 0     : " + str(int(bin_cfg[0])))
		print("  LG, Ch. 0     : " + str(int(bin_cfg[1])))
		print("  HG, Ch. 16    : " + str(int(bin_cfg[2])))
		print("  LG, Ch. 16    : " + str(int(bin_cfg[3])))
		print("  HG, Ch. 31    : " + str(int(bin_cfg[4])))
		print("  LG, Ch. 31    : " + str(int(bin_cfg[5])))

if __name__ == "__main__":
	main()
	print()
