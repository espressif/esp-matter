# /*************************************************************************//**
#  * @file
#  * @brief Script to generate customer PA curve fits that map from
#  *        power levels to actual powers for the EFR32.
#  * @details
#  *   To use this file:
#  *   1. Load railtest configured to the desired PA onto your chip which
#  *      is mounted onto the board you are trying to characterize.
#  *   2. In railtest, run the "sweepTxPower" command. As you sweep through
#  *      the power levels, record the dBm output from a spectrum analyzer
#  *      into the chip specific csv file, depending
#  *      on the PA you are trying to characterize.
#  *   3. Run this python script from the same directory as the csv file.
#  *   4. Make a copy of pa_curves_efr32.h, copy the results of this python
#  *      script into the appropriate macro in that file. If you only need one
#  *      PA, you only need to copy the data for that PA, not both.
#  *   5. Update HAL_PA_CURVE_HEADER to point your new file.
#  *****************************************************************************
#  * # License
#  * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
#  *****************************************************************************
#  *
#  * SPDX-License-Identifier: Zlib
#  *
#  * The licensor of this software is Silicon Laboratories Inc.
#  *
#  * This software is provided 'as-is', without any express or implied
#  * warranty. In no event will the authors be held liable for any damages
#  * arising from the use of this software.
#  *
#  * Permission is granted to anyone to use this software for any purpose,
#  * including commercial applications, and to alter it and redistribute it
#  * freely, subject to the following restrictions:
#  *
#  * 1. The origin of this software must not be misrepresented; you must not
#  *    claim that you wrote the original software. If you use this software
#  *    in a product, an acknowledgment in the product documentation would be
#  *    appreciated but is not required.
#  * 2. Altered source versions must be plainly marked as such, and must not be
#  *    misrepresented as being the original software.
#  * 3. This notice may not be removed or altered from any source distribution.
#  *
#  ****************************************************************************/

from __future__ import print_function
from __future__ import division
import os
import sys
import argparse
import numpy
try:
  from matplotlib import pyplot as plt
except ImportError:
  pass

RAIL_TX_POWER_LEVEL_INVALID =  255
API_MAX_POWER_DEFAULT  = 20
API_INCREMENT_DEFAULT = 4
NUM_SEGMENTS_DEFAULT = 8
API_MIN_POWER = -50
enablePlotting = False
minPowerLevel = 255
maxPowerLevel = 255

def FitAndPlotPower(actPower, yAxisValues, min=-100, max=100):
  filtPwr=[]
  filtYAxisValues=[]
  x = 0
  minYAxisValue = 10000
  maxYAxisValue = -1
  errorflag = 0
  for pwr in actPower:
    # Select which key value pairs to use for this segment based on their power
    if (min - 1) <= pwr <= (max):
      filtPwr.append(actPower[x])
      filtYAxisValues.append(yAxisValues[x])
      if actPower[x] >= min and yAxisValues[x] < minYAxisValue:
        minYAxisValue = yAxisValues[x]
      if actPower[x] <= max and yAxisValues[x] > maxYAxisValue:
        maxYAxisValue = yAxisValues[x]

    x += 1
  MSE = 99999
  if (len(filtPwr) <= 1) or (len(filtYAxisValues) <=1):
    errorflag = 1
  # Do the actual curve fit
  if filtPwr:
    if enablePlotting:
      fig = plt.plot(filtPwr, filtYAxisValues)
    coefficients = numpy.polyfit(filtPwr, filtYAxisValues, 1)
    polynomial = numpy.poly1d(coefficients)
    ys = polynomial(filtPwr)
    MSE = numpy.square(numpy.subtract(filtYAxisValues,ys)).mean()
    if enablePlotting:
      fig = plt.plot(filtPwr, ys, label=polynomial)
  else:
    polynomial = [0, 0]

  return minYAxisValue, maxYAxisValue, polynomial,MSE,errorflag

