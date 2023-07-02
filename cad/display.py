import cadquery as cq

height = 35
width = 50
thickness = 2
centerXY = (True, True, False)

result = (cq.Workplane("XY")
         .box(width, height, thickness, centered=centerXY)
         .tag("origin")
         )

# display hole
disp_y_offset = 0
disp_x_offset = 5
dhh = 27
dhw = 28
# display size
dh = 30
dw = 40.5
w1 = 3
h2 = 6.3
disphole = (cq.Workplane("XY")
            .transformed(offset=(disp_x_offset, disp_y_offset, 0))
            .box(dhw+2*thickness, dhh+2*thickness, thickness, centerXY)
            .edges(">Z")
            .chamfer(thickness*0.99)
            )
rod_d = 1
result = (result.cut(disphole)
       .workplaneFromTagged("origin")
       .transformed(offset=(dw/2+w1/2, 0, thickness))
       .box(w1, dh, h2, centered=(False, True, False))
       .workplaneFromTagged("origin")
       .transformed(offset=(dw/2+w1/2, dh/2, thickness+h2-1),
                    rotate=(90, 0, 0))
       .circle(rod_d/2).extrude(dh)
       .workplaneFromTagged("origin")
       .transformed(offset=(-dw/2-1.5*w1, 0, thickness))
       .box(w1, dh, h2, centered=(False, True, False))
       .workplaneFromTagged("origin")
       .transformed(offset=(-dw/2-0.5*w1, dh/2, thickness+h2-1),
                    rotate=(90, 0, 0))
       .circle(rod_d/2).extrude(dh)
)

show_object(result)

