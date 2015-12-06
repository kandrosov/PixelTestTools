#!/usr/bin/env python
#
#  \file text_to_root.py
#  \brief Convert text output of the thermal camera DAQ to the ROOT TH3D histogram.
#  \author Konstantin Androsov (INFN Pisa)
#
#  Copyright 2015 Konstantin Androsov <konstantin.androsov@gmail.com>
#
#  This file is part of PixelTestTools.
#
#  PixelTestTools is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 2 of the License, or
#  (at your option) any later version.
#
#  PixelTestTools is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with PixelTestTools.  If not, see <http://www.gnu.org/licenses/>.


import sys
import os
import ROOT

print "Usage: txt_to_root.py input_txt_dir output_root_file n_frames time_interval"

if len(sys.argv) != 5:
    sys.exit()

input_txt_dir = sys.argv[1]
output_file_name = sys.argv[2]
n_frames = int(sys.argv[3])
time_interval = int(sys.argv[4])

output_file = ROOT.TFile(output_file_name, 'RECREATE')

[size_x, size_y] = [255, 200]

hist_name = 'temperature'
hist = ROOT.TH3D(hist_name, hist_name, size_x, -0.5, size_x - 0.5, size_y, -0.5, size_y - 0.5,
                 n_frames, 0, time_interval * n_frames)

for n in range(0,n_frames):
    txt_name = '%s/FRAME%03d.txt' % (input_txt_dir,n+1)
    if not os.path.isfile(txt_name):
        print "Error: file '%s' not found." % (txt_name)
        sys.exit()

    f = open(txt_name, 'r')
    header = f.readline()
    [size_x_file, size_y_file] = map(int, header.split())
    if size_x_file != size_x or size_y_file != size_y:
        print 'Unexpected map size'
        sys.exit()

    for x in range(0, size_x):
        line = f.readline().replace(',','.')
        values = map(float, line.split())
        if len(values) != size_y:
            print "ERROR: the length of the input line %d is %d, while is expected to be %d." % \
                (y, len(values), size_y)
            sys.exit()
        for y in range(0, size_y):
            hist.SetBinContent(size_x - x, size_y - y, n + 1, values[y])

hist.Write()
print 'Done.'
