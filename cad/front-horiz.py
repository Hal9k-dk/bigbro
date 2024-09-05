import cadquery as cq

from defs import *

o_width, o_height = o_height, o_width

centerXY = (True, True, False)

res = (cq.Workplane("XY")
       .tag("o")
       .box(o_width, o_height, front_th, centered=(True, True, False))
       .edges(">Z or |Z")
       .fillet(fillet_r)
      )

card_offset = 33

# card cutout
res = (res
       .workplaneFromTagged("o")
       .transformed(offset=(card_offset , (o_height - card_h)/2, 0))
       .rect(card_w, card_h)
       .cutBlind(card_th)
      )

# keyfob cutout
res = (res
       .workplaneFromTagged("o")
       .transformed(offset=(card_offset , (o_height - keyfob_th)/2, 0), rotate=(0, 0, 90))
       .slot2D(2*keyfob_d, keyfob_d)
       .cutBlind(keyfob_th)
      )

disp_x_center = -35
disp_y_offset = 0

# display bezel
disphole = (cq.Workplane("XY")
            .transformed(offset=(disp_x_offset + disp_x_center , disp_y_offset, front_th - disp_wth))
            .box(dhw+2*disp_wth, dhh+2*disp_wth, disp_wth, centerXY)
            .edges("<Z")
            .chamfer(disp_wth*0.99)
            )

# display cutout
dispcutout = (cq.Workplane("XY")
            .transformed(offset=(disp_x_center , disp_y_offset, -disp_wth))
            .box(dw + 2*dwextra, dh, front_th, centerXY)
            )

z_offset = front_th - (disp_wth + h2)
clip_h = dh-2

res = (res
       .cut(disphole)
       .cut(dispcutout)
       # display holders
       .workplaneFromTagged("o")
       .transformed(offset=(disp_x_center + w1/2 + dw/2, disp_y_offset, z_offset))
       .box(w1, clip_h , h2, centered=(True, True, False))
       .workplaneFromTagged("o")
       .transformed(offset=(disp_x_center + dw/2 + 0.1, disp_y_offset + clip_h/2, z_offset+1), rotate=(90, 0, 0))
       .circle(rod_d/2).extrude(clip_h)
       .workplaneFromTagged("o")
       .transformed(offset=(disp_x_center - w1/2 - dw/2, disp_y_offset, z_offset))
       .box(w1, clip_h, h2, centered=(True, True, False))
       .workplaneFromTagged("o")
       .transformed(offset=(disp_x_center - dw/2 - 0.1, disp_y_offset + clip_h/2, z_offset+1), rotate=(90, 0, 0))
       .circle(rod_d/2).extrude(clip_h)
      )

# screw holes
res = (res
      .workplaneFromTagged("o")
      .rect(o_width - 2*mh_inset, o_height - 2*mh_inset, forConstruction=True)
      .vertices()
      .circle(insert_r)
      .cutBlind(insert_l)
      )

show_object(res)
