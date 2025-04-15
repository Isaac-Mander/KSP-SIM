"""An example program showing usage of the laser tools library

Isaac Mander 08/04/25
"""


#Import the laser tools library function set
import laser_tools as lt


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
img = lt.setup("example_panel.svg",image_width,image_height)



#Create the basic frames for each panel
base_x = padding
base_y = padding
img.add(lt.create_rounded_rectangle((base_x,base_y), (width,height), 3, "black", 0.5, False)) #Base panel

middle_x = base_x + width + padding
middle_y = padding
img.add(lt.create_rounded_rectangle((middle_x,middle_y), (width,height), 8, "black", 0.5, True)) #Middle panel

face_x = middle_x + width + padding
face_y = padding
img.add(lt.create_rounded_rectangle((face_x,face_y), (width,height), 8, "black", 0.5, True)) #Face panel


#Create the mounting holes on the base panel
mount_hole_offset = 4
mount_hole_radius = 1.5
img.add(lt.create_circle((base_x+mount_hole_offset,base_y+mount_hole_offset),mount_hole_radius,"black",0.5))
img.add(lt.create_circle((base_x-mount_hole_offset+width,base_y+mount_hole_offset),mount_hole_radius,"black",0.5))
img.add(lt.create_circle((base_x+mount_hole_offset,base_y-mount_hole_offset+height),mount_hole_radius,"black",0.5))
img.add(lt.create_circle((base_x-mount_hole_offset+width,base_y-mount_hole_offset+height),mount_hole_radius,"black",0.5))

#These holes create a green marker to show where the holes are in relation to the other layers
layers = [middle_x,face_x]
for offset in layers:
    img.add(lt.create_circle((offset+mount_hole_offset,base_y+mount_hole_offset),mount_hole_radius,"green",0.5))
    img.add(lt.create_circle((offset-mount_hole_offset+width,base_y+mount_hole_offset),mount_hole_radius,"green",0.5))
    img.add(lt.create_circle((offset+mount_hole_offset,base_y-mount_hole_offset+height),mount_hole_radius,"green",0.5))
    img.add(lt.create_circle((offset-mount_hole_offset+width,base_y-mount_hole_offset+height),mount_hole_radius,"green",0.5))







#Create the holes and elements which pass through every layer (e.g. switches)
plate_offsets = [base_x,middle_x,face_x]

for offset in plate_offsets:
    #Top row of switches / leds
    img.add(lt.create_circle((offset+18.75,padding+10),2.5,"black",0.5))
    img.add(lt.create_circle((offset+18.75,padding+22),3.5,"black",0.5))
    img.add(lt.create_circle((offset+37.5,padding+10),2.5,"black",0.5))
    img.add(lt.create_circle((offset+37.5,padding+22),3.5,"black",0.5))
    img.add(lt.create_circle((offset+56.25,padding+16),3.5,"black",0.5))
    


    #Middle row
    for switch_offset in range(0,5,1):
        img.add(lt.create_circle((offset+11.3+(15*switch_offset),padding+49.5),3.5,"black",0.5))
    
    #Bottom Right grid of switches
    img.add(lt.create_circle((offset+88,padding+63),3.5,"black",0.5))
    for switch_offset in range(0,3):
        img.add(lt.create_circle((offset+73,padding+88+(25*switch_offset)),3.5,"black",0.5))
        img.add(lt.create_circle((offset+88,padding+88+(25*switch_offset)),3.5,"black",0.5))

    #Cuttout
    img.add(lt.create_rectangle((offset+8,padding+70.5),(48,71.5),"black",0.5))


#Define an offset for all the text labels
text_offset = 8 #mm

#Add in engraved text to the top and for each text item create a hole for a 3mm backlight led
img.add(lt.create_text((face_x+18.75,face_y+22+text_offset),"RCS","red",0.5,"3pt")) #Add text
img.add(lt.create_circle((base_x+18.75,base_y+22+text_offset),1.5,"black",0.5)) #Backlight Base
img.add(lt.create_circle((middle_x+18.75,middle_y+22+text_offset),1.5,"black",0.5)) #Backlight middle

