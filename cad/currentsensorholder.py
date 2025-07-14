# %%
from build123d import *
from ocp_vscode import *

H = 18
th = 2
w = 6
cc = 24

pts = [
    (0, 0),
    (10, 0),
    (10, H - th),
    (34, H - th),
    (34, H),
    (1, H),
    (1, H - th),
    (6, H - th),
    (6, th),
    (0, th),
]

with BuildPart() as o:
    with BuildSketch(Plane.YZ) as o_sk:
        with BuildLine() as o_ln:
            Polyline(pts, close=True)
        make_face()
    extrude(amount=w)
    with BuildSketch(o.faces().sort_by(Axis.Z)[-1]) as h:
        with Locations([ (-cc/2, 0, 0) ]):
            Rectangle(w, w)
        with Locations([ (cc/2, 0, 0) ]):
            Rectangle(w, w)
    extrude(amount=2)
    fillet(o.edges().filter_by(Axis.Z), radius=.5)
    fillet(o.edges().sort_by(Axis.Z)[-1], radius=.5)
    with BuildSketch(o.faces().sort_by(Axis.Z)[-1]) as h:
        with Locations([ (-cc, 0, 0) ]):
            Circle(radius=2.5/2)
        with Locations([ (0, 0, 0) ]):
            Circle(radius=2.5/2)
    extrude(amount=-5, mode=Mode.SUBTRACT)
    with BuildSketch(o.faces().sort_by(Axis.Z)[0]) as h:
        with Locations([ (-4, 0, 0) ]):
            SlotOverall(8, 3.2)
    extrude(amount=-5, mode=Mode.SUBTRACT)

show(o)    
export_step(o.part, "currentsensorholder.step")
