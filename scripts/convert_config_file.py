#!/usr/bin/python3
from datetime import date
import math
import binascii
import sys
import os
import argparse


def Generator(s, n):
	for start in range(0, len(s)-1, n):
		yield s[start:start+n]

def invert_bits(s):
	return s[::-1]

GCC_HEADER = """
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
"""

def write_defconfig(inputname):
	f = open(inputname, 'r')
	conf = f.readline()
	f.close()

	OUTPUTNAME = "citiroc_defconfig.h"

	g = open(OUTPUTNAME, 'w')

	g.write("/*\n")
	g.write(" * Default Citiroc Configuration\n")
	g.write(" *\n")
	g.write(" * Generated from " + os.path.basename(inputname) + " on " +
	    str(date.today()) + " using\n" +
		" * " + os.path.basename(__file__) + "\n")
	g.write(" *")
	g.write(GCC_HEADER)
	g.write(" */\n")
	g.write('\n')

	g.write("const uint8_t CITIROC_DEFCONFIG[144] = {")

	i = 0;
	for y in Generator(conf, 8):
		if i%8 == 0:
			g.write('\n')
			g.write('\t')
		else:
			g.write(' ')
		y = (int(invert_bits(y), 2))
		g.write("0x{0:02x}".format(y) + ',')
		i += 1

	g.write(" 0x00")  # CONF_ID field
	g.write("\n}")
	g.write('\n')

	g.close()

	print("Wrote successfully to " + OUTPUTNAME);

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Convert Citiroc .conf file " +
		"expoted by CitirocUI to default configuration string to be applied " +
		" by MSS on CUBES initialization.")
	parser.add_argument("filename", help=".conf file exported by CitirocUI")
	args = parser.parse_args()

	write_defconfig(args.filename)
