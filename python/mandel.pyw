#!/usr/bin/env python
# -*- coding: latin-1; -*-

import sys
from PyQt4 import QtCore, QtGui

class RenderThread(QtCore.QThread):
    def __init__(self, parent=None, width=100, height=100):
        QtCore.QThread.__init__(self, parent)
        self.wx = width
        self.wy = height
        self.inc = 1
        self.zoom = 300
        self.ox = -0.5
        self.oy = 0.5
        self.xc = 0.0
        self.yc = 0.0
        self.nbc = 50
        self.pixmap = QtGui.QPixmap(width, height)
        self.mutex = QtCore.QMutex()
        #self.cond = QtCore.QWaitCondition()
        self.abort = False

    def run(self):
        #print "starting thread on %dx%d" % (self.wx, self.wy) 
        self.mutex.lock()  
        painter = QtGui.QPainter(self.pixmap)
        painter.fillRect(self.pixmap.rect(), QtCore.Qt.black)
        self.mutex.unlock()  
               
        for xe in range(0, self.wx, self.inc):
            for ye in range(0, self.wy, self.inc):
                #print "xe, ye=" , xe, ye
                x,y = self.transfo(xe,ye)
                n=0; xn=0.0; yn=0.0
                while (n!=self.nbc) and (yn*yn+xn*xn<4):
                    n+=1
                    xn, yn = xn*xn-yn*yn+x, 2*xn*yn+y 
                if n==self.nbc:
                    painter.setPen(QtGui.QColor(0, 0, 0))
                else:
	                painter.setPen(QtGui.QColor(255.0-255.0/(n+1), 0, 0)) 
                self.mutex.lock()
                painter.drawPoint(xe,ye)
                self.mutex.unlock()
                if self.abort:
                    return
        #print "thread is over!"
    def transfo(self,xe,ye):
        x = self.ox + float(xe-self.wx/2)/self.zoom
        y = self.oy - float(ye-self.wy/2)/self.zoom
        return x,y         

class MandelWindow(QtGui.QWidget):
    """
    Main Qt Widget
    """
    def __init__(self, parent = None):
        QtGui.QWidget.__init__(self, parent)
        self.setWindowTitle("Mandelbrot")
        self.resize(320, 200)
        self.thread = RenderThread()
        self.timer = QtCore.QTimer(self)
        self.connect(self.timer, QtCore.SIGNAL("timeout()"), self.timerfct)
        self.timer.start(10)
        self.mouse = QtCore.QPoint()
        self.setMouseTracking(True) # appelle mouseMoveEvent meme si pas de clic
        
    def timerfct(self):
        self.update()
        #print "timer()"
        
    def resizeEvent(self, event):
        #print "resize!"
        while self.thread.isRunning():
            self.thread.abort = True
            #self.thread.mutex.lock()
            #self.thread.cond.wait(self.thread.mutex)
            
        self.thread = RenderThread(width=self.width(), height=self.height())
        self.thread.start()

    def paintEvent(self, event):
        #print "paint!"
        self.thread.mutex.lock()
        painter = QtGui.QPainter(self)
        painter.drawPixmap(0, 0, self.thread.pixmap)
        self.thread.mutex.unlock()

        # coords
        text = "X = %f, Y = %f" % (self.mouse.x(), self.mouse.y())
        metrics = painter.fontMetrics()
        textWidth = metrics.width(text)

        painter.setPen(QtCore.Qt.NoPen)
        painter.setBrush(QtGui.QColor(0, 0, 0, 127))
        painter.drawRect((self.width() - textWidth) / 2 - 5, 0, textWidth + 10,
                         metrics.lineSpacing() + 5)
        painter.setPen(QtCore.Qt.white)
        painter.drawText((self.width() - textWidth) / 2,
                         metrics.leading() + metrics.ascent(), text)

    def mouseMoveEvent(self, event):
        #print "move!"
        pos = event.pos()
        x,y = self.thread.transfo(pos.x(), pos.y())
        self.mouse = QtCore.QPointF(x,y)
        self.update()

def main():
    app = QtGui.QApplication(sys.argv)
    win = MandelWindow()
    win.show()
    app.connect(app, QtCore.SIGNAL("lastWindowClosed()"),app,QtCore.SLOT("quit()"))
    sys.exit(app.exec_())
 
if __name__=="__main__":
    main() 
             