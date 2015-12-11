#!/usr/bin/env python
#
#  \file temp_study.py
#  \brief Analysis of thermal images used for the bare modules assembly procedure qualification.
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
import ROOT

print "Usage: temp_study.py input_file output_file"

if len(sys.argv) != 3:
    sys.exit()

input_file_name = sys.argv[1]
output_file_name = sys.argv[2]

input_file = ROOT.TFile(input_file_name, 'READ')
data = input_file.Get("temperature")

canvas = ROOT.TCanvas("c", "c", 50, 50, 700, 700)
canvas.Print(output_file_name + '[')
canvas.cd()

time_slices = [ 1, 5, 10, 15, 20 ]

ROOT.gStyle.SetOptStat(0)
for t_slice in time_slices:
    t = data.GetZaxis().GetBinLowEdge(t_slice)
    data.GetZaxis().SetRange(t_slice, t_slice)
    proj = data.Project3D('yx')

    title = 'Temperature at t = %d min' % (t/60)

    proj.SetTitle(title)
    proj.Draw('colz')

    canvas.Draw()
    canvas.Print(output_file_name, 'Title:' + title)


# [up_region_x, up_region_y] = [35, 122]
# [down_region_x, down_region_y] = [35, 60]
# [region_size_x, region_size_y] = [170, 15]
# [control_region_x, control_region_y, control_region_size_x, control_region_size_y] = [118, 192, 6, 8]
# nbins = [20, 30, 40, 40, 40]
# limits = [ [21, 24], [21, 60], [21, 70], [21, 70], [21, 70] ]
# sf = [1.3, 2.2, 2, 1.8, 2]

[up_region_x, up_region_y] = [45, 99]
[down_region_x, down_region_y] = [45, 39]
[region_size_x, region_size_y] = [178, 15]
[control_region_x, control_region_y, control_region_size_x, control_region_size_y] = [131, 179, 8, 21]
nbins = [20, 30, 40, 40, 40]
limits = [ [20, 24], [20, 40], [20, 40], [20, 40], [20, 40] ]
sf = [1.3, 1.5, 2, 1.8, 2]


for n in range(0, len(time_slices)):
    t_slice = time_slices[n]
    t = data.GetZaxis().GetBinLowEdge(t_slice)

    title = 'Temperature in the selected regions at t = %d min' % (t/60)
    [xmin, xmax] = limits[n]
    hist_down = ROOT.TH1D("down" + str(t_slice), title, nbins[n], xmin, xmax)
    hist_up = ROOT.TH1D("up" + str(t_slice), title, nbins[n], xmin, xmax)
    hist_control = ROOT.TH1D("control" + str(t_slice), title, nbins[n], xmin, xmax)

    for x in range(down_region_x, down_region_x + region_size_x + 1):
        for y in range(down_region_y, down_region_y + region_size_y + 1):
            hist_down.Fill(data.GetBinContent(x, y, t_slice))

    for x in range(up_region_x, up_region_x + region_size_x + 1):
        for y in range(up_region_y, up_region_y + region_size_y + 1):
            hist_up.Fill(data.GetBinContent(x, y, t_slice))

    for x in range(control_region_x, control_region_x + control_region_size_x + 1):
        for y in range(control_region_y, control_region_y + control_region_size_y + 1):
            hist_control.Fill(data.GetBinContent(x, y, t_slice))


    hist_down.Scale(1./hist_down.Integral())
    hist_down.SetLineColor(ROOT.kBlue)
    hist_down.SetLineWidth(2)
    hist_down.GetXaxis().SetTitle('Temperature, deg C')
    hist_down.SetMaximum(hist_down.GetMaximum() * sf[n])
    hist_down.Draw()

    hist_up.Scale(1./hist_up.Integral())
    hist_up.SetLineColor(ROOT.kRed)
    hist_up.SetLineWidth(2)
    hist_up.Draw('same')

    hist_control.Scale(1./hist_control.Integral())
    hist_control.SetLineColor(ROOT.kBlack)
    hist_control.SetLineWidth(2)
    hist_control.Draw('same')

    legend = ROOT.TLegend(0.75, 0.70, 0.95, 0.85)
    legend.SetTextSize(0.035)
    legend.SetTextFont(42)
    legend.SetBorderSize(0)
    legend.AddEntry(hist_down, 'Down region', 'l')
    legend.AddEntry(hist_up, 'Up region', 'l')
    legend.AddEntry(hist_control, 'Ctrl region', 'l')
    legend.Draw('same')

    canvas.Draw()
    canvas.Print(output_file_name, 'Title:' + title)

title = 'Average temperature evolution in the selected regions'
data.GetZaxis().SetRange()
hist_down_time = data.ProjectionZ("down_time", up_region_x, up_region_x + region_size_x,
                                               up_region_y, up_region_y + region_size_y)
hist_up_time = data.ProjectionZ("up_time", down_region_x, down_region_x + region_size_x,
                                           down_region_y, down_region_y + region_size_y)
hist_control_time = data.ProjectionZ("control_time", control_region_x, control_region_x + control_region_size_x,
                                                     control_region_y, control_region_y + control_region_size_y)

hist_down_time.Scale(1.0/((region_size_x + 1.0) * (region_size_y + 1.0)))
hist_up_time.Scale(1.0/((region_size_x + 1.0) * (region_size_y + 1.0)))
hist_control_time.Scale(1.0/((control_region_size_x + 1.0) * (control_region_size_y + 1.0)))

hist_down_time.SetLineColor(ROOT.kBlue)
hist_down_time.SetLineWidth(2)
hist_down_time.GetXaxis().SetTitle('Time, sec')
hist_down_time.GetYaxis().SetTitle('Temperature, deg C')
hist_down_time.SetTitle(title)
hist_down_time.Draw()

hist_up_time.SetLineColor(ROOT.kRed)
hist_up_time.SetLineWidth(2)
hist_up_time.Draw('same')

hist_control_time.SetLineColor(ROOT.kBlack)
hist_control_time.SetLineWidth(2)
hist_control_time.Draw('same')

legend = ROOT.TLegend(0.75, 0.50, 0.95, 0.65)
legend.SetTextSize(0.035)
legend.SetTextFont(42)
legend.SetBorderSize(0)
legend.AddEntry(hist_down, 'Down region', 'l')
legend.AddEntry(hist_up, 'Up region', 'l')
legend.AddEntry(hist_control, 'Ctrl region', 'l')
legend.Draw('same')

canvas.Draw()
canvas.Print(output_file_name, 'Title:' + title)

canvas.Clear()
canvas.Print(output_file_name, 'Title:empty')
canvas.Print(output_file_name + ']')
