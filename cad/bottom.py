import cadquery as cq

from defs import *

centerXY = (True, True, False)

# Screwpost for corners of a box
def square_screwpost_body(d, h, r):
    return (cq.Workplane()
            .box(d, d, h, centered=(True, True, False))
            .edges("|Z").fillet(r)
            )

res = (cq.Workplane("XY")
       .tag("o")
       .box(o_width, o_height, bottom_th, centered=(True, True, False))
       .faces(">Z")
       .shell(-sh_th)
       .edges("<Z or |Z")
       .fillet(fillet_r)
      )

#show_object(res)

res = (res
       .workplaneFromTagged("o")
       .transformed(offset=(0, 0, gland_z),
                    rotate=(90, 0, 0))
       .rarray(gland_cc, 1, 2, 1)
       .circle(gland_r).cutBlind(o_height)
      )

# screw posts
screwpost_d = 2.5*fillet_r
screwpost = square_screwpost_body(screwpost_d, bottom_th-sh_th, fillet_r)
screwposts = (res
      .workplaneFromTagged("o")
      .workplane(sh_th)
      .rect(o_width - 2.5*mh_inset, o_height - 2.5*mh_inset, forConstruction=True)
      .vertices()
      .eachpoint(lambda loc: screwpost.val().moved(loc), True)
     )

res = res.union(screwposts)

# screw holes
res = (res
      .workplaneFromTagged("o")
      .rect(o_width - 2*mh_inset, o_height - 2*mh_inset, forConstruction=True)
      .vertices()
      .circle(screw_head_r)
      .cutBlind(screw_head_h)
      .rect(o_width - 2*mh_inset, o_height - 2*mh_inset, forConstruction=True)
      .vertices()
      .circle(screw_hole_r)
      .cutThruAll()
      )

# holes for wall fitting
res = (res
       #.workplaneFromTagged("bottom")
          .transformed(offset=(0, o_height/2 - 15, 0))
          .rarray(wh_dist, 1, 2, 1)
          .circle(3.5/2).cutThruAll()
          )

show_object(res)
