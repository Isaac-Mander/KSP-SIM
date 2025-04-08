"""A utility which enables the quick generation of laser cutting files

By Isaac Mander 08/04/25
"""

import svgwrite
import math

"""Sets up a new doc under a common standard"""
def setup(img_filename, width, height):
    #Create a new file
    new_doc = svgwrite.Drawing(img_filename, size=(f"{width}mm", f"{height}mm"), profile="full")
    new_doc.viewbox(width=width, height=height)

    # Add white background rectangle
    background = svgwrite.shapes.Rect(insert=(0, 0), size=(width, height), fill="white")
    new_doc.add(background)

    return new_doc


"""Creates a circle"""
def create_circle(center, radius, outline_color, stroke_width=1):
    return svgwrite.shapes.Circle(center=center, r=radius,
                                  fill="none", stroke=outline_color,
                                  stroke_width=stroke_width)
"""Creates a rectangle"""
def create_rectangle(insert, size, outline_color, stroke_width=1):
    return svgwrite.shapes.Rect(insert=insert, size=size,
                                fill="none", stroke=outline_color,
                                stroke_width=stroke_width)
"""Creates a arc with an angle"""
def create_arc(center, radius, start_angle, end_angle, outline_color, stroke_width=1):
    cx, cy = center
    start_rad = math.radians(start_angle)
    end_rad = math.radians(end_angle)
    start_x = cx + radius * math.cos(start_rad)
    start_y = cy + radius * math.sin(start_rad)
    end_x = cx + radius * math.cos(end_rad)
    end_y = cy + radius * math.sin(end_rad)

    delta_angle = (end_angle - start_angle) % 360
    large_arc_flag = 1 if delta_angle > 180 else 0
    sweep_flag = 1

    path_str = (
        f"M {start_x} {start_y} "
        f"A {radius} {radius} 0 {large_arc_flag} {sweep_flag} {end_x} {end_y}"
    )
    return svgwrite.path.Path(d=path_str, fill="none", stroke=outline_color,
                              stroke_width=stroke_width)
"""Creates a block of text"""
def create_text(insert, content, outline_color, stroke_width=1, font_size="12pt", font_family="Arial",text_anchor="middle"):
    return svgwrite.text.Text(
        content,
        insert=insert,
        fill="none",
        stroke=outline_color,
        stroke_width=stroke_width,
        font_size=font_size,
        font_family=font_family,
        text_anchor=text_anchor,
        dominant_baseline="middle"
    )

def create_rounded_rectangle(insert, size, corner_radius, outline_color,
                             stroke_width=1, inward=False):
    """
    Create an SVG rectangle with rounded corners and no fill.
    
    Parameters:
      insert: tuple (x, y) - top-left position in millimeters.
      size: tuple (w, h) - overall dimensions in millimeters.
      corner_radius: radius for corner rounding in millimeters.
      outline_color: stroke color.
      stroke_width: stroke width in millimeters.
      inward: Boolean. If False, creates a normal (convex) rounded rectangle;
              if True, creates an inward (concave) rounded rectangle.
    
    Returns:
      An svgwrite element:
        - A Convex rounded rectangle using <rect> if inward is False.
        - A concave (inward) rounded rectangle as a <path> if inward is True.
    """
    x, y = insert
    w, h = size
    r = corner_radius

    if not inward:
        # Standard (convex) rounded rectangle: use the <rect> element with rx and ry.
        return svgwrite.shapes.Rect(
            insert=(f"{x}", f"{y}"),
            size=(f"{w}", f"{h}"),
            fill="none",
            stroke=outline_color,
            stroke_width=f"{stroke_width}",
            rx=f"{r}",
            ry=f"{r}"
        )
    else:
        # Inward (concave) rounded rectangle:
        # Our goal is to start with a rectangle whose outer bounds are (x,y) to (x+w,y+h),
        # then carve an inward arc at each corner.
        # For each corner, we want to draw an arc from one edge to the adjacent edge.
        # Using the SVG arc command, we specify:
        #   - rx and ry = r
        #   - x-axis-rotation = 0
        #   - large-arc-flag = 1 (for the major, concave, arc)
        #   - sweep-flag = 0.
        #
        # For example, at the top-right corner, the arc goes from (x+w-r, y) to (x+w, y+r).
        # (Similarly for the other corners.)
        #
        # Note: The d string must be unitless (plain numbers), even though our overall coordinates are in mm.
        d = (
            f"M {x + r} {y} "                    # Start at top edge (x+r, y)
            f"L {x + w - r} {y} "                # Draw top edge to (x+w-r, y)
            f"A {r} {r} 1 0 0 {x + w} {y + r} "   # Top-right arc from (x+w-r, y) to (x+w, y+r) with center at (x+w, y)
            f"L {x + w} {y + h - r} "             # Draw right edge to (x+w, y+h-r)
            f"A {r} {r} 1 0 0 {x + w - r} {y + h} " # Bottom-right arc from (x+w, y+h-r) to (x+w-r, y+h) with center at (x+w, y+h)
            f"L {x + r} {y + h} "                 # Draw bottom edge to (x+r, y+h)
            f"A {r} {r} 1 0 0 {x} {y + h - r} "   # Bottom-left arc from (x+r, y+h) to (x, y+h-r) with center at (x, y+h)
            f"L {x} {y + r} "                     # Draw left edge to (x, y+r)
            f"A {r} {r} 1 0 0 {x + r} {y} Z"      # Top-left arc from (x, y+r) to (x+r, y) with center at (x, y)
        )
        return svgwrite.path.Path(d=d, fill="none", stroke=outline_color, stroke_width=f"{stroke_width}")




# Example usage:
if __name__ == "__main__":

    dwg = setup("test.svg",500,500)


    # Convex (outward) rounded rectangle
    convex_rect = create_rounded_rectangle(
        insert=(20, 20),
        size=(170, 100),
        corner_radius=5,
        outline_color="blue",
        stroke_width=2,
        inward=False
    )
    dwg.add(convex_rect)

    # Concave (inward) rounded rectangle
    concave_rect = create_rounded_rectangle(
        insert=(5, 5),
        size=(170, 100),
        corner_radius=5,
        outline_color="red",
        stroke_width=2,
        inward=True
    )
    dwg.add(concave_rect)

    dwg.save()
