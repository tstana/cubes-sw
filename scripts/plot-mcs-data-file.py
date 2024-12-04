#!/usr/bin/python3

import argparse
import matplotlib.pyplot as plt

def process(filename):
	print()
	print("Reading from data file \"" + filename + "\"...")
	print()

	with open(filename, "rb") as f:
		# Read the HK elements in the histogram header
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

		f.read(93)

		conf_id = f.read(1)

		bin_cfg = f.read(6)

		bins = range(0, 2048, 1)
		# TODO: Adapt `y` array range for different `bin_cfg`...
		hist = [[] for i in range(6)]

		fig, axs = plt.subplots(3, 2, sharex=True, sharey=True)
		for i in range(0, 6):
			dat = f.read(4096)
			for j in range(0, len(dat), 2):
				hist[i].append((dat[j]<<8)|(dat[j+1]))
			k, l = int(i/2), i%2
			axs[k][l].bar(bins, hist[i], width=32)
		plt.show()

parser = argparse.ArgumentParser(description="Plot histogram data from MIST MCS data file")
parser.add_argument("filename",
	help="DAQ data file obtained from MIST Mission Control Software (Elveti)")
args = parser.parse_args()

process(args.filename)

print()
