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
def create_text(insert, content, outline_color, stroke_width=1, font_size="3pt",
                font_family="Arial", text_anchor="middle", rotation=0):
    text_attrs = {
        "insert": insert,
        "fill": "none",
        "stroke": outline_color,
        "stroke_width": stroke_width,
        "font_size": font_size,
        "font_family": font_family,
        "text_anchor": text_anchor,
        "dominant_baseline": "middle",
    }

    if rotation != 0:
        text_attrs["transform"] = f"rotate({rotation},{insert[0]},{insert[1]})"

    return svgwrite.text.Text(content, **text_attrs)


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

from PIL import ImageFont
def get_text_width(text, font_path="arial.ttf", font_size=12):
    font = ImageFont.truetype(font_path, font_size)
    bbox = font.getbbox(text)
    return bbox[2] - bbox[0]  # width


def rotory_labels_middle(centre_pos,hole_radius,label_array,label_angles,label_size,padding,line_length,stroke_width,):
    item_array = []
    item_array.append(create_circle(centre_pos,hole_radius,"black",0.5))
    label_count = len(label_array)

    index = 0
    for label in label_array:
        deg_angle = label_angles[index]
        angle = deg_angle * math.pi/180

        #Calcualte the text direction
        if angle == (0.5 * math.pi) or angle == (1.5 * math.pi):
            text_direction = "middle"
        elif angle > (0.5 * math.pi) and angle < (1.5 * math.pi): #Left
            text_direction = "end" #Swap start point of text
        else: #Right
            text_direction = "start"
            line_direction = 1
        
        #Calculate the starting point of the text obj
        text_pos = (
                    centre_pos[0] + padding*math.cos(angle),
                    centre_pos[1] + padding*math.sin(angle)
                    )
        line_pos = (
                    centre_pos[0] + line_length*math.cos(angle),
                    centre_pos[1] + line_length*math.sin(angle)
                    )
        item_array.append(create_text(text_pos,label,"red",stroke_width,label_size,"Arial",text_direction))

        visual_line = (
        f"M {centre_pos[0] + hole_radius*math.cos(angle)} {centre_pos[1] + hole_radius*math.sin(angle)}" #Start at the edge of the circle
        f"L {line_pos[0]} {line_pos[1]}" # Draw a line to the start of the text
        )
        item_array.append(svgwrite.path.Path(d=visual_line, fill="none", stroke="red", stroke_width=f"{stroke_width}"))
        index += 1
    return item_array




def rotory_labels_low(centre_pos,hole_radius,label_array,label_angles,label_size,padding,stroke_width,):
    item_array = []
    item_array.append(create_circle(centre_pos,hole_radius,"black",0.5))
    label_count = len(label_array)

    index = 0
    for label in label_array:
        angle = label_angles[index] * math.pi/180

        #Calcualte the text direction
        if angle > (0.5 * math.pi) and angle < (1.5 * math.pi): #Left
            text_direction = "end" #Swap start point of text
            line_offset = (1,2) #Start pos of the under text line
            line_direction = -1 #Swap the direction of the line underneath the text
        else: #Right
            text_direction = "start"
            line_offset = (-1,2) #Start pos of the under text line
            line_direction = 1
        
        #Calculate the starting point of the text obj
        text_pos = (
                    centre_pos[0] + padding*math.cos(angle),
                    centre_pos[1] + padding*math.sin(angle)
                    )
        item_array.append(create_text(text_pos,label,"red",stroke_width,label_size,"Arial",text_direction))

        # If a custom line length isnt specified calculate it
        line_length = get_text_width(label,"arial-font/arial.ttf",3) * line_direction

        visual_line = (
        f"M {centre_pos[0] + hole_radius*math.cos(angle)} {centre_pos[1] + hole_radius*math.sin(angle)}" #Start at the edge of the circle
        f"L {text_pos[0]+line_offset[0]} {text_pos[1]+line_offset[1]}" # Draw a line to the start of the text
        f"L {text_pos[0]+line_length} {text_pos[1]+line_offset[1]}" # Draw a line accross the bottom of the text
        )
        item_array.append(svgwrite.path.Path(d=visual_line, fill="none", stroke="red", stroke_width=f"{stroke_width}"))
        index += 1
    return item_array


