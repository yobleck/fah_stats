from PyQt5 import QtWidgets, QtCore, QtGui;
import sys, os, requests, time, multiprocessing;


def do_thing():
    app = QtWidgets.QApplication(["F@H Stats"]);
    win = QtWidgets.QMainWindow();
    
    win.setAttribute(QtCore.Qt.WA_ShowWithoutActivating); #stops new window from stealing focus
    win.setFocusPolicy(QtCore.Qt.NoFocus); #does this do anything?
    win.setWindowFlags(QtCore.Qt.WindowStaysOnBottomHint | QtCore.Qt.FramelessWindowHint  | QtCore.Qt.Tool); #window always behind,no title bar/taskbar icon
    win.resize(150, 100);
    win.move(0,1340);

    label = QtWidgets.QLabel(win); #label in window
    label.resize(150,100);
    label.setAlignment(QtCore.Qt.AlignTop | QtCore.Qt.AlignLeft);
    label.setStyleSheet("background-color:black;");
    
    source = requests.get("https://api.foldingathome.org/user/yobleck"); #https://api.foldingathome.org/
    html = "";
    if(source.status_code == requests.codes.ok):
        html = source.json();
        name = html["name"];
        score = html["score"];
        wus = html["wus"];
        rank = html["rank"];
    user_count = requests.get("https://api.foldingathome.org/user-count").text;
    
    label.setText(name + "\n" + #str(time.time())[0:11] +
                  "Score: " + f"{score:,}" + "\n" +
                  "WUs: " + f"{wus:,}" + "\n" +
                  "Rank: " + f"{rank:,}" + " / " + f"{int(user_count):,}" + "\n" +
                  "Top: " + str((rank/int(user_count))*100)[0:4] + "%"
                  );
    
    
    #actually show everything on screen
    win.show();
    label.show();
    app.exec_();
    print("test");

while(True): #this is lazy shit
    thread = multiprocessing.Process(target=do_thing); #my brain is really foggy right now from sleep deprivation so I can't handle learing Qthread
    thread.daemon = True;
    thread.start();
    time.sleep(600);
    thread.kill();
    thread.join();
