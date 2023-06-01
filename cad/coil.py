import cadquery as cq

corner_dia = 2
rod_dia = 4
knob_expand = 1.1
h = 3.5
hcc = 41
vcc = 29

top = (cq.Workplane("XY")
       .transformed(offset=(0, 0, h))
       .rarray(hcc, vcc, 2, 2)
       .box(rod_dia*knob_expand, rod_dia*knob_expand, 2, centered=(True, True, False))
       .edges(">Z or <Z or |Z")
       .fillet(0.5)
      )

res = (cq.Workplane("XY")
       .rect(hcc+10, vcc+10)
       .extrude(-2)
       .rarray(hcc, vcc, 2, 2)
       .rect(rod_dia, rod_dia)
       .extrude(h)
       .edges("|Z")
       .fillet(corner_dia/2)
       .union(top)
      )

show_object(res)