def GenCArrayFromPolys(polylist):
  # build the C array string
  # param polylist is [minYAxisValue, maxYAxisValue, 1st degree polycoeff,...]
  curveSegments = []
  for i in range(0, len(polylist), 3):
    if polylist[i] == RAIL_TX_POWER_LEVEL_INVALID :
      curveSegments.append(CurveSegment(polylist[i], int(polylist[i+1]*10), int(polylist[i+2]*10)))
      continue
    maxpowerlevel = polylist[i+1]
    pwrcoeff = polylist[i+2]
    curveSegments.append(CurveSegment(int(maxpowerlevel), int(pwrcoeff[1] * 100), int(pwrcoeff[0] * 1000)))
  global minPowerLevel
  minPowerLevel = polylist[len(polylist)-3]
  return curveSegments

def StringFromCurveSegments(curveSegments):
  arrayStr = "{ "
  length = 0

  # For formatting purposes, go through and find the longest string so we know what to
  # pad to
  for i in range(0, len(curveSegments)):
    segment = curveSegments[i]
    pwrStr = "{{{}, {}, {}}},".format(segment.maxValue, segment.slope, segment.intercept)

    if len(pwrStr) > length:
      length = len(pwrStr)

  for i in range(0, len(curveSegments)):
    segment = curveSegments[i]
    pwrStr = "{{ {}, {}, {} }},".format(segment.maxValue, segment.slope, segment.intercept)

    # Apply the padding width to each line
    pwrStr = pwrStr.ljust(length + 5)

    # Align matrix rows
    if i > 0:
      pwrStr = "  " + pwrStr

    pwrStr += "\\\n"
    arrayStr += pwrStr

  # Format the last part of the string correctly
  while arrayStr[len(arrayStr) - 1] != '}':
    arrayStr = arrayStr[:-1]
  arrayStr += " }"

  return arrayStr

