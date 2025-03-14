import cadquery as cq
from defs import *

h = 120
w = wh_dist + wh_height
# thickness of screw-on part
th1 = 3
# thickness of trapezoidal part
th2 = 5
# minimum width of trapezoidal part
min_w = 30
slope = 0.7

# extrude part with slot
result = (cq.Workplane("XY")
          .hLine(-w/2) # bottom
          .vLine(th1+th2)  # edge
          .hLine(w/2 - min_w/2) # flat upper part
          .lineTo(-(min_w/2 + slope*th2), th1)
          .lineTo(0, th1)
          .mirrorY()
          .extrude(wh_height)
          )

# add part without slot
result = (result
          .faces(">Z")
          .workplane()
          .transformed(offset=(0, 0, 0))
          .box(w, 2*th1+th2, h, centered=(True, False, False))
          .edges("|Z")
          .fillet(0.5)
)

# drill mounting holes
result = (result
          .faces(">Z")
          .transformed(offset=(0, 2*th1+th2, wh_height), rotate=(90, 180, 0))
          .rarray(wh_dist, 1, 2, 1)
          .cskHole(4.5, 9, 82)
          )


show_object(result)
