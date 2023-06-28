import cadquery as cq

from defs import *

res = (cq.Workplane("XY")
       .tag("o")
       .box(o_width, o_height, front_th, centered=(True, True, False))
       .edges(">Z or |Z")
       .fillet(3)
      )

res = (res
       .workplaneFromTagged("o")
       .transformed(offset=(0, (o_height - card_h)/2, 0))
       .rect(card_w, card_h)
       .cutBlind(card_th)
      )

#show_object(card)

show_object(res)
