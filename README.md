# Lab1

去模擬Periodic tasks，並輸出tasks執行的狀況

### uCOS-II.H
* 增加一個PRINT_BUFFER的Struct，用來儲存要print的訊息，以circular-queue儲存
* 增加一個TASK_INFO的Struct，用來儲存每個Task的compTime,period,deadline

### OS_CORE.C
* 在**OS_Sched**中如果要Context Switch就將訊息放到Buffer中
* 在**OSIntExit**中如果要Context Switch就將訊息放到Buffer中
* 在**OSTimeTick**中如果執行這個Task，就將其compTime減1，並且會檢查deadline是否到期

### TEST.C
* StartUpTask中建立3個所要的Tasks
* 每個Task各自執行對應的[c,p]，並且會檢查是否要輸出訊息 (檢查Buffer)


# Lab2

實作出EDF Scheduler

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
* 每個Task各自執行對應的[c,p]，並且會檢查是否要輸出訊息 (檢查Buffer)


# Lab3

實作出Ceiling Priority Protocol (CPP)，在uCOS-II中，要先預定義出這個Resource的最高Priority並佔用

在執行CPP的時候會把TASK的Priority去做切換， 修改OSRdyGrp和OSRdyTbl

在原先的uCOS-II中是採用 Priority-Inheritance Protocol (PIP) ，Priority的上升在blocked的時候才觸發

在這次的Lab中改成 CPP，要資源的時候就會觸發


### uCOS-II.H
* 增加一個PRINT_BUFFER的Struct，用來儲存要print的訊息，以circular-queue儲存
* 增加一個TASK_INFO的Struct，用來儲存每個Task的compTime，因為task執行完一次就結束，所以不用特別定義deadline和period，會在TEST.C定義每個task所要的資源和執行時間


### OS_CORE.C
* 在**OS_Sched**中如果要Context Switch就將訊息放到Buffer中
* 在**OSIntExit**中如果要Context Switch就將訊息放到Buffer中 (但這個Lab用不到，所以先註解掉)
* 在**OSTimeTick**中如果執行這個Task，就將其compTime減1

### OS_MUTEX.C
* 在**OSMutexPend**中，改成要使用資源的時候就會把Priority上升


### TEST.C
* StartUpTask中建立3個所要的Tasks
* 每個Task有各自對於所需要的資源之執行時間，並且每次執行都會檢查是否要輸出訊息
* 總共有2個OS_EVENT，在一開始的時候直接佔用priority 1 和 2
* 每個Task根據所要的資源來執行 OSMutexPend() 或 OSMutexPost()來占用或釋放資源
* 釋放的前後會儲存Priority的資訊