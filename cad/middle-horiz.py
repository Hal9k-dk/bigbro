import cadquery as cq

from defs import *

o_width, o_height = o_height, o_width

card_offset = 33
mid_th = 2
mid_th_extra = 8
switch_y_offset = -card_offset
switch_x_offset = 25 - 11
disp_x_center = 35
disp_y_offset = 0

centerXY = (True, True, False)
noCenter = (False, True, False)

res = (cq.Workplane("XY")
       .tag("o")
       .box(o_width, o_height, mid_th + mid_th_extra, centered=(True, True, False))
       .edges("|Z")
       .fillet(fillet_r)
      )

# switch
res = (res
       # pocket
       .faces(">Z").workplane()#.workplaneFromTagged("o")
       .transformed(offset=(switch_y_offset, switch_x_offset, 0))
       .rect(5*sw_w1, 2*sw_l2)
       .cutBlind(-mid_th_extra)
       # long hole
       .workplaneFromTagged("o")
       .transformed(offset=(switch_y_offset, switch_x_offset, 0))
       .rect(sw_w1, sw_l2)
       .cutThruAll()
       # short hole
       .workplaneFromTagged("o")
       .transformed(offset=(switch_y_offset, switch_x_offset+sw_l1/2, 0))
       .rect(sw_w2, sw_l2 - sw_l1)
       .cutBlind(mid_th)
       # right block
       .workplaneFromTagged("o")
       .transformed(offset=(switch_y_offset - sw_w1*1.5, switch_x_offset, mid_th))
       .box(sw_w1, sw_l2, sw_h2, centered=noCenter)
       # left block
       .workplaneFromTagged("o")
       .transformed(offset=(switch_y_offset + sw_w1/2, switch_x_offset, mid_th))
       .box(sw_w1, sw_l2, sw_h2, centered=noCenter)
       # right ridge
       .workplaneFromTagged("o")
       .transformed(offset=(switch_y_offset - sw_w1/2, switch_x_offset + sw_l2/2, mid_th+sw_h2-1),
                    rotate=(90, 0, 0))
       .circle(1).extrude(sw_l2)
       # left ridge
       .workplaneFromTagged("o")
       .transformed(offset=(switch_y_offset + sw_w1/2, switch_x_offset + sw_l2/2, mid_th+sw_h2-1),
                    rotate=(90, 0, 0))
       .circle(1).extrude(sw_l2)
)

# display cutout
dispcutout = (cq.Workplane("XY")
            .transformed(offset=(disp_x_center, disp_y_offset, 0))
            .box(dw + 2*dwextra, dh, mid_th + mid_th_extra, centerXY)
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
      .circle(screw_hole_r + 0.5)
      .cutThruAll()
      )


show_object(res)
