#!/usr/bin/env python3

""" Positioning visualizer tool """

import argparse
import json
import os
import queue
import re
import random
import sys
import time

from pyqtgraph.Qt import QtCore, QtGui
import pyqtgraph.opengl as gl
import pyqtgraph as pg
import numpy as np

from PIL import Image
import paho.mqtt.client as mqtt

WINDOW_WIDTH = 1024 # * 2
WINDOW_HEIGHT = 768 # * 2

MAX_NUM_TAGS = 300

PLOT_VIEW_3D = 1            # if 0, uses 2D view
PLOT_TAG_ADDRESSES = 1      # show tag address next to marker
PLOT_ACCURACY = 0           # tag marker size depends on accuracy

PLOT_ROOM = 0               # only available in 3D view
PLOT_DEBUG_LINES = 1        # only available in 3D view
PLOT_DEBUG_LOCATORS = 1     # only available in 3D view
PLOT_MARKER_TRACES = 0      # only available in 3D view

MAX_NUM_TAG_LINES = 2       # if PLOT_DEBUG_LINES == 1, show lines to this many tags
MAX_NUM_LOCATOR_LINES = 5   # if PLOT_DEBUG_LINES == 1, show lines from this many locators

DEFAULT_CONFIG = os.path.join(os.path.dirname(__file__), "../bt_host_positioning/config/positioning_config.json")
DEFAULT_CONNECTION = {"host": "localhost", "port": 1883}

pg.setConfigOptions(enableExperimental=True, useOpenGL=True)

def Rx(theta):
  return np.array([[ 1, 0            , 0            ],
                   [ 0, np.cos(theta),-np.sin(theta)],
                   [ 0, np.sin(theta), np.cos(theta)]])
def Ry(theta):
  return np.array([[ np.cos(theta), 0, np.sin(theta)],
                   [ 0            , 1, 0            ],
                   [-np.sin(theta), 0, np.cos(theta)]])
def Rz(theta):
  return np.array([[ np.cos(theta), -np.sin(theta), 0 ],
                   [ np.sin(theta), np.cos(theta) , 0 ],
                   [ 0            , 0             , 1 ]])

class TextGLViewWidget(gl.GLViewWidget):
  def __init__(self):
    super(TextGLViewWidget, self).__init__()
    self.textPositions = {} # Key shall be the text to be displayed and value shall be [x, y, z]
    if "renderText" not in dir(self):
      print("WARNING! Text rendering is not supported in 3D mode. Please try to use pyqtgraph 0.11.0")

  def paintGL(self, *args, **kwds):
    gl.GLViewWidget.paintGL(self, *args, **kwds)
    for text, pos in self.textPositions.items():
      try:
        self.renderText(pos[0], pos[1], pos[2], text)
      except:
        pass

  def setText(self, text, pos):
    self.textPositions[text] = pos

class Visualizer(object):
  def __init__(self):
    self.marker_trace = False
    if PLOT_MARKER_TRACES:
      self.marker_trace = True

    self.markerTraces = []
    self.markerTraceStep = 10
    self.numMarkerTraces = 20
    self.numEstimatesPlotted = 0
    if PLOT_VIEW_3D:
      self.plotlines = PLOT_DEBUG_LINES
    else:
      self.plotlines = 0

    self.app = QtGui.QApplication(sys.argv)

    self.w = None
    self.view = None

    if PLOT_VIEW_3D:
      self.w = TextGLViewWidget()
      self.w.opts['distance'] = 25
      self.w.setGeometry(200, 200, WINDOW_WIDTH, WINDOW_HEIGHT)
      self.w.orbit(225, 90)
      self.w.show()

      planeColor = [226.0 / 255.0, 205.0 / 255.0, 155.0 / 255.0, 0.5]
      z = np.zeros((20, 20))
      p1 = gl.GLSurfacePlotItem(z=z, shader='shaded', color=planeColor, glOptions='additive')
      p1.translate(-10, -10, 0)
      self.w.addItem(p1)

      xgrid = gl.GLGridItem(glOptions='additive')
      xgrid.rotate(90, 0, 1, 0)
      ygrid = gl.GLGridItem(glOptions='additive')
      ygrid.rotate(90, 1, 0, 0)
      zgrid = gl.GLGridItem(glOptions='additive')

