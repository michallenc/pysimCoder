from supsisim.qtvers import *

from supsisim.port import Port, InPort, OutPort
from supsisim.connection import Connection
from supsisim.const import GRID, PW, LW, BWmin, BHmin, BHstep, PD, respath

import os

class ResizeHandle(QGraphicsRectItem):   
    def __init__(self, parent, position):
        SIZE = 4
        super().__init__(-SIZE/2, -SIZE/2, 2*SIZE, 2*SIZE, parent)
        self.setPen(QPen(QBrush(Qt.GlobalColor.blue),1.0))
        self.setFlag(QGraphicsItem.GraphicsItemFlag.ItemIsMovable)
        self.position = position
        self.block = self.parentItem()
        
    def mousePressEvent(self, event):
        if self.block.isSelected():  
            self.startPos = event.scenePos()
            self.block.old_center = self.block.boundingRect().center()
            
    def mouseMoveEvent(self, event):
        if self.block.isSelected():
            delta = event.scenePos() - self.startPos  
            self.startPos = event.scenePos()
            if self.block.flip:
                f_delta = -1
            else:
                f_delta = 1
            
            if self.position == "bottom_right":
                self.block.dims[0] = max(self.block.minW, self.block.dims[0] + f_delta*delta.x())
                self.block.dims[1] = max(self.block.minH, self.block.dims[1] + delta.y())
                
                self.block.updateShape(False)
                        
    def mouseReleaseEvent(self, event):
        if self.block.isSelected():
            self.block.updateShape(True)
        self.block.updateShape(True)
            