img.add(lt.create_text((face_x+37.5,face_y+22+text_offset),"SAS","red",0.5,"3pt")) #Add text
img.add(lt.create_circle((base_x+37.5,base_y+22+text_offset),1.5,"black",0.5)) #Backlight Base
img.add(lt.create_circle((middle_x+37.5,middle_y+22+text_offset),1.5,"black",0.5)) #Backlight middle

img.add(lt.create_text((face_x+56.25,face_y+16-text_offset),"RAW","red",0.5,"3pt")) #Add text
img.add(lt.create_circle((base_x+56.25,base_y+16-text_offset),1.5,"black",0.5)) #Backlight Base
img.add(lt.create_circle((middle_x+56.25,middle_y+16-text_offset),1.5,"black",0.5)) #Backlight middle

img.add(lt.create_text((face_x+56.25,face_y+16+text_offset),"DAMP","red",0.5,"3pt")) #Add text
img.add(lt.create_circle((base_x+56.25,base_y+16+text_offset),1.5,"black",0.5)) #Backlight Base
img.add(lt.create_circle((middle_x+56.25,middle_y+16+text_offset),1.5,"black",0.5)) #Backlight middle

img.add(lt.create_text((face_x+70,face_y+13),"FLIGHT","red",0.5,"3pt")) #Add text
img.add(lt.create_circle((base_x+70,base_y+13),1.5,"black",0.5)) #Backlight Base
img.add(lt.create_circle((middle_x+70,middle_y+13),1.5,"black",0.5)) #Backlight middle

img.add(lt.create_text((face_x+70,face_y+19),"MODE","red",0.5,"3pt")) #Add text
img.add(lt.create_circle((base_x+70,base_y+19),1.5,"black",0.5)) #Backlight Base
img.add(lt.create_circle((middle_x+70,middle_y+19),1.5,"black",0.5)) #Backlight middle

#Border
img.add(lt.create_rounded_rectangle((offset+18.75-8,padding+2),(70,28+6),3,"red",0.5))



#Programatically add the middle row labels as it has a repeating pattern
plate_offsets = [base_x,middle_x]
upper_labels = ["1","EXND","3","4","5",]
lower_labels = ["6","RECT","8","9","10",]
for switch_offset in range(0,5,1):
    label_index = switch_offset
    img.add(lt.create_text((offset+11.3+(15*switch_offset),padding+49.5+text_offset),lower_labels[label_index],"red",0.5,"3pt"))
    img.add(lt.create_text((offset+11.3+(15*switch_offset),padding+49.5-text_offset),upper_labels[label_index],"red",0.5,"3pt"))

for offset in plate_offsets:
    for switch_offset in range(0,5,1):
        img.add(lt.create_circle((offset+11.3+(15*switch_offset),padding+49.5+text_offset),1.5,"black",0.5))
        img.add(lt.create_circle((offset+11.3+(15*switch_offset),padding+49.5-text_offset),1.5,"black",0.5))



#Programatically add labels to the lower grid
plate_offsets = [base_x,middle_x]
labels_right = ["Grid 1","Grid 3","Grid 5","Grid 7"]
labels_left = ["Grid 2","Grid 4","Grid 6"]

#Text labels
img.add(lt.create_text((face_x+88,face_y+63+text_offset),labels_right[0],"red",0.5,"3pt"))
for switch_offset in range(0,3):
    label_index = switch_offset
    img.add(lt.create_text((face_x+73,face_y+88+(25*switch_offset)+text_offset),labels_left[label_index],"red",0.5,"3pt"))
    img.add(lt.create_text((face_x+88,face_y+88+(25*switch_offset)+text_offset),labels_right[label_index+1],"red",0.5,"3pt"))


#Backlight holes
for offset in plate_offsets:
    img.add(lt.create_circle((offset+88,face_y+63+text_offset),1.5,"black",0.5))
    for switch_offset in range(0,3):
        img.add(lt.create_circle((offset+73,face_y+88+(25*switch_offset)+text_offset),1.5,"black",0.5))
        img.add(lt.create_circle((offset+88,face_y+88+(25*switch_offset)+text_offset),1.5,"black",0.5))




#Save the image as a file
img.save()