add_ut(TestTimerWheel
  SRC 
    TestTimerWheel.cpp
  LIB
    common
  ORIGS
    TimerWheel
)

add_ut(TestSlotHandler
  SRC 
    TestSlotHandler.cpp
    "${CMAKE_CURRENT_LIST_DIR}/../SlotHandler.cpp"
  LIB
    common
  ORIGS
    SlotHandler
)