class Block(QGraphicsPathItem):
    """A block holds ports that can be connected to."""
    def __init__(self, *args):
        self.scene = args[1]
        parent = args[0]
        super(Block, self).__init__(parent)
        self.scene.addItem(self)
        self.syspath = ''
        self.ident = -1

        self.roundedBlocks = True
        
        if len(args) == 12:
            parent, self.scene, self.name, self.inp, self.outp, self.insetble, self.outsetble, self.icon, self.params, self.helpTxt, self.dims, self.flip = args
        elif len(args) == 3:
            parent, self.scene, strBlk = args
            ln = strBlk.split('@')
            self.name = str(ln[0])
            self.inp = int(ln[1])
            self.outp = int(ln[2])
            self.icon = ln[5]
            self.params = ln[6]
            self.helpTxt = ln[7]
            self.dims = int(ln[8])
            self.flip = False
            stbin = int(ln[3])
            stbout = int(ln[4])
            self.insetble = (stbin==1)
            self.outsetble = (stbout==1)
        else:
            raise ValueError('Needs 12 or 3 arguments; received %i.' % len(args))

        self.line_color = Qt.GlobalColor.black
        self.fill_color = Qt.GlobalColor.black
        
        tl = type(self.dims) is list
        if not tl:
            w = self.dims
            self.dims = [w, BHmin]
                   
        self.handles = []
        
        self.setup()
                
        try:
            self.scene.blocks.add(self)
        except:
            pass
            
    def __str__(self):
        txt  = 'Name         :' + self.name.__str__() + '\n'
        txt += 'Sizes        :' + self.dims.__str__() + '\n'
        txt += 'pos          :' + '[' + self.pos().x().__str__() + ', ' + self.pos().y().__str__() + ']\n'
        txt += 'Input ports  :' + self.inp.__str__()  + '\n'
        txt += 'Output ports :' + self.outp.__str__() + '\n'
        txt += 'Icon         :' + self.icon.__str__() + '\n'
        txt += 'Params       :' + self.params.__str__() + '\n'
        return txt
        
    def setup(self):
        Nports = max(self.inp, self.outp)
        
        self.minW = 60
        self.minH = BHstep * (Nports+1)
        
        h = BHmin+PD*(max(Nports-1,0))
     
        self.w = self.dims[0]
        self.h = max(BHmin+PD*(max(Nports-1,0)), self.dims[1])
        
        self.dims = [self.w, self.h]
        
        p = QPainterPath()
        self.setLabel(p)
        self.setPath(p)

        for n in range(0,self.inp):
            self.add_inPort(n)
        for n in range(0,self.outp):
            self.add_outPort(n)

        self.old_center = self.boundingRect().center()
        
        self.setFlag(QGraphicsItem.GraphicsItemFlag.ItemIsMovable)
        self.setFlag(QGraphicsItem.GraphicsItemFlag.ItemIsSelectable)
        
        self.addHandles()
        self.setFlip()

    def getPPos(self, nP):
        d = self.dims[1]/(nP+1)
        dExt = (d // GRID) * GRID
        if nP == 1:
            dSpc = 0
        else:
            dSpc = (self.dims[1]-2*dExt)/(nP-1)
        return dExt, dSpc
        
    def add_inPort(self, n):
        L = self.inp
        dExt, dSpc = self.getPPos(L)
        pos = -self.h/2  + dExt + dSpc*n
        port = InPort(self, self.scene)
        port.block = self
        xpos = -(self.w)/2
        port.setPos(xpos, pos)
        return port

    def add_outPort(self, n):
        L = self.outp
        dExt, dSpc = self.getPPos(L)
        pos = -self.h/2 + dExt + dSpc*n
        port = OutPort(self, self.scene)
        port.block = self
        xpos = (self.w)/2
        port.setPos(xpos, pos)
        return port

    def ports(self):
        ports = []
        for thing in self.childItems():
            if isinstance(thing, Port):
                ports.append(thing)
        return ports

    def paint(self, painter, option, widget):
        pen = QPen()
        pen.setBrush(self.line_color)
        pen.setWidth = LW
        if self.isSelected():
            pen.setStyle(Qt.PenStyle.DotLine)
            self.showHandles()
        else:
            self.hideHandles()
        
        painter.setPen(pen)

        if self.roundedBlocks:
            painter.drawRoundedRect(self.boundingRect(), 10, 10)
        else:
            painter.drawPath(self.path())
            
        svg_size = self.renderer.defaultSize()
        
        r = self.boundingRect()

        delta = r.center()    
        new_left: float = -svg_size.width()/2 + delta.x()
        new_top: float = -svg_size.height()/2 + delta.y()
        where_to: QRectF = QRectF(new_left, new_top, svg_size.width(), svg_size.height())
        self.renderer.render(painter, where_to)

    def itemChange(self, change, value):
        return value

    def remove(self):
        self.scene.blocks.remove(self)
        for thing in self.childItems():
            try:
                thing.remove()
            except:
                pass
        self.scene.removeItem(self)

    def setPos(self, *args):
        if len(args) == 1:
            pt = self.gridPos(args[0])
            super(Block, self).setPos(pt)
        else:
            pt = QPointF(args[0],args[1])
            pt = self.gridPos(pt)
            super(Block, self).setPos(pt)
        
    def clone(self, pt):
        b = Block(None, self.scene, self.name, self.inp, self.outp, 
                      self.insetble, self.outsetble, self.icon, self.params, self.helpTxt, self.dims, self.flip)
        b.setPos(self.scenePos().__add__(pt))

    def setFlip(self, flip=None):
        if flip: 
            self.flip = flip
        
        self.updateHandles()
        
        str_path = respath + 'blocks/Icons/' + self.icon + '.svg'
        if self.flip:
            self.setTransform(QTransform.fromScale(-1, 1))
            # the path
            mirr_path = '/tmp/' + self.icon + '.svg'
            if not os.path.exists(mirr_path):
                cmd = 'inkscape --actions="select-all;object-flip-horizontal" -o ' + mirr_path + ' ' + str_path
                os.system(cmd)
            self.renderer = QtSvg.QSvgRenderer(mirr_path)
        else:
            self.setTransform(QTransform.fromScale(1, 1))
            self.renderer = QtSvg.QSvgRenderer(str_path)
        self.flipLabel()

    def setLabel(self, p):
        labels = set()
        try:
            for b in self.scene.blocks:
                if b != self:
                    labels.add(b.label.toPlainText())
            name = self.name
            if name in labels:
                cnt = 0
                while name and name[-1] in '0123456789':
                    name = name[:-1]
                base = name
                while name in labels:
                    name = base + str(cnt)
                    cnt += 1
            self.name = name
            self.label = QGraphicsTextItem(self)
            self.label.setPlainText(name)
            
        except:
            self.label = QGraphicsTextItem(self)
            self.label.setPlainText(self.name)           
        
        p.addRect(-self.w/2, -self.h/2, self.w, self.h)
        w = self.label.boundingRect().width()
        self.label.setPos(-w/2, self.h/2+5)
 
    def flipLabel(self):
        w = self.label.boundingRect().width()
        if self.flip:
            self.label.setTransform(QTransform.fromScale(-1,1).translate(-w,0))
        else:
            self.label.setTransform(QTransform.fromScale(1,1))          
    
    def save(self):
        pos = (self.pos().x(), self.pos().y())
        vals = [self.name, self.inp, self.outp, self.insetble, self.outsetble, 
                self.icon, self.params, self.helpTxt, self.dims, self.flip, pos]
        keys = ['name', 'inp', 'outp', 'inset', 'outset', 'icon', 'params', 'help', 'dims', 'flip', 'pos']
        return dict(zip(keys, vals))

    def getPorts(self):
        InP = []
        OutP = []
        for item in self.childItems():
            if isinstance(item, InPort):
                InP.append(item)
            elif isinstance(item, OutPort):
                OutP.append(item)
            else:
                pass

        InP.sort(key=lambda p: p.pos().y())
        OutP.sort(key=lambda p: p.pos().y())
        return InP, OutP

    def cloneBlkWithPorts(self):
        # create a perfect copy of a block
        b = Block(None, self.scene, self.name, self.inp, self.outp,
                      self.insetble, self.outsetble, self.icon, self.params,
                      self.helpTxt, self.dims, self.flip)
        b.name = self.name

        inp1, outp1 = self.getPorts()
        inp2, outp2 = b.getPorts()

        for n in range(0, len(inp1)):
            for c in inp1[n].connections:
                newConn = Connection(None, self.scene)
                newConn.port1 = c.port1
                newConn.pos1 = c.pos1
                newConn.port2 = inp2[n]
                newConn.pos2 = c.pos2
                for pt in c.connPoints:
                    newConn.connPoints.append(pt)
                inp2[n].connections.append(newConn)
                if c in c.port1.connections:
                    c.port1.connections.remove(c)
                    c.port1.connections.append(newConn)                    
                self.scene.removeItem(newConn)

        for n in range(0, len(outp1)):
            for c in outp1[n].connections:
                newConn = Connection(None, self.scene)
                newConn.port1 = outp2[n]
                newConn.pos1 = c.pos2
                newConn.port2 = c.port2
                newConn.pos2 = c.pos2
                outp2[n].connections.append(newConn)
                if c in c.port2.connections:
                    c.port2.connections.remove(c)
                    c.port2.connections.append(newConn)                    
                self.scene.removeItem(newConn)

        b.scene.removeItem(b)
        return b

    def gridPos(self, pt):
        gr = GRID
        x = gr * ((pt.x() + gr /2) // gr)
        y = gr * ((pt.y() + gr /2) // gr)
        return QPointF(x,y)

    def grid_2_Pos(self, pt):
        gr = 2*GRID
        x = gr * ((pt.x() + gr /2) // gr)
        y = gr * ((pt.y() + gr /2) // gr)
        return QPointF(x,y)

    def setSysPath(self,basepath):
        self.syspath = f"{basepath}/{self.name}"

    def getCodeName(self):
        return self.name + '_' + str(self.ident)

    def addHandles(self):
        rect = self.boundingRect()
        
        self.handles.append(ResizeHandle(self, "top_left"))
        self.handles[-1].setPos(rect.topLeft())
        self.handles[-1].setCursor(Qt.CursorShape.ForbiddenCursor)
        
        self.handles.append(ResizeHandle(self, "top_right"))
        self.handles[-1].setPos(rect.topRight())
        self.handles[-1].setCursor(Qt.CursorShape.ForbiddenCursor)
        
        self.handles.append(ResizeHandle(self, "bottom_left"))
        self.handles[-1].setPos(rect.bottomLeft())
        self.handles[-1].setCursor(Qt.CursorShape.ForbiddenCursor)
        
        self.handles.append(ResizeHandle(self, "bottom_right"))
        self.handles[-1].setPos(rect.bottomRight())
        self.handles[-1].setCursor(Qt.CursorShape.SizeFDiagCursor)
                    
    def hideHandles(self):
        for handle in self.handles:
            handle.hide()
            
    def showHandles(self):
        for handle in self.handles:
            handle.show()
                            
    def updateShape(self, flag):
        self.prepareGeometryChange()  
        p = QPainterPath()
        p1 = QPointF(-self.w/2, -self.h/2)
        if flag:
            self.dims[0] = 2*GRID*((self.dims[0] + GRID) // (2*GRID))
            self.dims[1] = 2*GRID*((self.dims[1] + GRID) // (2*GRID))
            self.w = self.dims[0]
            self.h = self.dims[1]
        p2 = p1 + QPointF(self.dims[0], self.dims[1])
        p.addRect(QRectF(p1, p2))
        self.setPath(p)
        self.updateCenterPos()
        self.updateHandles()
        self.updatePorts(p1, p2)
        self.updateLabel(p1,p2)
        
    def updateCenterPos(self):
        self.prepareGeometryChange()        
        new_center = self.boundingRect().center()
        delta = self.grid_2_Pos(new_center - self.old_center)
        self.setPos(self.pos() - delta)
        self.sceneTransform().map(QPointF(0, 0))
        self.old_center = new_center        
        
    def updatePorts(self, p1, p2): 
        inP, outP = self.getPorts()
        # Input ports
        L = len(inP)
        dExt, dSpc = self.getPPos(L)
        
        px = p1.x()
        for n in range(0,L):
            p = inP[n]
            py = p1.y() + dExt + dSpc*n
            p.setPos(px, py)

        # Output ports
        L = len(outP)
        d = self.dims[1]/(L+1)
        dSpc = (d // GRID) * GRID
        dExt = (self.dims[1] - dSpc*(L-1)) // 2
        px = p2.x()
        for n in range(0,L):
            p = outP[n]
            py = p1.y() + dExt + dSpc*n
            p.setPos(px, py)
        try:        
            self.scene.mainw.editor.redrawSelectedItems()
            self.scene.mainw.editor.redrawNodes()
        except:
            pass
        
    def updateLabel(self, p1, p2):
        w = self.label.boundingRect().width()
        cp = (p2.x()-p1.x())/2
        self.label.setPos(p1.x()+cp-w/2, p2.y()+5)
                    
    def updateHandles(self):
        try:
            rect = self.boundingRect()
            self.handles[0].setPos(rect.topLeft())
            self.handles[1].setPos(rect.topRight())
            self.handles[2].setPos(rect.bottomLeft())
            self.handles[3].setPos(rect.bottomRight())
            if self.flip:
                self.handles[3].setCursor(Qt.CursorShape.SizeBDiagCursor)
            else:
                self.handles[3].setCursor(Qt.CursorShape.SizeFDiagCursor)
        except:
           pass
