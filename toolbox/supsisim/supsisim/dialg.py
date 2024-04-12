from supsisim.qtvers import *

from supsisim.const import path

class IO_Dialog(QDialog):
    def __init__(self,parent=None):
        super(IO_Dialog, self).__init__(parent)
        layout = QGridLayout()
        self.setWindowModality(Qt.WindowModality.ApplicationModal)
        self.resize(380, 180)
        self.spbInput = QSpinBox()
        self.spbOutput = QSpinBox()
        self.spbInput.setValue(1)
        self.spbOutput.setValue(1)

        label2 = QLabel('Number of inputs:')
        label3 = QLabel('Number of outputs')
        self.pbOK = QPushButton('OK')
        self.pbCANCEL = QPushButton('CANCEL')
        layout.addWidget(self.spbInput,0,1)
        layout.addWidget(self.spbOutput,1,1)
        layout.addWidget(label2,0,0)
        layout.addWidget(label3,1,0)
        layout.addWidget(self.pbOK,2,0)
        layout.addWidget(self.pbCANCEL,2,1)
        self.setLayout(layout)
        self.pbOK.clicked.connect(self.accept)
        self.pbCANCEL.clicked.connect(self.reject)

class BlockName_Dialog(QDialog):
    def __init__(self,parent=None):
        super(BlockName_Dialog, self).__init__(parent)
        layout = QGridLayout()
        self.setWindowModality(Qt.WindowModality.ApplicationModal)
        self.resize(380, 100)
        self.name = QLineEdit()

        label1 = QLabel('Block ID:')
        self.pbOK = QPushButton('OK')
        self.pbCANCEL = QPushButton('CANCEL')
        layout.addWidget(label1,0,0)
        layout.addWidget(self.name,0,1)
        layout.addWidget(self.pbOK,2,0)
        layout.addWidget(self.pbCANCEL,2,1)
        self.setLayout(layout)
        self.pbOK.clicked.connect(self.accept)
        self.pbCANCEL.clicked.connect(self.reject)

class RTgenDlg(QDialog):
    def __init__(self, parent=None):
        super(RTgenDlg, self).__init__(None)
        self.setWindowModality(Qt.WindowModality.ApplicationModal)
        self.resize(600, 100)

        lab1 = QLabel('Template Makefile')
        self.template = QLineEdit('')
        btn_template = QPushButton('BROWSE...')
        lab2 = QLabel('Parameter script')
        self.parscript = QLineEdit('')
        btn_script = QPushButton('BROWSE...')
        lab3 = QLabel('Additional Objs')
        self.addObjs = QLineEdit('')
        btn_addObjs = QPushButton('BROWSE...')
        lab4 = QLabel('Sampling Time')
        self.Ts = QLineEdit('')
        lab5 = QLabel('Final Time')
        self.Tf = QLineEdit('')
        lab6 = QLabel('Priority')
        self.prio = QLineEdit('')

        pbOK = QPushButton('OK')
        pbCANCEL = QPushButton('CANCEL')
        grid = QGridLayout()

        grid.addWidget(lab1, 0, 0)
        grid.addWidget(self.template, 0, 1)
        grid.addWidget(btn_template, 0, 2)
        grid.addWidget(lab2, 1, 0)
        grid.addWidget(self.parscript, 1, 1)
        grid.addWidget(btn_script, 1, 2)
        grid.addWidget(lab3, 2, 0)
        grid.addWidget(self.addObjs, 2, 1)
        grid.addWidget(btn_addObjs, 2, 2)
        grid.addWidget(lab6, 3, 0)
        grid.addWidget(self.prio, 3, 1)
        grid.addWidget(lab4, 4, 0)
        grid.addWidget(self.Ts, 4, 1)
        grid.addWidget(lab5, 5, 0)
        grid.addWidget(self.Tf, 5, 1)
        grid.addWidget(pbOK, 6, 0)
        grid.addWidget(pbCANCEL, 6, 1)
        pbOK.clicked.connect(self.accept)
        pbCANCEL.clicked.connect(self.reject)
        btn_template.clicked.connect(self.getTemplate)
        btn_addObjs.clicked.connect(self.getObjs)
        btn_script.clicked.connect(self.getScript)
        self.setLayout(grid)

    def getTemplate(self):
        fname = QFileDialog.getOpenFileName(self,'Open Template Makefile',
                                                  path+'CodeGen/templates', 'Template (*.tmf)')
        fname = fname[0]
        if len(fname) != 0:
            ln = fname.split('/')
            templ = ln[-1].__str__()
            self.template.setText(templ)

    def getObjs(self):
        fname = QFileDialog.getOpenFileName(self,'Additional libraries',
                                                  '.','Dynamic libraries (*.so)')
        fname = fname[0]
        if len(fname) != 0:
            ln = fname.split('/')
            libname = ln[-1].__str__()
            self.addObjs.setText(libname)

    def getScript(self):
        fname = QFileDialog.getOpenFileName(self,'Open Python script',
                                                  '.', 'Python file (*.py)')
        fname = fname[0]
        if len(fname) != 0:
            self.parscript.setText(fname)