def AdjustMaxValues(curveSegments):
  global RAIL_TX_POWER_LEVEL_INVALID
  start = 1
  if curveSegments[0].maxValue == RAIL_TX_POWER_LEVEL_INVALID :
    start = start + 1
  for i in range(start, len(curveSegments)):
    if curveSegments[i].slope != curveSegments[i-1].slope and curveSegments[i-1].slope != 0 and curveSegments[i].slope != 0:
      # Adjust the max values so that they overlap where the curves actually intercept
      x_intercept = int((curveSegments[i-1].intercept - curveSegments[i].intercept)
                     // (curveSegments[i].slope - curveSegments[i-1].slope))
      curveSegments[i].maxValue = min(curveSegments[i-1].maxValue,
                                      (curveSegments[i].slope * x_intercept + curveSegments[i].intercept + 500) // 1000)

  global maxPowerLevel
  maxPowerLevel = curveSegments[start-1].maxValue
  return curveSegments

def FitCharData(csvFile, increment=API_INCREMENT_DEFAULT, maxpwr=API_MAX_POWER_DEFAULT, numSegment=NUM_SEGMENTS_DEFAULT):
  fitResult = ProcessCharDataAndFindPoly(csvFile, increment , maxpwr, numSegment)

  cStr = ""
  cStr += '\nRAIL_TxPowerCurveSegment_t[] C Structure\n'
  cStr += StringFromCurveSegments(AdjustMaxValues(fitResult))
  cStr += '\n'
  return '\n' + cStr

def ProcessCharDataAndFindPoly(filename, increment=API_INCREMENT_DEFAULT, maxpwr=API_MAX_POWER_DEFAULT, numSegment=NUM_SEGMENTS_DEFAULT):
  data = ReadAndProcessCharData(filename, maxpwr)
  polys = CalcPowerPolys(data.pwrlvls, data.outpwrs, increment, maxpwr, numSegment)
  return polys

def ReadAndProcessCharData(filename, maxpwr=API_MAX_POWER_DEFAULT):
  chardata = numpy.loadtxt(filename, delimiter=',')
  pwrlvls = []
  outpwrs = []
  maxpower = None

  # Average powers in case users provide a list of dBm sample for each level
  for entry in chardata:
    pwrlvls.append(entry[0])
    avgpower = numpy.average(entry[1:len(entry)])
    outpwrs.append(avgpower)
    if maxpower == None or avgpower > maxpower:
      maxpower = avgpower

  if maxpower > maxpwr:
    for x in range(0, len(pwrlvls)):
      outpwrs[x] -= (maxpower - maxpwr)

  return PaData(pwrlvls, outpwrs, maxpower)

scriptPath = os.path.dirname(__file__)

class PaData():
  def __init__(self, pwrlvls=None, outpwrs=None, maxpower=None):
    self.pwrlvls = pwrlvls
    self.outpwrs = outpwrs
    self.maxpower = maxpower

class CurveSegment():
  def __init__(self, maxValue, slope, intercept):
    self.maxValue = maxValue
    self.slope = slope
    self.intercept = intercept

  def __eq__(self, other):
    return (self.maxValue == other.maxValue \
            and self.slope == other.slope \
            and self.intercept == other.intercept)

def CalcPowerPolys(yAxisValues, powers, increment, maxpwr, numSegment=NUM_SEGMENTS_DEFAULT):
  polylist = []
  pwr = maxpwr
  numberOfSegments = numSegment
  #Add extra segment to store maxpwr and increment
  
  if (maxpwr != API_MAX_POWER_DEFAULT) or (increment != API_INCREMENT_DEFAULT):
    numberOfSegments = numberOfSegments+1 
  
  for x in range(0, numberOfSegments):
    
    if x == 0 and (numberOfSegments != numSegment):
      polylist.append(RAIL_TX_POWER_LEVEL_INVALID)
      polylist.append(maxpwr)
      polylist.append(increment)
      continue
    
    lowerBound = pwr - increment
    if x == (numberOfSegments - 1):
      # Large negative number, want to avoid float('-inf') due to arithmetic in
      # FitAndPlotPower
      lowerBound = -99999
    minYAxisValue, maxYAxisValue, polycoeff, MSE, errorflag = FitAndPlotPower(powers, yAxisValues, lowerBound, pwr)
    if errorflag == 1 :
      print("WARNING :: segment {} is not generated properly".format(x))
      print("For better fit of the curve, change maxPower (-m) or increment(-i)\n \
      or combination of both according to the data.")
    pwr -= increment
    polylist.append(int(minYAxisValue))
    polylist.append(int(maxYAxisValue))
    polylist.append(polycoeff)

    if pwr < API_MIN_POWER:
      break

  return GenCArrayFromPolys(polylist)

def main():
  global enablePlotting
  global API_INCREMENT_DEFAULT
  global API_MAX_POWER_DEFAULT
  parser = argparse.ArgumentParser(description="Utility to fit PA data to a\
  curve that can be used in RAIL.")
  parser.add_argument('csvFile',
                      type=str,
                      help="The input CSV file to parse for power information")
  parser.add_argument('-i', '--increment',
                      type=float,
                      default=API_INCREMENT_DEFAULT,
                      required=False,
                      help="The step size (in dBm rounded to 1 decimal place) to segment the power range. ")
  parser.add_argument('-m', '--maxPower',
                      type=int,
                      default=API_MAX_POWER_DEFAULT,
                      required=False,
                      help="The maximum power(in integer dBm) the curve should fit.")
  parser.add_argument('-n', '--numSegment',
                      type=int,
                      default=NUM_SEGMENTS_DEFAULT,
                      required=False,
                      help="The number of curve segments to fit the data.")
  parser.add_argument('-o', '--output',
                      type=str,
                      default=None,
                      required=False,
                      help="The output file to print the results to. Will \
                      print to stdout by default.")
  parser.add_argument('-p', '--plot',
                      action='store_true',
                      help="Pass this option to enable plotting the curve \
                      fit for visual inspection")
  a = parser.parse_args()

  # Only use matplotlib if we're graphing results
  enablePlotting = a.plot
  if enablePlotting and 'plt' not in globals():
    print("Error: matplotlib is required for plotting results")
    return 1

  # Compute the fit and output the result
  output = FitCharData(a.csvFile, round(a.increment,1), a.maxPower, a.numSegment)
  if a.output == None:
    print(output)
  else:
    with open(a.output, 'w') as f:
      f.write(output)
  if (minPowerLevel == 255) or (maxPowerLevel == 255):
    print("WARNING :: Curve doesn't have correct min/max powerlevels.")
  print("\nMax Powerlevel: %d, Min Powerlevel: %d" %(minPowerLevel, maxPowerLevel))
  # Show the plot of the curve fit if requested
  if enablePlotting:
    plt.show()

  return 0

if __name__ == '__main__':
  sys.exit(main())
