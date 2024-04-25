# Lab1

### uCOS-II.H
* 增加一個PRINT_BUFFER的Struct，用來儲存要print的訊息，以circular-queue儲存
* 在**os_tcb**中加入compTime,period,deadline的訊息

### OS_CORE.C
* 在**OS_Sched**中如果要Context Switch就將訊息放到Buffer中
* 在**OSIntExit**中如果要Context Switch就將訊息放到Buffer中
* 在**OSTimeTick**中如果執行這個Task，就將其compTime減1，並且會檢查deadline是否到期

### TEST.C
* StartUpTask中建立3個所要的Tasks
* 每個Task各自執行對應的[c,p]，並且會檢查是否要輸出訊息
