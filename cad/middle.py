import cadquery as cq

from defs import *

centerXY = (True, True, False)
noCenter = (False, True, False)

res = (cq.Workplane("XY")
       .tag("o")
       .box(o_width, o_height, mid_th, centered=(True, True, False))
       .edges("|Z")
       .fillet(fillet_r)
      )

# switch
switch_y_offset = 47
res = (res
       # long hole
       .workplaneFromTagged("o")
       .transformed(offset=(0, switch_y_offset, 0))
       .rect(sw_w1, sw_l2)
       .cutThruAll()
       # right block
       .workplaneFromTagged("o")
       .transformed(offset=(sw_w2/2, switch_y_offset, mid_th))
       .box(sw_w1, sw_l2, sw_h2, centered=noCenter)
       .workplaneFromTagged("o")
       # left block
       .transformed(offset=(-sw_w1-sw_w2/2, switch_y_offset, mid_th))
       .box(sw_w1, sw_l2, sw_h2, centered=noCenter)
       .workplaneFromTagged("o")
       # right ridge
       .transformed(offset=(sw_w2/2+0.5, switch_y_offset+sw_l2/2, mid_th+sw_h2-1),
                    rotate=(90, 0, 0))
       .circle(1).extrude(sw_l2)
       .workplaneFromTagged("o")
       .transformed(offset=(-sw_w2/2-0.5, switch_y_offset+sw_l2/2, mid_th+sw_h2-1),
                    rotate=(90, 0, 0))
       .circle(1).extrude(sw_l2)
       .workplaneFromTagged("o")
       .transformed(offset=(0, switch_y_offset+sw_l1/2+0, 0))
       # short hole
       .rect(sw_w2, sw_l2 - sw_l1)
       .cutBlind(mid_th)
)

disp_x_center = disp_x_offset

# display cutout
dispcutout = (cq.Workplane("XY")
            .transformed(offset=(disp_x_center , disp_y_offset, 0))
            .box(dw + 2*dwextra, dh, front_th, centerXY)
            )

res = (res
       .cut(dispcutout)
       .workplaneFromTagged("o")
      )

res = (res
       # screw holes
      .workplaneFromTagged("o")
      .rect(o_width - 2*mh_inset, o_height - 2*mh_inset, forConstruction=True)
      .vertices()
      .circle(screw_hole_r)
      .cutThruAll()
      )


show_object(res)
#show_object(screwposts)