#      self.w.addItem(xgrid)
#      self.w.addItem(ygrid)
      self.w.addItem(zgrid)

      axis = gl.GLAxisItem()
      axis.setSize(x=15, y=15, z=15)
      self.w.addItem(axis)

      if PLOT_DEBUG_LOCATORS:
        im = Image.open(r"img/locator.png")
        self.img_tex_front = np.array(im)
        imB = Image.open(r"img/locator_back.png")
        self.img_tex_back = texB = np.array(imB)
        self.img_scale = 4000.0

    else:
      # PLOT_VIEW_2D
      self.view = pg.PlotWidget()
      self.view.showGrid(x=True, y=True)
      self.view.resize(WINDOW_WIDTH, WINDOW_HEIGHT)
      self.view.setAspectLocked()
      self.view.show()
      self.view.setXRange(-15, 15)
      self.view.setYRange(-15, 15)

    self.q_pos = queue.Queue()
    self.q_ang = queue.Queue()
    self.tags = {}
    self.locators = {}
    self.positioning_id = None

  def start(self):
    if (sys.flags.interactive != 1) or not hasattr(QtCore, 'PYQT_VERSION'):
        QtGui.QApplication.instance().exec_()

  def plot_room(self):
    # Only supported in 3D mode

    if PLOT_VIEW_3D:
      # This is an example on how to plot simple objects into the 3D view
      vertexes = np.array([[1, 0, 0], #0
                       [0, 0, 0], #1
                       [0, 1, 0], #2
                       [0, 0, 1], #3
                       [1, 1, 0], #4
                       [1, 1, 1], #5
                       [0, 1, 1], #6
                       [1, 0, 1]], dtype=int)#7

      faces = np.array([[1,0,7], [1,3,7],
                    [1,2,4], [1,0,4],
                    [1,2,6], [1,3,6],
                    [0,4,5], [0,7,5],
                    [2,4,5], [2,6,5],
                    [3,6,5], [3,7,5]])

      shelfColor = [226.0 / 255.0, 205.0 / 255.0, 155.0 / 255.0, 0.5]
      tableColor = [100.0 / 255.0, 100.0 / 255.0, 100.0 / 255.0, 0.5]

      md = gl.MeshData(vertexes=vertexes, faces=faces)

      # shelf
      self.m1 = gl.GLMeshItem(meshdata=md, smooth=True, color=shelfColor, glOptions='translucent')
      self.m1.translate(1.25, -5.9, 0.0)
      self.m1.scale(3.20, 0.57, 1.26)
      self.w.addItem(self.m1)

      # table
      self.m16 = gl.GLMeshItem(meshdata=md, smooth=True, color=tableColor, glOptions='translucent')
      self.m16.translate(1.27, -4.5, 0.7)
      self.m16.scale(2.0, 0.8, 0.05)
      self.w.addItem(self.m16)

  def plot_locator(self, loc_id):
    if (PLOT_VIEW_3D != 1) or (PLOT_DEBUG_LOCATORS != 1):
      return

    # Draw front image
    front = gl.GLImageItem(self.img_tex_front, glOptions='translucent')
    front.translate(-self.img_tex_front.shape[0] / (2.0 * self.img_scale), -self.img_tex_front.shape[1] / (2.0 * self.img_scale), 0.005)
    front.scale(1.0 / self.img_scale, 1.0 / self.img_scale, 1.0 / self.img_scale)
    front.rotate(self.locators[loc_id]["orientation"]["z"], x=0, y=0, z=1, local=True)
    front.rotate(self.locators[loc_id]["orientation"]["y"], x=0, y=1, z=0, local=True)
    front.rotate(self.locators[loc_id]["orientation"]["x"], x=1, y=0, z=0, local=True)
    front.translate(self.img_tex_front.shape[0] / (2.0 * self.img_scale), self.img_tex_front.shape[1] / (2.0 * self.img_scale), -0.005)
    front.translate(self.locators[loc_id]["coordinate"]["x"], self.locators[loc_id]["coordinate"]["y"], self.locators[loc_id]["coordinate"]["z"])
    front.translate(-self.img_tex_front.shape[0] / (2.0), -self.img_tex_front.shape[1] / (2.0), 0.005 * self.img_scale, local=True)
    self.w.addItem(front)
    self.locators[loc_id]["front"] = front

    # Draw back image
    back = gl.GLImageItem(self.img_tex_back, glOptions='translucent')
    back.translate(-self.img_tex_back.shape[0] / (2.0 * self.img_scale), -self.img_tex_back.shape[1] / (2.0 * self.img_scale), -0.005)
    back.scale(1.0 / self.img_scale, 1.0 / self.img_scale, 1.0 / self.img_scale)
    back.rotate(self.locators[loc_id]["orientation"]["z"], x=0, y=0, z=1, local=True)
    back.rotate(self.locators[loc_id]["orientation"]["y"], x=0, y=1, z=0, local=True)
    back.rotate(self.locators[loc_id]["orientation"]["x"], x=1, y=0, z=0, local=True)
    back.translate(self.img_tex_back.shape[0] / (2.0 * self.img_scale), self.img_tex_back.shape[1] / (2.0 * self.img_scale), 0.005)
    back.translate(self.locators[loc_id]["coordinate"]["x"], self.locators[loc_id]["coordinate"]["y"], self.locators[loc_id]["coordinate"]["z"])
    back.translate(-self.img_tex_back.shape[0] / (2.0), -self.img_tex_back.shape[1] / (2.0), -0.005 * self.img_scale, local=True)
    self.w.addItem(back)
    self.locators[loc_id]["back"] = back

  def plot_line(self, tag_id, loc_id):
    if (self.tags[tag_id]["sequence_nr"] >= MAX_NUM_TAG_LINES) or (self.locators[loc_id]["sequence_nr"] >= MAX_NUM_LOCATOR_LINES):
      return

    loc_x = self.locators[loc_id]["coordinate"]["x"]
    loc_y = self.locators[loc_id]["coordinate"]["y"]
    loc_z = self.locators[loc_id]["coordinate"]["z"]

    rot_x = np.radians(self.locators[loc_id]["orientation"]["x"])
    rot_y = np.radians(self.locators[loc_id]["orientation"]["y"])
    rot_z = np.radians(self.locators[loc_id]["orientation"]["z"])

    azimuth = np.radians(self.tags[tag_id]["angle"][loc_id]["azimuth"])
    elevation = np.radians(self.tags[tag_id]["angle"][loc_id]["elevation"])
    distance = self.tags[tag_id]["angle"][loc_id]["distance"]

    # Convert spherical coordinates to cartesian coordinates
    x = distance * np.cos(azimuth) * np.cos(elevation)
    y = distance * np.sin(azimuth) * np.cos(elevation)
    z = distance * np.sin(elevation)

    # Rotate coordinates according to the orientation of the locator
    line = np.array([x,y,z])
    line = Rx(rot_x).dot(line)
    line = Ry(rot_y).dot(line)
    line = Rz(rot_z).dot(line)
    start = np.array([loc_x, loc_y, loc_z])
    end = start + line
    pos = np.array([start, end])

    if "line" not in self.tags[tag_id]["angle"][loc_id]:
      # Draw new line
      self.tags[tag_id]["angle"][loc_id]["line"] = gl.GLLinePlotItem(pos=pos, color=self.tags[tag_id]["color"])
      self.w.addItem(self.tags[tag_id]["angle"][loc_id]["line"])
    else:
      # Update existing line
      self.tags[tag_id]["angle"][loc_id]["line"].setData(pos=pos)

  def plot_marker(self, tag_id):
    pos = np.array([(self.tags[tag_id]["position"]["x"],
                     self.tags[tag_id]["position"]["y"],
                     self.tags[tag_id]["position"]["z"])])
    color = self.tags[tag_id]["color"]
    if PLOT_ACCURACY:
      if PLOT_VIEW_3D:
        # Calculate accuracy as Mean Radial Spherical Error (MRSE)
        # Confidence region probability: 61%
        size = np.sqrt(self.tags[tag_id]["position"]["x_stdev"]**2 +
                        self.tags[tag_id]["position"]["y_stdev"]**2 +
                        self.tags[tag_id]["position"]["z_stdev"]**2)
      else:
        # Calculate accuracy as Distance Root Mean Square (DRMS)
        # Confidence region probability: 65%
        size = np.sqrt(self.tags[tag_id]["position"]["x_stdev"]**2 +
                       self.tags[tag_id]["position"]["y_stdev"]**2)

    if "marker" not in self.tags[tag_id]:
      if PLOT_ACCURACY:
        pxMode = False
      else:
        pxMode = True
        size = 40
      # Draw new marker
      if PLOT_VIEW_3D:
        self.tags[tag_id]["marker"] = gl.GLScatterPlotItem(pos=pos, color=color, size=size, pxMode=pxMode)
        self.tags[tag_id]["marker"].setGLOptions('translucent')
        self.w.addItem(self.tags[tag_id]["marker"])

      else:
        # PLOT_VIEW_2D
        brushColor = QtGui.QColor(color[0] * 255, color[1] * 255, color[2] * 255)

        self.tags[tag_id]["marker"] = pg.ScatterPlotItem(pen=pg.mkPen(width=1), brush=brushColor, symbol='o', size=size, pxMode=pxMode)
        self.tags[tag_id]["marker"].setData(pos=pos)
        self.view.addItem(self.tags[tag_id]["marker"])
    else:
      # Update existing marker
      self.tags[tag_id]["marker"].setData(pos=pos)
      if PLOT_ACCURACY:
        if PLOT_VIEW_3D:
          self.tags[tag_id]["marker"].setData(size=size)
        else:
          self.tags[tag_id]["marker"].setSize(size)

    # Plot a trailing marker trace
    if PLOT_VIEW_3D:
      if self.marker_trace and self.numEstimatesPlotted % self.markerTraceStep == 0:
        trace = gl.GLScatterPlotItem(pos=pos, color=[0.6, 0.3, 0.8, 1.0], size=10)
        trace.setGLOptions('opaque')
        trace.setDepthValue(-0.5)
        self.markerTraces.insert(0, trace)
        self.w.addItem(self.markerTraces[0])

        if len(self.markerTraces) >= self.numMarkerTraces:
          item = self.markerTraces.pop()
          self.w.removeItem(item)

    self.numEstimatesPlotted += 1

  def plot_text(self, tag_id):
    pos = [self.tags[tag_id]["position"]["x"],
           self.tags[tag_id]["position"]["y"],
           self.tags[tag_id]["position"]["z"]]
    if PLOT_VIEW_3D:
      self.w.setText(tag_id, pos)

    else:
      # PLOT_VIEW_2D
      if "text" not in self.tags[tag_id]:
        # Draw new text
        self.tags[tag_id]["text"] = pg.TextItem(tag_id, color=(200, 200, 200), fill=None)
        self.tags[tag_id]["text"].setPos(pos[0], pos[1])
        self.view.addItem(self.tags[tag_id]["text"])
      else:
        # Update existing text
        self.tags[tag_id]["text"].setPos(pos[0], pos[1])

  def update(self):
    # Process position messages
    while not self.q_pos.empty():
      entry = self.q_pos.get()
      tag_id = entry.pop("tag_id")
      if tag_id not in self.tags:
        if len(self.tags) == MAX_NUM_TAGS:
          continue
        self.add_tag(tag_id)
      self.tags[tag_id]["position"].update(entry)

      self.plot_marker(tag_id)
      if PLOT_TAG_ADDRESSES:
        self.plot_text(tag_id)

    # Process angle messages
    while not self.q_ang.empty():
      entry = self.q_ang.get()
      tag_id = entry.pop("tag_id")
      loc_id = entry.pop("loc_id")
      if tag_id not in self.tags:
        if len(self.tags) == MAX_NUM_TAGS:
          continue
        self.add_tag(tag_id)
      if loc_id not in self.tags[tag_id]["angle"]:
        self.tags[tag_id]["angle"][loc_id] = {}
      self.tags[tag_id]["angle"][loc_id].update(entry)

      if self.plotlines:
        self.plot_line(tag_id, loc_id)

  def animation(self):
    timer = QtCore.QTimer()
    timer.timeout.connect(self.update)
    timer.setInterval(1)
    timer.start()
    self.start()

  def parse_config(self, conf_file):
    locators = []
    with open(conf_file, "rb") as conf:
        config = json.load(conf)
        locators = config["locators"]
        self.positioning_id = config["id"]

    for loc in locators:
      loc_id = loc.pop("id")
      # MQTT topics are case sensitive
      m = re.match(r"^ble-(?P<address_type>[A-Za-z]+)-(?P<address>[0-9A-Fa-f]+)$", loc_id)
      if m is None:
        raise Exception("Invalid locator id format: {}".format(loc_id))
      loc_id = "ble-{}-{}".format(m.group("address_type").lower(), m.group("address").upper())
      self.locators[loc_id] = loc
      self.locators[loc_id]["sequence_nr"] = len(self.locators) - 1
      self.plot_locator(loc_id)

  def add_tag(self, tag_id):
    print("Add tag {}".format(tag_id))
    self.tags[tag_id] = {}
    self.tags[tag_id]["sequence_nr"] = len(self.tags) - 1
    self.tags[tag_id]["color"] = [random.uniform(0, 1), random.uniform(0, 1), random.uniform(0, 1), 1.0]
    self.tags[tag_id]["position"] = {}
    self.tags[tag_id]["angle"] = {}

