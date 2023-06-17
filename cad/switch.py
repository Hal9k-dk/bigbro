import cadquery as cq

w1 = 4.5
w2 = 6.4
l1 = 15
l2 = 20
h2 = 13-1.25
noCenter = (False, True, False)

res = (cq.Workplane("XY")
       .tag("origin")
       .rect(25, 20)
       .extrude(-2)
       .workplaneFromTagged("origin")
       .transformed(offset=(l1/2, 0, 0))
       .rect(l2 - l1, w2)
       .cutThruAll()
       .workplaneFromTagged("origin")
       .rect(l2, w1)
       .cutThruAll()
       .workplaneFromTagged("origin")
       .transformed(offset=(-l2/2, w2/2+w1/2, 0))
       .box(l2, w1, h2, centered=noCenter)
       .workplaneFromTagged("origin")
       .transformed(offset=(-l2/2, -w2/2-w1/2, 0))
       .box(l2, w1, h2, centered=noCenter)
       .workplaneFromTagged("origin")
       .transformed(offset=(-l2/2, w2/2+0.5, h2-1),
                    rotate=(90, 90, 0))
       .circle(1).extrude(l2)
       .workplaneFromTagged("origin")
       .transformed(offset=(-l2/2, -w2/2-0.5, h2-1),
                    rotate=(90, 90, 0))
       .circle(1).extrude(l2)
      )

show_object(res)
