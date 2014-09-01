import random

from java.awt import Color
from java.awt import Font
from java.awt import Toolkit
from java.awt.event import ActionListener
from java.awt.event import KeyEvent
from java.awt.event import KeyListener
from javax.swing import ImageIcon
from javax.swing import JPanel
from javax.swing import Timer

WIDTH = 300
HEIGHT = 300
DOT_SIZE = 10
ALL_DOTS = WIDTH * HEIGHT / (DOT_SIZE * DOT_SIZE)
RAND_POS = 29
DELAY = 140

x = [0] * ALL_DOTS
y = [0] * ALL_DOTS


class Board(JPanel, KeyListener, ActionListener):
    def __init__(self):
        super(Board, self).__init__()

        self.initUI()


    def initUI(self):
        
        self.setBackground(Color.black)

        iid = ImageIcon("dot.png")
        self.ball = iid.getImage()

        iia = ImageIcon("apple.png")
        self.apple = iia.getImage()

        iih = ImageIcon("head.png")
        self.head = iih.getImage()

        self.setFocusable(True)
        self.addKeyListener(self)
        self.initGame()


    def initGame(self):

        self.left = False
        self.right = True
        self.up = False
        self.down = False
        self.inGame = True
        self.dots = 3

        for i in range(self.dots):
            x[i] = 50 - i * 10
            y[i] = 50


        self.locateApple()

        self.timer = Timer(DELAY, self)
        self.timer.start()
    


    def paint(self, g):

        # due to bug, cannot call super()
        JPanel.paint(self, g)

        if self.inGame:
            self.drawObjects(g)

        else:
            self.gameOver(g)

    def drawObjects(self, g):
        
        g.drawImage(self.apple, self.apple_x, self.apple_y, self)

        for z in range(self.dots):
            if (z == 0):
                g.drawImage(self.head, x[z], y[z], self)
            else:
                g.drawImage(self.ball, x[z], y[z], self)

        Toolkit.getDefaultToolkit().sync()
        g.dispose()

    def gameOver(self, g):

        msg = "Game Over"
        small = Font("Helvetica", Font.BOLD, 14)
        metr = self.getFontMetrics(small)

        g.setColor(Color.white)
        g.setFont(small)
        g.drawString(msg, (WIDTH - metr.stringWidth(msg)) / 2,
                     HEIGHT / 2)


    def checkApple(self):

        if x[0] == self.apple_x and y[0] == self.apple_y:
            self.dots = self.dots + 1
            self.locateApple()


    def move(self):

        z = self.dots

        while z > 0:
            x[z] = x[(z - 1)]
            y[z] = y[(z - 1)]
            z = z - 1

        if self.left:
            x[0] -= DOT_SIZE

        if self.right:
            x[0] += DOT_SIZE

        if self.up:
            y[0] -= DOT_SIZE

        if self.down:
            y[0] += DOT_SIZE


    def checkCollision(self):

        z = self.dots

        while z > 0:
            if z > 4 and x[0] == x[z] and y[0] == y[z]:
                self.inGame = False
            z = z - 1

        if y[0] > HEIGHT - DOT_SIZE:
            self.inGame = False

        if y[0] < 0:
            self.inGame = False

        if x[0] > WIDTH - DOT_SIZE:
            self.inGame = False

        if x[0] < 0:
            self.inGame = False

    def locateApple(self):

        r = random.randint(0, RAND_POS)
        self.apple_x = r * DOT_SIZE
        r = random.randint(0, RAND_POS)
        self.apple_y = r * DOT_SIZE

#    public void actionPerformed(ActionEvent e) {

    def actionPerformed(self, e):

        if self.inGame:
            self.checkApple()
            self.checkCollision()
            self.move()
        else:
            self.timer.stop()
        
        self.repaint()
    

    def keyPressed(self, e):

        key = e.getKeyCode()

        if key == KeyEvent.VK_LEFT and not self.right:
            self.left = True
            self.up = False
            self.down = False


        if key == KeyEvent.VK_RIGHT and not self.left:
            self.right = True
            self.up = False
            self.down = False

        if key == KeyEvent.VK_UP and not self.down:
            self.up = True
            self.right = False
            self.left = False

        if key == KeyEvent.VK_DOWN and not self.up:
            self.down = True
            self.right = False
            self.left = False
