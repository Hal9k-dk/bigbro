import cadquery as cq

from defs import *

centerXY = (True, True, False)

horizontal = True

hoox = -5
hooy = -5

# Screwpost for corners of a box
def square_screwpost_body(d, h, r):
    return (cq.Workplane()
            .box(d, d, h, centered=(True, True, False))
            .edges("|Z").fillet(r)
            )

# PCB standoff
def round_standoff(d, h):
    max_d = min(h, 3*insert_l)
    return (cq.Workplane()
            .cylinder(radius=d/2, height=h)
            .faces(">Z")
            .circle(insert_r).cutBlind(-insert_l)
            .faces(">Z")
            .circle(insert_sr+.25).cutBlind(-max_d)
            )

# basic shape
res = (cq.Workplane("XY")
       .tag("o")
       .box(o_width, o_height, bottom_th, centered=(True, True, False))
       .faces(">Z")
       .shell(-sh_th)
       .edges("<Z or |Z")
       .fillet(fillet_r)
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

# PCB standoffs
standoff_h = 5
standoff_d = 10

standoff = round_standoff(standoff_d, standoff_h)
pcb_x = -o_width/2 - 56 + 5
pcb_y = -o_height/2 - 66 + 23
standoff1 = (res
             .workplaneFromTagged("o")
             .transformed(offset=(pcb_x + 89, pcb_y + 74, sh_th+standoff_h/2))
             .eachpoint(lambda loc: standoff.val().moved(loc), True))

standoff2 = (res
             .workplaneFromTagged("o")
             .transformed(offset=(pcb_x + 63, pcb_y + 99, sh_th+standoff_h/2))
             .eachpoint(lambda loc: standoff.val().moved(loc), True))

standoff3 = (res
             .workplaneFromTagged("o")
             .transformed(offset=(pcb_x + 63, pcb_y + 161, sh_th+standoff_h/2))
             .eachpoint(lambda loc: standoff.val().moved(loc), True))

standoff4 = (res
             .workplaneFromTagged("o")
             .transformed(offset=(pcb_x + 104, pcb_y + 161, sh_th+standoff_h/2))
             .eachpoint(lambda loc: standoff.val().moved(loc), True))

res = res.union(standoff1).union(standoff2).union(standoff3).union(standoff4)

# screw terminal hole 1
res = (res
       .workplaneFromTagged("o")
       .transformed(offset=(17-1, -40, 0))
       .rect(18, 10).cutThruAll()
      )
# screw terminal hole 1
res = (res
       .workplaneFromTagged("o")
       .transformed(offset=(-7.5, -40, 0))
       .rect(13, 10).cutThruAll()
      )

show_object(res)