def create_border_with_text(insert, content, border_size, stroke_width=1, font_size="3pt", font_family="Arial"):
    item_array = []
    item_array.append(create_text((insert[0],insert[1]-border_size[1]/2),content,"red",stroke_width,font_size,font_family))
    title_length = get_text_width(content,"arial-font/arial.ttf",3)
    line = (
    f"M {insert[0]+title_length} {insert[1] - border_size[1]/2}"
    f"L {insert[0] + (border_size[0])/2} {insert[1] - border_size[1]/2}"
    f"L {insert[0] + (border_size[0])/2} {insert[1] + border_size[1]/2}"
    f"L {insert[0] - (border_size[0])/2} {insert[1] + border_size[1]/2}"
    f"L {insert[0] - (border_size[0])/2} {insert[1] - border_size[1]/2}"
    f"L {insert[0]-title_length} {insert[1] - border_size[1]/2}"
    )
    item_array.append(svgwrite.path.Path(d=line, fill="none", stroke="red", stroke_width=f"{stroke_width}"))
    return item_array

def create_border(centre,border_size, stroke_width=0.5):
    line = (
    f"M {centre[0]} {centre[1] - border_size[1]/2}"
    f"L {centre[0] + (border_size[0])/2} {centre[1] - border_size[1]/2}"
    f"L {centre[0] + (border_size[0])/2} {centre[1] + border_size[1]/2}"
    f"L {centre[0] - (border_size[0])/2} {centre[1] + border_size[1]/2}"
    f"L {centre[0] - (border_size[0])/2} {centre[1] - border_size[1]/2}"
    f"L {centre[0]} {centre[1] - border_size[1]/2}"
    )
    return svgwrite.path.Path(d=line, fill="none", stroke="red", stroke_width=f"{stroke_width}")




def add_switch(file,pos,hole_radius,layer_offset_list,text_offset=10,top_text=None,bottom_text=None,left_text=None,right_text=None):
    #Create the hole in each layer
    for offset in layer_offset_list:
        file.add(create_circle(pos,hole_radius,"black",0.5))
    
    #Add the text and add backlight on the non face layers
    if top_text != None:
        text_pos = (pos[0],pos[1]-text_offset)
        create_text(text_pos,top_text,"red",0.5,"3pt")
        
        create_circle((text_pos[0],text_pos[1]),1.5,"black",0.5)

    if bottom_text != None:
        text_pos = (pos[0],pos[1]+text_offset)
        create_text(text_pos,bottom_text,"red",0.5,"3pt")
    if right_text != None:
        text_pos = (pos[0]-text_offset,pos[1])
        create_text(text_pos,right_text,"red",0.5,"3pt")
    if left_text != None:
        text_pos = (pos[0]+text_offset,pos[1])
        create_text(text_pos,left_text,"red",0.5,"3pt")


    

def add_backlight_text(file,center,text):
    pass




# Example usage
if __name__ == "__main__":
    #Panel Size (mm)
    width = 100
    height = 150

    #Padding between each panel in the image
    padding = 5

    #Number of layers in each panel
    layers = 3

    #Calculate the total image size
    image_width = (width * layers) + (layers * 2) + (padding * layers)
    image_height = (height) + (padding * 2)
    img = setup("test.svg",image_width,image_height)



    #Create the basic frames for each panel
    base_x = padding
    base_y = padding
    img.add(create_rounded_rectangle((base_x,base_y), (width,height), 3, "black", 0.5, False)) #Base panel

    middle_x = base_x + width + padding
    middle_y = padding
    img.add(create_rounded_rectangle((middle_x,middle_y), (width,height), 8, "black", 0.5, True)) #Middle panel

    face_x = middle_x + width + padding
    face_y = padding
    img.add(create_rounded_rectangle((face_x,face_y), (width,height), 8, "black", 0.5, True)) #Face panel

    layers = [base_x,middle_x,face_x]


    add_switch(img,(50,50),4,layers,top_text="TOP")


    img.save()



