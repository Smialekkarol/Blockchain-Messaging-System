g++ WebSocketServer.cpp  -o WebSocketServer -L /usr/lib/ -lboost_system -lboost_thread -lpthread

g++ HTTPServer.cpp  -o HTTPServer  -lboost_system -lboost_thread -lpthread -lamqpcpp -lev

g++ Gui.cpp `wx-config --cxxflags --libs` -o gui