# Lab1

### uCOS-II.H
* 增加一個PRINT_BUFFER的Struct，用來儲存要print的訊息，以circular-queue儲存
* 增加一個TASK_INFO的Struct，用來儲存每個Task的compTime,period,deadline

### OS_CORE.C
* 在**OS_Sched**中如果要Context Switch就將訊息放到Buffer中
* 在**OSIntExit**中如果要Context Switch就將訊息放到Buffer中
* 在**OSTimeTick**中如果執行這個Task，就將其compTime減1，並且會檢查deadline是否到期

### TEST.C
* StartUpTask中建立3個所要的Tasks
* 每個Task各自執行對應的[c,p]，並且會檢查是否要輸出訊息


# Lab2

### uCOS-II.H
* 增加一個PRINT_BUFFER的Struct，用來儲存要print的訊息，以circular-queue儲存
* 增加一個TASK_INFO的Struct，用來儲存每個Task的compTime,period,deadline,startTime

### OS_CORE.C
* 在**OS_Sched**中如果要Context Switch就將訊息放到Buffer中，會用EDF來選擇Task，會遍歷OSTCBList中的所有Task，選擇deadline最小的來執行，並且會根據OSTimeGet()是否>=Task的StartTime來確認這個Task是否可以被選取(Sleeping)
* 在**OSIntExit**中如果要Context Switch就將訊息放到Buffer中，會用EDF來選擇Task，會遍歷OSTCBList中的所有Task，選擇deadline最小的來執行，並且會根據OSTimeGet()是否>=Task的StartTime來確認這個Task是否可以被選取(Sleeping)
* 在**OSStart**中用EDF來選擇Task，會遍歷OSTCBList中的所有Task，選擇deadline最小的來執行
* 在**OSTimeTick**中如果執行這個Task，就將其compTime減1

### TEST.C
* StartUpTask中建立3個所要的Tasks
* 每個Task各自執行對應的[c,p]，並且會檢查是否要輸出訊息
