""" jy_ImageViewer2.py
view an image with Jython using the Java swing GUI toolkit
and other Java library items
get the free Jython installer
jython_installer-2.7a2.jar
from
http://www.jython.org/downloads.html
to run this code on Windows you can use
Jython.bat jy_ImageViewer2.py
tested with jython2.7.2 by vegaseat 07jan2013
"""

from pawt import swing
from java import io
from javax import imageio
def view_image(image):
      # create a frame/window
      frame = swing.JFrame("Jython Image Viewer")
      #allows frame corner x to exit properly
      frame.defaultCloseOperation = swing.JFrame.EXIT_ON_CLOSE
      frame.visible = True
      #change to fit image size
      frame.setSize(1500, 1000)
      frame.getContentPane().add(swing.JLabel(swing.ImageIcon(image)))
      frame.show()

	  
# get an image file you have available ...
image_filename = "input.png"
# load the image from the file
image = imageio.ImageIO.read(io.File(image_filename))
reimage = getScaledInstance(image.getIconWidth(), image.getIconHeight(), null)
view_image(reimage)

