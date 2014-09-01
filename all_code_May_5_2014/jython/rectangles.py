"""
ZetCode Jython Swing tutorial

In this program, we lay out widgets
using absolute positioning

author: Jan Bodnar
website: www.zetcode.com
last modified: November 2010
"""

from java.awt import Color
from javax.swing import ImageIcon
from javax.swing import JFrame
from javax.swing import JPanel
from javax.swing import JLabel



class Example(JFrame):

    def __init__(self):
        super(Example, self).__init__()

        self.initUI()

    def initUI(self):

        panel = JPanel()
        panel.setLayout(None)
        panel.setBackground(Color(66, 66, 66))
        self.getContentPane().add(panel)

        rot = ImageIcon("input.png")
        rotLabel = JLabel(rot)
        rotLabel.setBounds(20, 20, rot.getIconWidth(), rot.getIconHeight())

        min = ImageIcon("cpuoutput.png")
        minLabel = JLabel(min)
        minLabel.setBounds(40, 160, min.getIconWidth(), min.getIconHeight())

        bar = ImageIcon("inputdata.png")
        barLabel = JLabel(bar)
        barLabel.setBounds(170, 50, bar.getIconWidth(), bar.getIconHeight())


        panel.add(rotLabel)
        panel.add(minLabel)
        panel.add(barLabel)


        self.setTitle("Absolute")
        self.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE)
        self.setSize(350, 300)
        self.setLocationRelativeTo(None)
        self.setVisible(True)


if __name__ == '__main__':
    Example()