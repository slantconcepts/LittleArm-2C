#This program is a reworking of the original V2 program into a python applicaiton and the addition of a serial port selection function

from Tkinter import *
import tkFileDialog
import tkMessageBox
import time
import serial
import serial.tools.list_ports
import copy

class Application (Frame, object):
    
    def __init__(self, master):
        super(Application, self).__init__(master)
        
        #serial values
        self.portOptions = []
        self.serialStatus = 0
        self.ports = 0 
        self.portOptions = []
        self.notificationShowed = 0
        self.badSerialCount = 0
        self.guiStarted = 0
        
        self.currentSequence = "motion_recording.txt"	#the file name of recording prei-intialized to the default
        self.currentDirectoy = "/"						#defines the working directory of the user

        self.loopStartStop = False
        
        self.pack()
        self.createMenu()
        self.createWidgets()
        root.after(1000, self.looper())
        
        self.backgroundColor = "#66ff33"

        
    def createMenu(self):
        #++++++++++++++++++Menu+++++++++++++++++++++++++
        print "create Menu"
        self.menubar = Menu(self)

        self.filemenu = Menu(self.menubar, tearoff = 0)

        self.filemenu = Menu(self.menubar, tearoff=0)
        self.filemenu.add_command(label="Open", command=self.openFile)
        self.filemenu.add_command(label= "New Sequence", command=self.newFile)

        self.filemenu.add_command(label="Save Sequence As", command=self.saveFileAs)
        self.filemenu.add_separator()
        self.filemenu.add_command(label="Exit", command=self.quit)
        self.menubar.add_cascade(label="File", menu=self.filemenu)

        # display the menu
        root.config(menu=self.menubar)
        
    def createWidgets(self):
        print "create Widgets"
        #+++++++++++++++++++++++USB Selection frame++++++++++++++++++++++++++++++++++
        self.portSelectionFrame = Frame(self)
        self.portSelectionFrame.grid(row= 0, column = 1)
        self.usbLabel = Label(self.portSelectionFrame, text = "Current USB Connection")
        self.usbLabel.grid(row=0,column = 0)
        self.serialSelect()
        
        #+++++++++++++++++++++++spacer frame++++++++++++++++++++++++++++++++++
        self.spacerFrameLeft = Frame(self)
        self.spacerFrameLeft.grid(row = 0, column = 0 )

        self.spacerLabel6 = Label(self.spacerFrameLeft, padx = 10)
        self.spacerLabel6.grid(row = 0, column = 0 )

        #+++++++++++++++++ARM+++++++++++++++++++++++++

        # The scroll bars
        self.armControl = Frame(self)
        self.armControl.grid(row = 1, column = 1 )

        #armLabel = Label(armControl, text = "Arm Components", font = ("ARIAL", 24),relief = GROOVE, padx = 100)
        #armLabel.pack()

        self.spacerLabel = Label(self.armControl, padx = 100)
        self.spacerLabel.grid(row = 1, column = 1 )

        #++++++++++++++++++++++++BASE+++++++++++++++++++++++++++

        self.baseLabel = Label(self.armControl, text = "Base", font = ("ARIAL", 16), relief = GROOVE, padx = 100, width = 9)
        self.baseLabel.grid(row = 2, column = 1 )

        self.base = Scale(self.armControl, from_= 5, to = 175, length = 306, orient = HORIZONTAL, showvalue = 0, command = self.move_it, bg = "blue")
        self.base.set(108)
        self.base.grid(row = 3, column = 1 )

        #++++++++++++++++++++++++Shoulder+++++++++++++++++++++++++

        self.shoulderLabel = Label(self.armControl, text = "Shoulder", font = ("ARIAL", 16),relief = GROOVE, padx = 100, width = 9)
        self.shoulderLabel.grid(row = 4, column = 1 )

        self.shoulder = Scale(self.armControl, from_= 5, to = 175, length = 306, orient = HORIZONTAL, showvalue = 0,command = self.move_it, bg = "blue")
        self.shoulder.set(90)
        self.shoulder.grid(row = 5, column = 1 )

        #++++++++++++++++++++++ELBOW++++++++++++++++++++++++++++

        self.elbowLabel = Label(self.armControl, text = "Elbow",font = ("ARIAL", 16), relief = GROOVE, padx = 100, width = 9)
        self.elbowLabel.grid(row = 6, column = 1 )

        self.elbow = Scale(self.armControl, from_= 5, to = 175, length = 306, orient = HORIZONTAL, showvalue = 0, command = self.move_it,bg = "blue")
        self.elbow.set(100)
        self.elbow.grid(row = 7, column = 1 )

        #++++++++++++++++++++++++++++Gripper+++++++++++++++++++

        self.gripperLabel = Label(self.armControl, text = "Gripper",font = ("ARIAL", 16), relief = GROOVE, padx = 100, width = 9)
        self.gripperLabel.grid(row = 8, column = 1 )

        self.gripper = Scale(self.armControl, from_= 5, to = 75, length = 306, orient = HORIZONTAL,  showvalue = 0,  command = self.move_it, bg = "orange")
        self.gripper.grid(row = 9, column = 1 )

        #++++++++++++++++++++++++++Speed++++++++++++++++++++++++

        self.spacerLabel2 = Label(self.armControl,  padx = 100)
        self.spacerLabel2.grid(row = 10, column = 1 )

        self.speedLabel = Label(self.armControl,  font = ("Arial", 16), text = "Speed", relief = GROOVE, padx = 100, width = 9)
        self.speedLabel.grid(row = 11, column = 1 )

        self.theSpeed = Scale(self.armControl, from_= 3, to = 20, length = 306, orient = HORIZONTAL, command = self.move_it)
        self.theSpeed.grid(row = 12, column = 1 )


        self.spacerLabel3 = Label(self.armControl, padx = 100)
        self.spacerLabel3.grid(row = 13, column = 1 )

        self.pauseButton = Button(self.armControl, font = ("ARIAL", 16), text= "Pause for 1 Sec", width = 20, command = self.recordPause, bd = 5)
        self.pauseButton.grid(row = 14, column = 1 )

        self.homeButton = Button(self.armControl, font = ("ARIAL", 16), text= "Go Home", width = 20, command = self.goHome, bd = 5)
        self.homeButton.grid(row = 15, column = 1 )

        self.spacerLabel8 = Label(self.armControl, padx = 100)
        self.spacerLabel8.grid(row = 16, column = 1 )

        #+++++++++++++++++++++++space frame++++++++++++++++++++++++++++++++++

        self.spacerFrame = Frame(self)
        self.spacerFrame.grid(row = 0, column = 2 )

        self.spacerLabel6 = Label(self.spacerFrame, padx = 20)
        self.spacerLabel6.grid(row = 0, column = 0 )

        #+++++++++++++++++++++++RECORD++++++++++++++++++++++++++++
        self.recordButtons = Frame(self)
        self.recordButtons.grid(row = 1, column = 3 )

        self.spacerLabel4 = Label(self.recordButtons, padx = 100)
        self.spacerLabel4.grid(row = 1, column = 2 )

        self.recordButton = Button(self.recordButtons, font = ("ARIAL", 16),text = "Record Position", width = 20, command = self.recordArmPos, bd = 5, bg = "#ff6600", highlightcolor = "green")
        self.recordButton.grid(row = 2, column = 2 )

        self.spacerLabel9 = Label(self.recordButtons,   padx = 100)
        self.spacerLabel9.grid(row = 3, column = 2 )

        self.playButton = Button(self.recordButtons, font = ("ARIAL", 16), text = "Play Sequence", width = 20, command = self.playback,bd = 5, bg = "green")
        self.playButton.grid(row = 4, column = 2 )

        self.clearButton = Button(self.recordButtons, font = ("ARIAL", 16), text = "Clear Sequence", width = 20, command = self.clearFile,bd = 5, bg = "orange")
        self.clearButton.grid(row = 5, column = 2 )

        self.spacerLabel5 = Label(self.recordButtons,   padx = 100)
        self.spacerLabel5.grid(row = 6, column = 2 )

        #++++++++Looping+++++++++++++++++++
        self.loopingFrame = Frame(self.recordButtons)
        self.loopingFrame.grid(row= 7, column =2)
        
        self.loopStartButton = Button(self.loopingFrame, font = ("ARIAL", 16), text = "Start Loop", width = 9, command = self.startLooper, bd = 5, bg = "green")
        self.loopStartButton.grid(row = 0, column = 0 )
        self.loopStopButton = Button(self.loopingFrame, font = ("ARIAL", 16), text = "Stop Loop", width = 9, command = self.stopLooper, bd = 5, bg = "red")
        self.loopStopButton.grid(row = 0, column = 1 )

        #+++++++++++++++++++++++space frame++++++++++++++++++++++++++++++++++

        self.spacerFrameRight = Frame(self)
        self.spacerFrameRight.grid(row = 0, column = 4 )

        self.spacerLabel7 = Label(self.spacerFrameRight, padx = 10)
        self.spacerLabel7.grid(row = 0, column = 0 )

    def serialSelect(self):
        print "Serial Select"
        #Find the serial port that the arduino is connected to
        self.ports = list(serial.tools.list_ports.comports())
        print self.ports
        for p in self.ports:
            self.portOptions.append(p[0])
            
        self.selection = StringVar()
        self.selection.set("Set USB Port")
        OptionMenu (self.portSelectionFrame, self.selection, *self.portOptions, command = self.assignPort ). grid (row=0, column = 1)

    def assignPort(self, event):
        print "assignPort"
        self.ser = serial.Serial(self.selection.get(), 9600, timeout = .5)
        self.serialStatus = 1
        
    #++++++++++++++++Functions+++++++++++++++++++++++

    def serialNotification(self):
        #warn of the bad serial connection
        print "The is not a port connected"
        if self.notificationShowed == 0:
            tkMessageBox.showerror("Bad Serial Connection", "You are not connected to the Arm USB Port. Please Select a different port")
            self.notificationShowed = 1


    def move_it(self, event):
        print "move it"
        #this function sends the command of joint angles to the arduino to move the servos to the desired positions in real time with the GUI
        
        #serial checks
        if self.serialStatus == 0:  #flag of whether a usb port has been chosen
            print "bad serial"
            self.badSerialCount = self.badSerialCount + 1
            if self.badSerialCount >= 6 and self.guiStarted == 0:      # This check lets the gui load before loading a notification to select the serial
                self.serialNotification()
                self.guiStarted = 1
            if self.badSerialCount == 25:    #this check is for all subsequent ignorning of the warning
                self.serialNotification()
                self.notificationShowed = 0
                self.badSerialCount = 0
            return
            
        self.notificationShowed = 0
        self.ser.flushInput()
        self.ser.flushOutput()
        command = str(self.base.get()) + ',' + str(self.shoulder.get()) + ',' + str(self.elbow.get())+','+ str(self.gripper.get())+','+ str(25 - self.theSpeed.get()) + '\n'
        print command
        self.ser.write(command)

        #wait until a repsonse if found from the arduino
        OK = 'no'
        while (OK != 'd'):
            OK = self.ser.read(1)
        
    def recordArmPos(self):
        #This function records the current positions of the GUI and places them in a TXT file in the same directory as this program
        readPosCommand = str(self.base.get()) + ',' + str(self.shoulder.get()) +   ',' + str(self.elbow.get())+','+ str(self.gripper.get()) +','+ str(25 -self.theSpeed.get())+'\n'
        recordFile = open(self.currentSequence, 'a')
        print (self.currentSequence)
        recordFile.write(readPosCommand)
        recordFile.close()

    def recordPause(self):
        #This function records the current positions of the GUI and places them in a TXT file in the same directory as this program
        pauseCommand = "pause" + '\n'
        recordFile = open(self.currentSequence, 'a')
        recordFile.write(pauseCommand)
        recordFile.close()    

    def playback(self):
       #This function reads the record file created in recordArmPos() and send the commands to the arm so that a sequence may be repeated.
       recordFile = open(self.currentSequence, 'r')
       Count = 1
       for line in recordFile:
           Count = Count + 1
           self.recordedCommand = line

           #send the command to the arduino using another function
           self.sendCommand()
       print ('Done')
       recordFile.close()

    def sendCommand(self):
        #this is a basic command function. It recieves the generic command in the form base,shoulder,elbow,effector\n and send it to the arduino and then waits to recieve confirmations that the arduino has processed it.
        #this function is a variation of move_it for the playback function
        if self.serialStatus == 0:
            self.serialNotification()
            return  
        self.notificationShowed = 0
        self.ser.flushInput()
        self.ser.flushOutput()
        theCommand = self.recordedCommand
        print theCommand

        if theCommand == "pause\n":
            time.sleep(1)
            return
        
        self.ser.write(theCommand)

        #wait until a repsonse if found from the arduino
        OK = 'no'
        while (OK != 'd'):
            OK = self.ser.read(1)
                
    def goHome(self):
        #This function returns the robot to its initial positions and changed the GUI positions to match
        homePos = str(108) + ',' + str(90) + ',' + str(100)+ ',' + str(10) + ',' + str(4) + '\n'
        self.base.set(108)
        self.shoulder.set(90)
        self.elbow.set(100)
        
        self.recordedCommand = homePos
        self.sendCommand()
        
    def clearFile(self):
        #this clears the file for a new sequence
        open(self.currentSequence, 'w').close()
        
    def saveFileAs(self):
        #This function is called by the menubar
        #this function opens the current set of commands in the file motion_recording.txt and saves the contents to a new
        print "Saving a File I see"
        
        #open the current file and copy its contents
        file = open(self.currentSequence, 'r')   
        textoutput = file.readlines()
        file.close()
        
        #open the new files and insert the contents
        theNewFile = tkFileDialog.asksaveasfilename(initialfile='Untitled.txt',defaultextension=".txt",filetypes=[("All Files","*.*"),("Text Documents","*.txt")])

        file = open(theNewFile, 'w')
        file.writelines(textoutput)		#not the writelines. write does not enter the data correctly from readlines
        file.close()
        
        #update the working file
        self.currentSequence = theNewFile	#update the file that is being used universally

    def openFile(self):
        #this function sets the file that is being edited and recorded into
        self.currentSequence = tkFileDialog.askopenfilename(initialdir = "/",title = "Select file",filetypes = (("txt files","*.txt"),("all files","*.*")))
        print (currentSequence)
        
    def newFile(self):
        #this function created a new .txt file to hold imput commands
        #open a new fle 
        theNewFile = tkFileDialog.asksaveasfilename(initialfile='Untitled.txt',defaultextension=".txt",filetypes=[("All Files","*.*"),("Text Documents","*.txt")])	#names the file and sets the location
        file = open(theNewFile, 'a')   #creates the file
        file.close()
        
        self.currentSequence = theNewFile	#update the file that is being used universally

    def looper( self):
        print "looper"
        #this function loops through a the current sequence repeatedly.
        #startStop is the boolean bit that stats looping
        if self.loopStartStop == 1:
            self.playback()
            print "in looper loop"
        root.after(1000, self.looper)	

    def startLooper(self):
        print "start looper"
        #global loopStartStop
        self.loopStartStop = 1

    def stopLooper(self):
    
        #global loopStartStop	
        self.loopStartStop = 0
	
#++++++++++++++++++++The GUI++++++++++++++++++++++
root = Tk()
root.wm_title("LittleArm")

root.configure(background  = "#66ff33")

#+++++++++++++++++++++++++++Primary Loop+++++++++++++++++
app = Application(root)

root.mainloop()


