import cadquery as cq

w1 = 4
w2 = 6.4
l1 = 15
l2 = 20
h2 = 13
noCenter = (False, True, False)

res = (cq.Workplane("XY")
       .tag("origin")
       .transformed(offset=(5, 0, 0))
       .rect(25, 20)
       .extrude(-1)
       .workplaneFromTagged("origin")
       .transformed(offset=(0, 0, 0))
       .rect(l2 - l1, w2)
       .cutThruAll()
       .transformed(offset=(l2 - l1, 0, 0))
       .rect(l1, w1)
       .cutThruAll()
       .workplaneFromTagged("origin")
       .transformed(offset=(l1 - l2, w2/2+w1/2, 0))
       .box(l2, w1, h2, centered=noCenter)
       .workplaneFromTagged("origin")
       .transformed(offset=(l1 - l2, -w2/2-w1/2, 0))
       .box(l2, w1, h2, centered=noCenter)
       .workplaneFromTagged("origin")
       .transformed(offset=(l1 - l2, w2/2+0.5, h2-1),
                    rotate=(90, 90, 0))
       .circle(1).extrude(l2)
       .workplaneFromTagged("origin")
       .transformed(offset=(l1 - l2, -w2/2-0.5, h2-1),
                    rotate=(90, 90, 0))
       .circle(1).extrude(l2)
      )

show_object(res)
