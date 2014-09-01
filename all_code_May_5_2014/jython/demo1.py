import random

from java.lang import System
from javax.swing import (BoxLayout, ImageIcon, JButton, JFrame, JPanel,
        JPasswordField, JLabel, JTextArea, JTextField, JScrollPane,
        SwingConstants, WindowConstants)
from java.awt import Component, GridLayout, BorderLayout
from javax.imageio import ImageIO 
from java import io
from java.lang import Runnable



import test_smooth
from test_smooth import *


class Demo(JFrame):

    def __init__(self):
        super(Demo, self).__init__()

        self.initUI()

    def initUI(self):

        panel = JPanel(size=(50,50))
        

        panel.setLayout(BorderLayout( ))
        panel.setToolTipText("A Panel container")

        joclButton = JButton("JOCL", actionPerformed=self.onJOCL)
        joclButton.setBounds(100, 500, 100, 30)
        joclButton.setToolTipText("JOCL Button")
        panel.add(joclButton)

        qButton = JButton("Quit", actionPerformed=self.onQuit)
        qButton.setBounds(200, 500, 80, 30)
        qButton.setToolTipText("Quit Button")
        panel.add(qButton)
		
		
		
        newImage = ImageIO.read(io.File("input.png"))
        resizedImage =  newImage.getScaledInstance(500, 500,10)
        newIcon = ImageIcon(resizedImage)
        label1 = JLabel("Image and Text",newIcon, JLabel.	CENTER)

        label1.setVerticalTextPosition(JLabel.BOTTOM)
        label1.setHorizontalTextPosition(JLabel.CENTER)
        label1.setSize(10,10)
        panel.add(label1)
        
        self.getContentPane().add(panel)
        self.setTitle("GPU Demo")
        self.setSize(1200, 600)
        self.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE)
        self.setLocationRelativeTo(None)
        self.setVisible(True)
	
    def onQuit(self, e): System.exit(0)
    def onJOCL(self, e):
		test_smooth()
	
if __name__ == '__main__':
    Demo()