def mqtt_conn_type(arg):
    """ Argument parser for MQTT server connection parameters. """
    retval = DEFAULT_CONNECTION
    arglist = arg.split(":", 1)
    if len(arglist[0]) == 0:
        raise argparse.ArgumentTypeError("Host name is empty")
    retval["host"] = arglist[0]
    if len(arglist) > 1:
        try:
            retval["port"] = int(arglist[1])
        except ValueError as val:
            raise argparse.ArgumentTypeError("Invalid port number: " + arglist[1]) from val
    return retval

def on_connect(client, userdata, flags, rc):
    ''' Called when a CONNACK response is received from the server. '''
    print("Connected with result code " + str(rc))
    topic = "silabs/aoa/position/{}/#".format(userdata.positioning_id)
    print("Subscribe for ", topic)
    client.subscribe(topic)
    for loc_id in userdata.locators:
      topic = "silabs/aoa/angle/{}/#".format(loc_id)
      print("Subscribe for ", topic)
      client.subscribe(topic)

def on_message(client, userdata, msg):
    ''' Called when a PUBLISH message is received from the server. '''
    m = re.match(r"silabs/aoa/position/.+/(?P<tag_id>.+)", msg.topic)
    if m is not None:
      entry = json.loads(msg.payload)
      entry["tag_id"] = m.group("tag_id")
      userdata.q_pos.put(entry)
    else:
      m = re.match(r"silabs/aoa/angle/(?P<loc_id>.+)/(?P<tag_id>.+)", msg.topic)
      if m is not None:
        entry = json.loads(msg.payload)
        entry["loc_id"] = m.group("loc_id")
        entry["tag_id"] = m.group("tag_id")
        userdata.q_ang.put(entry)

def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-c", metavar="CONFIG_FILE", help="Configuration file path", default=DEFAULT_CONFIG)
    parser.add_argument("-m", metavar="HOST[:PORT]", help="MQTT broker connection parameters", default=DEFAULT_CONNECTION, type=mqtt_conn_type)
    args = parser.parse_args()

    v = Visualizer()
    v.parse_config(args.c)

    client = mqtt.Client(userdata=v)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(host=args.m["host"], port=args.m["port"])
    client.loop_start()

    if PLOT_ROOM:
      v.plot_room()
    v.animation()

if __name__ == "__main__":
  main()