class WDDlg(QDialog):
    def __init__(self, parent=None):
        super(WDDlg, self).__init__(None)
        self.setWindowModality(Qt.WindowModality.ApplicationModal)
        self.resize(300, 100)

        lab1 = QLabel('Math errors')
        self.CheckMath = QCheckBox('')
        self.CheckMath.setChecked(False)

        pbOK = QPushButton('OK')
        pbCANCEL = QPushButton('CANCEL')
        grid = QGridLayout()

        grid.addWidget(lab1, 0, 0)
        grid.addWidget(self.CheckMath, 0, 1)
        grid.addWidget(pbOK, 1, 0)
        grid.addWidget(pbCANCEL, 1, 1)

        pbOK.clicked.connect(self.accept)
        pbCANCEL.clicked.connect(self.reject)

        self.setLayout(grid)

class SHVDlg(QDialog):
    def __init__(self, parent=None):
        super(SHVDlg, self).__init__(None)
        self.setWindowModality(Qt.WindowModality.ApplicationModal)
        self.resize(600, 100)

        lab1 = QLabel('Enable SHV protocol')
        self.SHVused = QCheckBox('')
        self.SHVused.setChecked(False)
        lab2 = QLabel('SHV Broker IP')
        self.SHVip = QLineEdit('')
        lab3 = QLabel('SHV Broker Port')
        self.SHVport = QLineEdit('')
        lab4 = QLabel('SHV Broker User')
        self.SHVuser = QLineEdit('')
        lab5 = QLabel('SHV Broker Password')
        self.SHVpassw = QLineEdit('')
        lab6 = QLabel('SHV Device ID')
        self.SHVdevid = QLineEdit('')
        lab7 = QLabel('SHV Device Mount Point')
        self.SHVmount = QLineEdit('')
        lab8 = QLabel('SHV Tree Type')
        self.SHVtree = QComboBox()
        self.SHVtree.addItems(['GAVL', 'GSA', 'GSA_STATIC'])

        pbOK = QPushButton('OK')
        pbCANCEL = QPushButton('CANCEL')
        grid = QGridLayout()

        grid.addWidget(lab1, 0, 0)
        grid.addWidget(self.SHVused, 0, 1)
        grid.addWidget(lab2, 1, 0)
        grid.addWidget(self.SHVip, 1, 1)
        grid.addWidget(lab3, 2, 0)
        grid.addWidget(self.SHVport, 2, 1)
        grid.addWidget(lab4, 3, 0)
        grid.addWidget(self.SHVuser, 3, 1)
        grid.addWidget(lab5, 4, 0)
        grid.addWidget(self.SHVpassw, 4, 1)
        grid.addWidget(lab6, 5, 0)
        grid.addWidget(self.SHVdevid, 5, 1)
        grid.addWidget(lab7, 6, 0)
        grid.addWidget(self.SHVmount, 6, 1)
        grid.addWidget(lab8, 7, 0)
        grid.addWidget(self.SHVtree, 7, 1)
        grid.addWidget(pbOK, 8, 0)
        grid.addWidget(pbCANCEL, 8, 1)
        pbOK.clicked.connect(self.accept)
        pbCANCEL.clicked.connect(self.reject)

        self.setLayout(grid)
