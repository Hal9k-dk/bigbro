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

# cable hole
res = (res
       .workplaneFromTagged("o")
       .transformed(offset=(0, - 53, 0))
       .slot2D(40, 17).cutThruAll()
      )

# magnets
res = (res
       .workplaneFromTagged("o")
       .rarray(50, 80, 2, 2)
       .circle(10/2).cutBlind(2.2)
       )

# PCB standoffs
standoff_h = 5
standoff_d = 10

standoff = round_standoff(standoff_d, standoff_h)
pcb_x = -o_width/2 - 56 + 5
pcb_y = -o_height/2 - 66 + 23
for c in pcb_holes:
    s = res
    .workplaneFromTagged("o")
    .transformed(offset=(pcb_x + c[0], pcb_y + c[1],
                         sh_th+standoff_h/2))
    .eachpoint(lambda loc: standoff.val().moved(loc), True)))
    
    res = res.union(s)

cut_h = bottom_th - 10
res = res.workplaneFromTagged("o").workplane(cut_h).split(keepTop=True)

show_object(res)
