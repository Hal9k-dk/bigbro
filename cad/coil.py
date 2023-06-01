import cadquery as cq

dia = 4
h = 4 #!!
hcc = 30 #!!
vcc = 25

top = (cq.Workplane("XY")
       .transformed(offset=(0, 0, h))
       .rarray(hcc, vcc, 2, 2)
       .box(dia, dia, 2, centered=(True, True, False))
       .edges(">Z or <Z or |Z")
       .fillet(0.5)
      )

res = (cq.Workplane("XY")
       .rect(hcc+10, vcc+10)
       .extrude(-2)
       .rarray(hcc, vcc, 2, 2)
       .circle(dia/2)
       .extrude(h)
       .union(top)
      )

show_object(res)
