import cadquery as cq

from defs import *

centerXY = (True, True, False)

th = 5

hole_d = 3.5 # upper
hole_d = 2   # lower

res = (cq.Workplane("XY")
       .box(23, th, 10, centered=centerXY)
       .faces("<Y")
       .workplane()
       .transformed(offset=(0, -2, 0))
       .circle(gland_r)
       .cutThruAll()
       .faces(">Z")
       .workplane()
       .transformed(offset=(0, th/2, 0))
       .rarray(18, 1, 2, 1)
       .circle(hole_d/2)
       .cutThruAll()
       )

show_object(res)
