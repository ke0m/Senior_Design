import random

from java.lang import System
from javax.swing import (BoxLayout, ImageIcon, JButton, JFrame, JPanel,
        JPasswordField, JLabel, JTextArea, JTextField, JScrollPane,
        SwingConstants, WindowConstants)
from java.awt import Component, GridLayout, BorderLayout


from java.lang import Runnable



class Demo(JFrame):

    def __init__(self):
        super(Demo, self).__init__()

        self.initUI()

    def initUI(self):

        panel = JPanel(size=(50,50))
        

        panel.setLayout(BorderLayout( ))
        panel.setToolTipText("A Panel container")

        joclButton = JButton("JOCL")
        joclButton.setBounds(100, 500, 100, 30)
        joclButton.setToolTipText("JOCL Button")
        panel.add(joclButton)

        qButton = JButton("Quit", actionPerformed=self.onQuit)
        qButton.setBounds(200, 500, 80, 30)
        qButton.setToolTipText("Quit Button")
        panel.add(qButton)
		
        inputImage = ImageIcon("input.png")
        #JLabel imageLabel = inputImage


        label1 = JLabel(inputImage)
        label1.setBounds(1, 1, inputImage.getIconWidth(), inputImage.getIconHeight())

        #label1.setVerticalTextPosition(JLabel.BOTTOM)
        #label1.setHorizontalTextPosition(JLabel.CENTER)
        #label1.setSize(1,1)
        panel.add(label1)

        self.getContentPane().add(panel)
        self.setTitle("GPU Demo")
        self.setSize(1200, 600)
        self.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE)
        self.setLocationRelativeTo(None)
        self.setVisible(True)

    def onQuit(self, e): System.exit(0)

if __name__ == '__main__':
    Demo()